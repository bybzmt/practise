package socks

import (
	"errors"
	"io"
)

var (
	ErrVer          = errors.New("socks version not supported")
	ErrMethod       = errors.New("socks only support 1 method now")
	ErrMethodFail   = errors.New("socks method negotiation failed")
	ErrAuthVer      = errors.New("socks server does not support user/password version 1")
	ErrAuth         = errors.New("socks user/password login failed")
	ErrReqExtraData = errors.New("socks request get extra data")
	ErrCmd          = errors.New("socks command not supported")
	ErrAddrType     = errors.New("socks addr type not supported")
)

const (
	SOCKS_VER        byte = 5
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

// const EmptyAddr = RawAddr([]byte{ATYP_IPV4, 0, 0, 0, 0, 0, 0})
type SimpleAuth struct {
	Username string
	Password string
}

func sendAuthReq(w io.Writer, s *SimpleAuth) error {
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

func sendAuthResp(w io.Writer, ok bool) error {
	code := SUCCESS
	if !ok {
		code = FAIL
	}

	if _, err := w.Write([]byte{AUTH_VER, code}); err != nil {
		return err
	}

	return nil
}

func readAuth(r io.Reader) (*SimpleAuth, error) {
	req := [2]byte{}

	if _, err := io.ReadFull(r, req[:]); err != nil {
		return nil, err
	}

	if req[0] != AUTH_VER {
		return nil, ErrAuthVer
	}

	user := make([]byte, req[1])
	if _, err := io.ReadFull(r, user); err != nil {
		return nil, err
	}

	if _, err := io.ReadFull(r, req[:1]); err != nil {
		return nil, err
	}

	pass := make([]byte, req[0])
	if _, err := io.ReadFull(r, pass); err != nil {
		return nil, err
	}

	a := &SimpleAuth{
		Username: string(user),
		Password: string(pass),
	}

	return a, nil
}

func checkAuthResp(r io.Reader) error {
	resp := [2]byte{}

	if _, err := io.ReadFull(r, resp[:]); err != nil {
		return err
	}

	if resp[0] != AUTH_VER {
		return ErrAuthVer
	} else if resp[1] != SUCCESS {
		return ErrAuth
	}

	return nil
}

func checkMethodResp(r io.Reader, method byte) error {
	resp := [2]byte{}

	if _, err := io.ReadFull(r, resp[:]); err != nil {
		return err
	}

	if resp[0] != SOCKS_VER {
		return ErrVer
	} else if resp[1] != method {
		return ErrMethodFail
	}

	return nil
}
