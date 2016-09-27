package main

import (
    "os"
    "fmt"
    "flag"
    "io/ioutil"
    "hash/crc32"
    "encoding/binary"
)

// AppEntry is the start address of the application
var AppEntry uint32
// AppSize is the size of application by bytes
var AppSize uint32
// AppCheck is the checksum of application firmware, using CRC32
var AppCheck uint32

func main() {
    fileName := flag.String("firmware", "imag.bin", "the file name of application firmware")
    outputName := flag.String("output", "APP_INFO.bin", "the name of output file")
    AppEntry := flag.Uint64("entry", 0x08005000, "start address of the application")
    flag.Parse()
    
    /* (1) read the firmware */
    firmware, err := readFirm(*fileName)
    if err != nil {
        fmt.Println(err.Error())
        return
    }
    _ = outputName
    /* (2) calculate the crc checksum */
    AppSize = uint32(len(firmware))
    AppCheck = crc32.ChecksumIEEE(firmware)

    /* (3) generate the app_info file */
    buf := make([]byte, 4)
    data := []byte{}
    binary.LittleEndian.PutUint32(buf, uint32(*AppEntry))
    data = append(data, buf...)
    for _, c := range buf {
        fmt.Printf("%02x", c)
    }
    fmt.Println()
    binary.LittleEndian.PutUint32(buf, AppSize)
    data = append(data, buf...)
    for _, c := range buf {
        fmt.Printf("%02x", c)
    }
    fmt.Println()
    binary.LittleEndian.PutUint32(buf, AppCheck)
    data = append(data, buf...)
    for _, c := range buf {
        fmt.Printf("%02x", c)
    }
    fmt.Println()

    f, err := os.OpenFile(*outputName, os.O_CREATE|os.O_WRONLY, 0666)
    if err != nil {
        fmt.Println(err.Error())
        return
    }
    defer f.Close()

    n, err := f.Write(data)
    if err != nil {
        fmt.Println(err.Error())
        return
    }
    if n != 12 {
        fmt.Println("not write enough data")
    }

}

func readFirm(entry string) (firm []byte, err error) {

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

