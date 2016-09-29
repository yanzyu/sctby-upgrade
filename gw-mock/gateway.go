package main

import (
	"hash/crc32"
	"os"
	"flag"
	"bytes"
	"encoding/hex"
	"encoding/binary"
	"encoding/json"
	"errors"
	"fmt"
	"github.com/Omegaice/go-xmodem/xmodem"
	"github.com/tarm/serial"
	"io/ioutil"
	"time"
)


type Module_t struct {
	Name		string  `full:"Name" json:"name"`
	Addr		string	`full:"Addr" json:"addr"`
	Data		[]byte	`full:"Data" json:"data, omitempty"`
	Size		uint32	`full:"Size" json:"size, omitempty"`
	Crc32		uint32	`full:"Crc32" json:"crc32, omitempty"`
}

type Sensor_t struct {
	DevEUI		string	 	`full:"DevEUI" json:"deveui"`
	Time		time.Time	`full:"Time" json:"time, omitempty"`	// not add time out now
	TimeOut		time.Time	`full:"TimeOut" json:"timeout, omitempty"`	// not add time out now
	Status		int			`full:"Status" json:"status, omitempty"`
}

type Task_t struct {
	Version		string		`full:"Version" json:"version"`
	Address 	string		`full:"Address" json:"address"`
	Size		string		`full:"Size" json:"size"`
	Crc32		string		`full:"Crc32" json:"crc32"`
	Module		[]Module_t	`full:"Module" json:"module"`
	Sensor		[]Sensor_t	`full:"Sensor" json:"sensor"`
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
	CMD_UPGRADE = 0x01
	CMD_JUMP  	= 0x02
	CMD_SLEEP   = 0x03
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
func readConfig() (task Task_t, err error) {
	config, err := ioutil.ReadFile(ConfigFile)
	if err != nil {
		fmt.Printf("file:%s, error:%s\n", ConfigFile, err.Error())
		return Task_t{}, err
	}
	err = json.Unmarshal(config, &task)
	if err != nil {
		fmt.Println(err.Error())
		return Task_t{}, err
	}
	// for _, s := range task.Sensor {
	// 	s.Status = 0
	// }
	/* for test */
	fmt.Println("received an ota task")
	fmt.Printf("version:%s, address:%s, crc32:%s\n", task.Version, task.Address, task.Crc32)
	fmt.Println("Infomation of modules to be upgrade:")
	for _, m := range task.Module {
		fmt.Printf("name:%s\taddress:%s\n", m.Name, m.Addr)
	}
	fmt.Println("Infomation of sensors to be upgrade:")
	for _, s := range task.Sensor {
		fmt.Printf("DevEUI:%s\n", s.DevEUI)
	}

	return task, nil
}

// wait ota request
func (s *SerialSender) waitForReq(task *Task_t) (bool, int) {

	cmd, err := s.read(nil, 7, time.Second*10)
	if err != nil {
		fmt.Println(err.Error())
		return false, 0
	}
	if len(cmd) < 14 {
		fmt.Println("not receive request")
		return false, 0
	}

	devEui := cmd[1:9]
	flag := cmd[9]
	version := cmd[12:16]
	if cmd[0] != RF_HEADER {
		return false, 0
	}
	if flag != SN_REQ {
		return false, 0
	}
	ok, index := hasSensor(task, devEui)
	ver, err := hex.DecodeString(task.Version)
	if err != nil {
		fmt.Println(err.Error())
		return false, 0
	}
	if !ok {
		fmt.Printf("receive request from 0x")
		for _, c := range devEui {
			fmt.Printf("%02x", c)
		}
		fmt.Println(", which is not in upgrade task")
		return false, 0
	}
	if bytes.Compare(version, ver) != 0 {
		fmt.Printf("receive request of version 0x")
		for _, c := range version {
			fmt.Printf("%02x", c)
		}
		fmt.Println(", which is not target version")
		return false, 0
	}
	
	return true, index
}

func (s *SerialSender) respSN(task *Task_t, index int) bool {
	/* (1) check whether complete */
	devEui, err := hex.DecodeString(task.Sensor[index].DevEUI)
	if err != nil {
		fmt.Println(err.Error())
		return  false
	}
	payload := []byte{}
	if task.Sensor[index].Status >= len(task.Module) {
		// jump to application
		payload = append(payload, CMD_JUMP)
		appAddr, _ := hex.DecodeString(task.Address)
		payload = append(payload, appAddr...)
		appSize, _ := hex.DecodeString(task.Size)
		payload = append(payload, appSize...)
		appCrc, _ := hex.DecodeString(task.Crc32)
		payload = append(payload, appCrc...)
		s.send(GW_RESP, payload, devEui)
		return false
	}
	//payload = append(payload, 0x01)
	payload = append(payload, CMD_UPGRADE)
	addr, err := hex.DecodeString(task.Module[task.Sensor[index].Status].Addr)
	for err != nil {
		fmt.Println(err.Error())
		return false
	}
	payload = append(payload, addr...)
	buf := make([]byte, 4)
	binary.BigEndian.PutUint32(buf, task.Module[task.Sensor[index].Status].Size)
	fmt.Printf("module[%d] size = %x\tcrc32 = %x\n", task.Sensor[index].Status, task.Module[task.Sensor[index].Status].Size, task.Module[task.Sensor[index].Status].Crc32)
	payload = append(payload, buf...)
	binary.BigEndian.PutUint32(buf, task.Module[task.Sensor[index].Status].Crc32)
	payload = append(payload, buf...)

	s.send(GW_RESP, payload, devEui)
	return true
}

// COM is the serial port of uart
var COM string
// Sender is a uart Sender
var Sender *SerialSender
// Sender is a uart Sender
var BaudRate int

func main() {
	
	flag.StringVar(&COM, "com", "COM5", "get the serial com")
	flag.IntVar(&BaudRate, "baudrate", 9600, "get the baudrate")
	flag.StringVar(&ConfigFile, "config", "config.json", "get the config")
	flag.Parse()
	// step 1: read the config file
	task, err := readConfig()
	if err != nil {
		fmt.Println(err.Error())
		return
	}
	// step 2: read the firmware file
	if readFirm(&task) != true {
		return
	}

	// step 3: open the COM
	config := &serial.Config{
		Name:        COM,
		Baud:        BaudRate,
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
		ok, index := Sender.waitForReq(&task)
		if ok {
			// step 5: response the sensor
			if Sender.respSN(&task, index) {
				fmt.Printf("start upgrade module[%d]:%s\n", task.Sensor[index].Status, task.Module[task.Sensor[index].Status].Name)
				if Sender.xmodem(&task, index) {
					task.Sensor[index].Status++
					fmt.Println("xmodem transfer complete")
				} else {
					fmt.Println("xmodem transfer error")	
				}	
			}
		}
	}
}



func (s *SerialSender) xmodem(task *Task_t, index int) bool {
	addr, err := hex.DecodeString(task.Sensor[index].DevEUI)
	if err != nil {
		fmt.Println(err.Error())
		return false
	}
	buf := task.Module[task.Sensor[index].Status].Data
	fmt.Printf("upgrade module %d of sensor %d", task.Sensor[index].Status, index)

	rtNum := 0;
	fmt.Println("waite for stating xmodem transfer...")
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
	rtNum = 0
	for {
		rtNum++
		if rtNum >= RtNum {
			fmt.Println("retransmission more than 5 times")
			break
		}

		fmt.Printf("send packet:%d\n", pktnum)
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
			rtNum = 0
			fmt.Printf("send %d packet sucessfully\n", pktnum)
			if int(pktnum)*PktSize >= len(buf) {
				fmt.Println("end of transmission")
				s.sendCmd([]byte{EOT}, addr)
				break
			}
			pktnum++
			continue
		}

		fmt.Println("not received data")
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

			data = append(data, buf[0:n]...)
			size += n

			// if not device specific, just return the whole received data
			if addr == nil && size >= length {
				ch <- SerialOk
				break
			}

			if size >= (length + 11) {
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

	fmt.Printf("receive\t")
	printHex(data)

	if addr == nil {
		return data, err
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

func readFirmByte(entry string) (firm []byte, err error) {

	// open the file
	f, err := os.Open(entry)
	if err != nil {
		fmt.Println(err)
		return []byte{}, err
	}
	defer f.Close()

	// read the file
	rawBinary, err := ioutil.ReadAll(f)

	return rawBinary, err
}

func readFirm(task *Task_t) bool {
	var err error
	//for i, t := range task.Module {
	for i := 0; i < len(task.Module); i++ {
		task.Module[i].Data, err = readFirmByte(task.Module[i].Name)
		if err != nil {
			fmt.Println(err.Error())
			return false
		}
		if task.Module[i].Data == nil {
			fmt.Println("Read empty firmware file.")
			return false
		}
		task.Module[i].Size = uint32(len(task.Module[i].Data))
		task.Module[i].Crc32 = crc32.ChecksumIEEE(task.Module[i].Data)
		fmt.Printf("module[%d] size = %x, crc32 = %x\n", i, task.Module[i].Size, task.Module[i].Crc32)
	}
	return true
}

func hasSensor(task *Task_t, DevEUI	[]byte) (bool, int) {
	if len(DevEUI) != 8 {
		return false, 0
	}
	
	for i := 0; i < len(task.Sensor); i++ {
		devEui, err := hex.DecodeString(task.Sensor[i].DevEUI)
		if err != nil {
			fmt.Println(err.Error())
			return false, 0
		}
		if bytes.Compare(DevEUI, devEui) == 0 	{
			return true, i
		}
	}
	return false, 0
}

func printHex(data []byte) {
	fmt.Printf("%d\tbytes:", len(data))
	for _, d := range data {
		fmt.Printf("%02x ", d)
	}
	fmt.Println()
}

