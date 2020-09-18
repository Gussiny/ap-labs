package main

import (
	"golang.org/x/tour/wc"
	"strings"
)

func WordCount(s string) map[string]int {
	data := make(map[string]int)
	for _, val := range strings.Fields(s) {
		data[val]++;
	}
	return data
}

func main() {
	wc.Test(WordCount)
}
