package serve

import (
	"bytes"
	"io"
	"net"
)

type delayConn struct {
	net.Conn
	req  []byte
	resp func(net.Conn) error
}

func (s *delayConn) Write(buf []byte) (n int, err error) {
	if s.req != nil {
		r := bytes.NewReader(append(s.req, buf...))

		if _, err := io.Copy(s.Conn, r); err != nil {
			return 0, err
		}

		s.req = nil
		return len(buf), nil
	}

	return s.Conn.Write(buf)
}

func (s *delayConn) Read(buf []byte) (int, error) {
	if s.resp != nil {
		err := s.resp(s.Conn)
		if err != nil {
			return 0, err
		}

		s.resp = nil
	}

	return s.Conn.Read(buf)
}
