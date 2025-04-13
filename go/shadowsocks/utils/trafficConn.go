package utils

import (
	"net"
)

type TrafficConn struct {
	net.Conn
	Traffic Traffic
	Now     *Traffic
	End     *Traffic
}

func (c *TrafficConn) Read(b []byte) (n int, err error) {
	defer func() {
		if n > 0 {
			c.Traffic.Incoming += int64(n)
			if c.Now != nil {
				c.Now.AddIncoming(int64(n))
			}
		}
	}()

	return c.Conn.Read(b)
}

func (c *TrafficConn) Write(b []byte) (n int, err error) {
	defer func() {
		if n > 0 {
			c.Traffic.Outgoing += int64(n)
			if c.Now != nil {
				c.Now.AddOutgoing(int64(n))
			}
		}
	}()

	return c.Conn.Write(b)
}

func (c *TrafficConn) Close() (err error) {
	if c.End != nil {
		c.End.Add(&c.Traffic)
	}
	return c.Conn.Close()
}
