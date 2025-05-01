package socks

import (
	"bufio"
	"net"
	"strconv"
)

type dalyWriter struct {
	*bufio.Reader
	*bufio.Writer
}

func (b *dalyWriter) Read(p []byte) (n int, err error) {
	if err := b.Flush(); err != nil {
		return 0, err
	}

	return b.Reader.Read(p)
}

func splitHostPort(addr string) (host string, port uint16, err error) {
	host, portStr, err := net.SplitHostPort(addr)
	if err != nil {
		return "", 0, err
	}
	portInt, err := strconv.ParseUint(portStr, 10, 16)
	if err != nil {
		return "", 0, err
	}
	port = uint16(portInt)
	return
}
