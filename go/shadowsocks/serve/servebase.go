package serve

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type serveBase struct {
	net.Listener
	addr        string
	ruleAddr    socks.RawAddr
	servers     []Proxy
	idleTimeout time.Duration
	timeout     time.Duration
	watcher     utils.Watcher
	traffic     utils.Traffic
	forbid      utils.Rules
}

func (s *serveBase) Listen() (err error) {
	s.Listener, err = net.Listen("tcp", s.addr)
	return
}

func (s *serveBase) match(addr socks.RawAddr) Proxy {
	for _, t := range s.servers {
		if t.Match(addr) {
			return t
		}
	}

	return nil
}

func (s *serveBase) connTraffic(conn net.Conn) net.Conn {
	return &utils.TrafficConn{
		Conn: conn,
		Now:  &s.traffic,
		End:  nil,
	}
}

func (s *serveBase) connTick(conn net.Conn) net.Conn {
	return &utils.TickConn{
		Conn:         conn,
		ReadTimeout:  s.idleTimeout,
		WriteTimeout: s.timeout,
	}
}

func (s *serveBase) Traffic() *utils.Traffic {
	return &s.traffic
}

func (s *serveBase) SetWatcher(w utils.Watcher) {
	s.watcher = w
}
