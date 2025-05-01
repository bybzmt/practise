package client

import (
	"net"
	"ss/utils"
	"ss/socks"
	"time"
)

type baseClient struct {
	net.Listener
	addr        string
	servers     []Server
	idleTimeout time.Duration
	timeout     time.Duration
	watcher     utils.Watcher
	traffic     utils.Traffic
}

func (s *baseClient) Listen() (err error) {
	s.Listener, err = net.Listen("tcp", s.addr)
	return
}

func (s *baseClient) match(addr socks.RawAddr) Server {
	for _, t := range s.servers {
		if t.Match(addr) {
			return t
		}
	}

	return nil
}

func (s *baseClient) connTraffic(conn net.Conn) net.Conn {
	return &utils.TrafficConn{
		Conn: conn,
		Now:  &s.traffic,
		End:  nil,
	}
}

func (s *baseClient) connTick(conn net.Conn) net.Conn {
	return &utils.TickConn{
		Conn:         conn,
		ReadTimeout:  s.idleTimeout,
		WriteTimeout: s.timeout,
	}
}

func (s *baseClient) Traffic() *utils.Traffic {
	return &s.traffic
}

func (s *baseClient) SetWatcher(w utils.Watcher) {
	s.watcher = w
}
