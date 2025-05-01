package socks

import (
	"io"
	"net"
)

type Client struct {
	socks
}

func NewClient(c net.Conn, auth *SimpleAuth) *Client {
	return &Client{socks: newSocks(c, auth)}
}

func (s *Client) Dial(addr RawAddr) error {
	if _, err := s.rw.Write([]byte{VER, 1, s.method}); err != nil {
		return err
	}

	if s.auth != nil {
		if err := writeAuth(s.rw, s.auth); err != nil {
			return err
		}
	}

	if _, err := s.rw.Write([]byte{VER, CMD_CONNECT, RSV}); err != nil {
		return err
	}

	if _, err := s.rw.Write(addr); err != nil {
		return err
	}

	resp := [3]byte{}

	if _, err := io.ReadFull(s.rw, resp[:2]); err != nil {
		return err
	}

	if resp[0] != VER {
		return errVer
	} else if resp[1] != s.method {
		return errMethodFail
	}

	if s.auth != nil {
		if _, err := io.ReadFull(s.rw, resp[:2]); err != nil {
			return err
		}

		if resp[0] != AUTH_VER {
			return errAuthVer
		} else if resp[1] != SUCCESS {
			return errAuth
		}
	}

	if _, err := io.ReadFull(s.rw, resp[:]); err != nil {
		return err
	}

	if resp[0] != VER {
		return errVer
	} else if resp[1] != SUCCESS {
		return errMethodFail
	}

	var err error
	s.bind, err = ReadRawAddr(s.rw)
	if err != nil {
		return err
	}

	return nil
}
