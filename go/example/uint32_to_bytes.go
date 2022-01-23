package main

import (
	"log"
	"unsafe"
)

func main() {
	var a uint32 = 1

	b := (*[4]int)(unsafe.Pointer(&a))

	log.Println(a)
	log.Println(b[0])
}
