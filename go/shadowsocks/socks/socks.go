package socks

import (
	"bufio"
	"net"
)

type socks struct {
	net.Conn
	rw     *dalyWriter
	bind   RawAddr
	auth   *SimpleAuth
	method byte
}

func (s *socks) Read(p []byte) (n int, err error) {
	return s.rw.Read(p)
}

func newSocks(c net.Conn, auth *SimpleAuth) socks {
	rb := bufio.NewReader(c)
	wb := bufio.NewWriter(c)

	method := METHOD_ANONYMOUS
	if auth != nil {
		method = METHOD_USERNAME
	}

	return socks{
		Conn:   c,
		auth:   auth,
		rw:     &dalyWriter{Reader: rb, Writer: wb},
		method: method,
	}
}
