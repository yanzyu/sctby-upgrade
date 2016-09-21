package main

import (
	"hash/crc32"
	"os"
	"flag"
	"bytes"
	"encoding/binary"
	"encoding/hex"
	"encoding/json"
	"errors"
	"fmt"
	"github.com/Omegaice/go-xmodem/xmodem"
	"github.com/tarm/serial"
	"io/ioutil"
	"time"
)

type UpdateInfo struct {
	DevEUI    	string  `full:"DevEUI" json:"deveui"`       // the short-address of the device to be updated
	FirmName	string	`full:"FirmName" json:"firmname"`
	FirmAddr	string	`full:"FirmAddr" json:"firmaddr"`
	FirmSize	uint32	`full:"FirmSize" json:"firmsize"`
}

type SerialSender struct {
	port *serial.Port
}

const (
	SerialFatal = iota
	SerialOk
	SerialTimeout
)

const (
	RF_HEADER = 0xaa
	SN_REQ    = 0x01
	GW_RESP   = 0x02
	RF_DATA   = 0x03
)

const (
	PktSize = 128
	RtNum   = 5
	Pad     = 0x1A
)

// xmodem command
const (
	SOH = 0x01
	EOT = 0x04
	ACK = 0x06
	NAK = 0x15
	CAN = 0x18
)



// COM is the serial port of uart
var ConfigFile string
func readConfig() (info UpdateInfo) {
	config, err := ioutil.ReadFile(ConfigFile)
	if err != nil {
		fmt.Printf("file:%s, error:%s\n", ConfigFile, err.Error())
		return
	}
	err = json.Unmarshal(config, &info)
	if err != nil {
		fmt.Println(err.Error())
		return
	}
	return info
}

// wait ota request
func (s *SerialSender) waitForReq(info UpdateInfo) bool {
	addr, err := hex.DecodeString(info.DevEUI)
	if err != nil {
		fmt.Println(err.Error())
		return false
	}
	cmd, err := s.read(addr, 7, time.Second*10)
	if err != nil {
		fmt.Println(err.Error())
		return false
	}
	if len(cmd) < 6 {
		fmt.Println("not receive request")
		return false
	}

	return true;
}

func (s *SerialSender) respSN(info UpdateInfo, crc uint32) {
	addr, err := hex.DecodeString(info.DevEUI)
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	payload := []byte{}
	payload = append(payload, 0x01)
	firmAddr, err := hex.DecodeString(info.FirmAddr)
	if err != nil {
		fmt.Println(err.Error())
		return
	}
	payload = append(payload, firmAddr...)
	firmSize := make([]byte, 4)
	binary.BigEndian.PutUint32(firmSize, info.FirmSize)
	payload = append(payload, firmSize...)
	Crc := make([]byte, 4)
	binary.BigEndian.PutUint32(Crc, crc)
	payload = append(payload, Crc...)

	s.send(GW_RESP, payload, addr)
}



// COM is the serial port of uart
var COM string
// Sender is a uart Sender
var Sender *SerialSender

func main() {
	
	flag.StringVar(&COM, "com", "COM5", "get the serial com")
	flag.StringVar(&ConfigFile, "config", "config.json", "get the config")
	flag.Parse()
	// step 1: read the config file
	info := readConfig()
	// step 2: read the firmware filef
	fmt.Printf("firmware name:%s\n", info.FirmName)
	firm, err := readFirm8(info.FirmName)
	if err != nil {
		fmt.Printf("file:%s, error:%s\n", info.FirmName, err.Error())
		return
	}
	crc := crc32.ChecksumIEEE(firm)
	info.FirmSize = uint32(len(firm))
	fmt.Printf("size:%4x, crc:%4x\n", info.FirmSize, crc)
	// step 3: open the COM
	config := &serial.Config{
		Name:        COM,
		Baud:        9600,
		ReadTimeout: time.Millisecond * 100,
	}
	port, err := serial.OpenPort(config)
	if err != nil {
		fmt.Println("failed to open serial port")
		return
	}
	Sender = new(SerialSender)
	Sender.port = port
	// step 4 : ota upgrade
	for {
		// wait for sensor request ota
		if Sender.waitForReq(info) {
			// step 5: response the sensor
			Sender.respSN(info, crc)
			if Sender.xmodem(firm, info) {
				fmt.Println("xmodem transfer complete")
			}
		}
	}
}



func (s *SerialSender) xmodem(buf []byte, info UpdateInfo) bool {
	addr, err := hex.DecodeString(info.DevEUI)
	if err != nil {
		fmt.Println(err.Error())
	}
	fmt.Printf("device address = %s\n", info.DevEUI)

	rtNum := 0;
	fmt.Println("\nwaite for stating xmodem transfer...")
	// step 2, wait for a xmodem start character
	for {
		rtNum ++
		if rtNum > 5 {
			return false
		}
		//n, err := s.port.Read(cmd)
		cmd, err := s.read(addr, 1, time.Second*2)

		if err != nil {
			fmt.Println(err.Error())
			return false
		}

		//if n == 1 && cmd[0] == 'C' {
		if len(cmd) == 1 && cmd[0] == 'C' {
			break
		}
	}

	fmt.Printf("\nstart xmodem transfer...\n")
	// step 3, send the file to sensor
	pktnum := byte(1)
	rttime := 0
	for {
		rttime++
		if rttime >= RtNum {
			fmt.Println("retransmission more than 5 times")
			break
		}

		if (int(pktnum) * PktSize) > len(buf) {
			s.xmodemSend(buf[int(pktnum-1)*PktSize:len(buf)], pktnum, addr)
		} else {
			s.xmodemSend(buf[int(pktnum-1)*PktSize:int(pktnum)*PktSize], pktnum, addr)
		}

		//n, err := s.port.Read(cmd)
		cmd, err := s.read(addr, 1, time.Second*10)
		if err != nil {
			fmt.Println(err.Error())
			return false
		}
		if len(cmd) > 0 && cmd[0] == CAN {
			fmt.Println("the receiver end the transfer")
			break
		}

		if len(cmd) > 0 && cmd[0] == NAK {
			fmt.Printf("retranmission packet %d\n", pktnum)
			continue
		}

		if len(cmd) > 0 && cmd[0] == ACK {
			rttime = 0
			fmt.Printf("send %d packet sucessfully\n", pktnum)
			if int(pktnum)*PktSize >= len(buf) {
				fmt.Println("end of transmission")
				s.sendCmd([]byte{EOT}, addr)
				break
			}
			pktnum++
			continue
		}

		fmt.Println("unknown error")
		continue
	}
	return true
}

func (s *SerialSender) send(frameFype byte, data []byte, devaddr []byte) error {
	payload := []byte{}

	payload = append(payload, byte(RF_HEADER)) 	// add header
	payload = append(payload, devaddr...)      	// add device address
	payload = append(payload, frameFype)   		// add the frame category
	payload = append(payload, byte(len(data)))	// add length
	payload = append(payload, data...)			// add data
	if frameFype != RF_DATA {
		crc := xmodem.CRC16(data)
		payload = append(payload, byte(crc >> 8))
		payload = append(payload, byte(crc))
	}

	n, err := s.port.Write(payload)
	if err != nil {
		return err
	}
	if n != len(payload) {
		return errors.New("send number error")
	}
	if len(data) < 50 {
		fmt.Printf("send\t")
		printHex(payload)
	}
	return nil
}

func (s *SerialSender) xmodemSend(data []byte, num byte, devaddr []byte) (err error) {
	// construct the paylaod
	payload := []byte{}
	buf := make([]byte, PktSize)

	// step 2, add SOH and pakcet number
	payload = append(payload, byte(SOH))
	payload = append(payload, num)
	payload = append(payload, ^num)

	// step 3, add the data
	copy(buf, data)
	if len(data) < PktSize {
		for i := len(data); i < PktSize; i++ {
			buf[i] = byte(Pad)
		}
	}
	payload = append(payload, buf...)

	// step4, add crc16
	crc := xmodem.CRC16(buf)
	crc16 := make([]byte, 2)
	binary.BigEndian.PutUint16(crc16, crc)
	payload = append(payload, crc16...)

	return s.send(RF_DATA, payload, devaddr)
}

func (s *SerialSender) sendCmd(cmd []byte, devaddr []byte) (err error) {

	fmt.Println("enter command send")
	// construct the paylaod
	payload := []byte{}

	payload = append(payload, cmd...) // add command

	return s.send(RF_DATA, payload, devaddr)
}

func (s *SerialSender) read(addr []byte, length int, timeout time.Duration) ([]byte, error) {
	data := []byte{}
	buf := make([]byte, 256)
	ch := make(chan int, 1)

	var err error
	var n int

	flag := false
	timeoutCh := time.After(timeout)

	go func() {
		size := 0
		for {
			n, err = s.port.Read(buf)
			if err != nil {
				fmt.Println(err.Error())
				ch <- SerialFatal
				return
			}

			// if n > 0 {
			// 	data = append(data, buf[0:n]...)
			// 	size += n
			// }
			data = append(data, buf[0:n]...)
			size += n

			// if not device specific, just return the whole received data
			if addr == nil && size >= length {
				ch <- SerialOk
				break
			}

			if size >= (length + 11) {
				// check the packet
				// if checkPacket(addr, data[0:size], RF_DATA) != true {
				// 	size = 0
				// 	data = []byte{}
				// 	continue
				// }
				ch <- SerialOk
				break
			}
			if flag {
				close(ch)
				break
			}
		}
	}()

	select {
	case state := <-ch:
		if state == SerialOk {
			err = nil
		}
	case <-timeoutCh:
		flag = true
		err = nil
	}

	if addr == nil {
		return data, err
	}

	if (len(data) > 0 && length < 133) {
		fmt.Printf("receive\t")
		printHex(data)
	}

	if len(data) > 11 {
		return data[11:len(data)], err
	}
	
	return []byte{}, err
}

func checkPacket(addr []byte, data []byte, flag byte) bool {

	fmt.Printf("receive %d data: ", len(data))
	for _, c := range data {
		fmt.Printf("%x ", c)
	}
	fmt.Println()

	if data[0] != RF_HEADER {
		fmt.Println("header wrong")
		return false
	}

	if bytes.Equal(addr, data[1:9]) != true {
		fmt.Println("addr wrong")
		fmt.Println(addr)
		fmt.Println(data)

		return false
	}

	if data[9] != flag {
		fmt.Println("flag wrong")
		return false
	}

	return true
}

func readFirm8(entry string) (firm []byte, err error) {

	// open the file
	f, err := os.Open(entry)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer f.Close()

	// read the file
	rawBinary, err := ioutil.ReadAll(f)

	return rawBinary, err
}

func printHex(data []byte) {
	fmt.Printf("%d\tbytes:", len(data))
	for _, d := range data {
		fmt.Printf("%02x ", d)
	}
	fmt.Println()
}

