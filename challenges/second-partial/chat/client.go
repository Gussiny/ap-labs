// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"fmt"
	"bufio"
)

//!+
func main() {

	if len(os.Args) < 5 {
		fmt.Println("ERROR: Missing parameters")
		os.Exit(1)
	}

	var user, server string

	if os.Args[1] == "-user" {
		user = os.Args[2]
	} else{
		fmt.Println("ERROR: Missing flag -user")
		os.Exit(1)
	}

	if os.Args[3] == "-server" {
		server = os.Args[4]
	} else{
		fmt.Println("ERROR: Missing flag -server")
		os.Exit(1)
	}

	conn, err := net.Dial("tcp", server)
	if err != nil {
		log.Fatal(err)
	}

	_, er := io.WriteString(conn, user+"\n")
	if er != nil {
		log.Fatal(er)
	}

	done := make(chan struct{})
	go func() {
		input := bufio.NewScanner(conn)
		for input.Scan(){
			fmt.Print("\n" + input.Text())
			fmt.Print("\n" + user + " > ")
		}
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
