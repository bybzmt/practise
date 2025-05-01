package socks

import (
	"bufio"
	"io"
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

func ReadCmd(r io.Reader) (cmd byte, addr RawAddr, err error) {
	resp := [3]byte{}

	if _, err := io.ReadFull(r, resp[:]); err != nil {
		return 0, nil, err
	}

	if resp[0] != SOCKS_VER {
		return 0, nil, ErrVer
	}

	addr, err = ReadRawAddr(r)
	if err != nil {
		return 0, nil, err
	}

	return resp[1], addr, nil
}

var EmptyAddr = RawAddr([]byte{ATYP_IPV4, 0, 0, 0, 0, 0, 0})

func SendCmd(w io.Writer, code byte, addr RawAddr) error {
	if _, err := w.Write([]byte{SOCKS_VER, code, RSV}); err != nil {
		return err
	}

	if addr == nil {
		addr = EmptyAddr
	}

	if _, err := w.Write(addr); err != nil {
		return err
	}

	return nil
}
