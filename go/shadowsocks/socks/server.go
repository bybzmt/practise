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
	return &Server{socks: newSocks(c, auth)}
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

	req := [3]byte{}

	if _, err := io.ReadFull(s, req[:]); err != nil {
		return nil, err
	}

	if req[0] != VER {
		return nil, errVer
	}

	if req[1] != CMD_CONNECT {
		return nil, errCmd
	}

	addr, err := ReadRawAddr(s)
	if err != nil {
		return nil, err
	}

	return addr, nil
}

func (s *Server) RespDial(rep byte) error {
	if _, err := s.rw.Write([]byte{VER, rep, RSV}); err != nil {
		return err
	}

	if s.bind == nil {
		s.bind = RawAddr([]byte{ATYP_IPV4, 0, 0, 0, 0, 0, 0})
	}

	if _, err := s.rw.Write(s.bind); err != nil {
		return err
	}

	if err := s.rw.Flush(); err != nil {
		return err
	}

	return nil
}

func (s *Server) checkAuth() error {
	if err := checkAuth(s.rw, s.auth); err != nil {
		return err
	}

	if _, err := s.rw.Write([]byte{AUTH_VER, SUCCESS}); err != nil {
		return err
	}

	return nil
}

func (s *Server) checkMethod() error {
	req := [3]byte{}

	if _, err := io.ReadFull(s.rw, req[:2]); err != nil {
		return err
	}

	if req[0] != VER {
		return errVer
	}

	methods := make([]byte, int(req[1]))
	if _, err := io.ReadFull(s.rw, methods); err != nil {
		return err
	}

	if idx := bytes.IndexByte(methods, s.method); idx < 0 {
		if _, err := s.rw.Write([]byte{VER, METHOD_INVALID}); err != nil {
			return err
		}
		return errMethodFail
	}

	if _, err := s.rw.Write([]byte{VER, s.method}); err != nil {
		return err
	}

	return nil
}
