package main

import (
	"fmt"
	"time"
)

var timeLimit = 5

func main() {
	var commsPerSecond int

	totalComms := 0

	in := make(chan int)
	out := make(chan int)
	start := time.Now()

	go comm(in, out)
	go comm(out, in)

	for timePass := 0; timePass < timeLimit; timePass++ {
		in <- 0
		time.Sleep(time.Duration(1) * time.Second)
		totalComms += <-in
	}

	commsPerSecond = totalComms / timeLimit

	fmt.Println("Communications Per Second : ", commsPerSecond)
	fmt.Println("TOTAL : ", totalComms, "/", time.Since(start))
}

func comm(in chan int, out chan int) {
	for {
		out <- (1 + <-in)
	}
}
