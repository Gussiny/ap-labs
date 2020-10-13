package main

import (
	"fmt"
	"log"
	"net"
	"os"
	"strings"
)

func main() {
	if len(os.Args) < 2{
		fmt.Println("ERROR: Missing parameters")
		os.Exit(1)
	}

	channels := make(chan int)
	for i := 1; i < len(os.Args); i++ {
		city := strings.Split(os.Args[i], "=")
		timeZone := city[0]
		port := city[1]
		conn, err := net.Dial("tcp", port)
		if err != nil{
			log.Fatal(err)
		}

		go handleConn(conn, timeZone, channels)
	}
	<-channels
}

func handleConn(conn net.Conn, timeZone string, channels chan int){
	for {
		data := make([]byte, 12)
		_, err := conn.Read(data)

		if err != nil{
			conn.Close()
			log.Fatal(err)
			return
		}

		fmt.Printf("%s\t: %s", timeZone, data)
	}
	channels <- 1
}