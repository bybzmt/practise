package socks

import (
	"net"
)

type Client struct {
	socks
}

func NewClient(c net.Conn, auth *SimpleAuth) *Client {
	return &Client{socks: newSocks(c, auth)}
}

func (s *Client) Dial(addr RawAddr) error {
	if _, err := s.rw.Write([]byte{SOCKS_VER, 1, s.method}); err != nil {
		return err
	}

	if s.auth != nil {
		if err := sendAuthReq(s.rw, s.auth); err != nil {
			return err
		}
	}

	err := SendCmd(s.rw, CMD_CONNECT, addr)
	if err != nil {
		return err
	}

	err = checkMethodResp(s.rw, s.method)
	if err != nil {
		return err
	}

	if s.auth != nil {
		if err := checkAuthResp(s.rw); err != nil {
			return err
		}
	}

	cmd, bind, err := ReadCmd(s.rw)
	if err != nil {
		return err
	}

	if cmd != SUCCESS {
		return ErrCmd
	}

	s.bind = bind

	return nil
}
