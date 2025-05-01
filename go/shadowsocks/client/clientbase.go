package client

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type clientBase struct {
	net.Listener
	addr        string
	servers     []Proxy
	idleTimeout time.Duration
	timeout     time.Duration
	watcher     utils.Watcher
	traffic     utils.Traffic
	forbid      utils.Rules
}

func (s *clientBase) Listen() (err error) {
	s.Listener, err = net.Listen("tcp", s.addr)
	return
}

func (s *clientBase) match(addr socks.RawAddr) Proxy {
	for _, t := range s.servers {
		if t.Match(addr) {
			return t
		}
	}

	return nil
}

func (s *clientBase) connTraffic(conn net.Conn) net.Conn {
	return &utils.TrafficConn{
		Conn: conn,
		Now:  &s.traffic,
		End:  nil,
	}
}

func (s *clientBase) connTick(conn net.Conn) net.Conn {
	return &utils.TickConn{
		Conn:         conn,
		ReadTimeout:  s.idleTimeout,
		WriteTimeout: s.timeout,
	}
}

func (s *clientBase) Traffic() *utils.Traffic {
	return &s.traffic
}

func (s *clientBase) SetWatcher(w utils.Watcher) {
	s.watcher = w
}
