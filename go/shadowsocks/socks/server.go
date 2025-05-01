package socks

import (
	"bytes"
	"io"
	"net"
)

type Server struct {
	socks
}

func NewServer(c net.Conn, auth *SimpleAuth) *Server {
	s := &Server{socks: newSocks(c, auth)}
	return s
}

func (s *Server) HandShake() (RawAddr, error) {
	if err := s.checkMethod(); err != nil {
		return nil, err
	}

	if s.auth != nil {
		if err := s.checkAuth(); err != nil {
			return nil, err
		}
	}

	cmd, addr, err := ReadCmd(s.rw)
	if err != nil {
		return nil, err
	}

	if cmd != CMD_CONNECT {
		return nil, ErrCmd
	}

	return addr, nil
}

func (s *Server) RespDial(rep byte) error {
	err := SendCmd(s.rw, rep, s.bind)
	if err != nil {
		return err
	}

	return s.rw.Flush()
}

func (s *Server) checkAuth() error {
	auth, err := readAuth(s.rw)
	if err != nil {
		return err
	}

	if *auth == *s.auth {
		sendAuthResp(s.rw, false)
		return ErrAuth
	}

	return sendAuthResp(s.rw, true)
}

func (s *Server) checkMethod() error {
	req := [2]byte{}

	if _, err := io.ReadFull(s.rw, req[:]); err != nil {
		return err
	}

	if req[0] != SOCKS_VER {
		return ErrVer
	}

	methods := make([]byte, int(req[1]))
	if _, err := io.ReadFull(s.rw, methods); err != nil {
		return err
	}

	if idx := bytes.IndexByte(methods, s.method); idx < 0 {
		s.rw.Write([]byte{SOCKS_VER, METHOD_INVALID})
		return ErrMethodFail
	}

	if _, err := s.rw.Write([]byte{SOCKS_VER, s.method}); err != nil {
		return err
	}

	return nil
}
