package utils

import (
	"net"
	"time"
)

type TickConn struct {
	net.Conn
	ReadTimeout time.Duration
	WriteTimeout time.Duration
}

func (c *TickConn) Read(b []byte) (n int, err error) {
	c.Conn.SetReadDeadline(time.Now().Add(c.ReadTimeout))
	return c.Conn.Read(b)
}

func (c *TickConn) Write(b []byte) (n int, err error) {
	c.Conn.SetWriteDeadline(time.Now().Add(c.WriteTimeout))
	return c.Conn.Write(b)
}
