package socks

import (
	"bytes"
	"errors"
	"io"
)

var (
	errVer          = errors.New("socks version not supported")
	errMethod       = errors.New("socks only support 1 method now")
	errMethodFail   = errors.New("socks method negotiation failed")
	errAuthVer      = errors.New("socks server does not support user/password version 1")
	errAuth         = errors.New("socks user/password login failed")
	errReqExtraData = errors.New("socks request get extra data")
	errCmd          = errors.New("socks command not supported")
	errAddrType     = errors.New("socks addr type not supported")
)

const (
	VER              byte = 5
	METHOD_ANONYMOUS byte = 0
	METHOD_USERNAME  byte = 2
	METHOD_INVALID   byte = 0xff
	CMD_CONNECT      byte = 1
	CMD_BIND         byte = 2
	CMD_UDP          byte = 3
	AUTH_VER         byte = 1
	SUCCESS          byte = 0
	ATYP_IPV4        byte = 1
	ATYP_DOMAINNAME  byte = 3
	ATYP_IPV6        byte = 4
	RSV              byte = 0
	FAIL             byte = 1
	ERR_FORBIDDEN    byte = 2
	ERR_NET          byte = 3
	ERR_DOMAIN       byte = 4
	ERR_REJECT       byte = 5
	ERR_ADDR         byte = 8
)

type SimpleAuth struct {
	Username string
	Password string
}

func writeAuth(w io.Writer, s *SimpleAuth) error {
	if _, err := w.Write([]byte{AUTH_VER}); err != nil {
		return err
	}

	if _, err := w.Write([]byte{byte(len(s.Username))}); err != nil {
		return err
	}

	if _, err := w.Write([]byte(s.Username)); err != nil {
		return err
	}

	if _, err := w.Write([]byte{byte(len(s.Password))}); err != nil {
		return err
	}

	if _, err := w.Write([]byte(s.Password)); err != nil {
		return err
	}

	return nil
}

func checkAuth(r io.Reader, s *SimpleAuth) error {
	req := [2]byte{}

	if _, err := io.ReadFull(r, req[:]); err != nil {
		return err
	}

	if req[0] != AUTH_VER {
		return errAuthVer
	}

	user := make([]byte, req[1])
	if _, err := io.ReadFull(r, user); err != nil {
		return err
	}

	if _, err := io.ReadFull(r, req[:1]); err != nil {
		return err
	}

	pass := make([]byte, req[0])
	if _, err := io.ReadFull(r, pass); err != nil {
		return err
	}

	if !bytes.Equal([]byte(s.Username), user) ||
		!bytes.Equal([]byte(s.Password), pass) {
		return errAuth
	}

	return nil
}
