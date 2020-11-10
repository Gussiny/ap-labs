package main

import (
	"fmt"
	"time"
)

var waitChan = make(chan struct{})

func pipeline(in chan struct{}, out chan struct{}, current int, total int) {
	if current <= total {
		go pipeline(out, in, current+1, total)
		out <- <-in
	} else {
		waitChan <- struct{}{}
	}
}

func main() {
	var maxStages = 200000
	var startTime = time.Now()

	in := make(chan struct{})
	out := make(chan struct{})

	go pipeline(in, out, 0, maxStages)
	<-waitChan

	transitTime := time.Since(startTime)
	fmt.Println("Maximum number of pipeline stages   : ", maxStages)
	fmt.Println("Time to transit trough the pipeline : ", transitTime)
}
