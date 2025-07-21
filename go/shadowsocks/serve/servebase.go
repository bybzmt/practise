package serve

import (
	"net"
	"ss/utils"
	"time"
)

type serveBase struct {
	net.Listener
	addr        string
	ruleAddr    net.Addr
	proxys      []Proxy
	idleTimeout time.Duration
	timeout     time.Duration
	watcher     utils.Watcher
	traffic     utils.Traffic
}

func newServeBase(f *ServeConfig, proxys []Proxy) serveBase {
	timeout := time.Duration(f.Timeout) * time.Second
	idleTimeout := time.Duration(f.IdleTimeout) * time.Second

	base := serveBase{}
	base.addr = f.Addr
	base.timeout = timeout
	base.idleTimeout = idleTimeout
	base.proxys = proxys
	base.watcher = utils.DefaultWatcher

	return base
}

func (s *serveBase) Listen() (err error) {
	s.Listener, err = net.Listen("tcp", s.addr)
	return
}

func (s *serveBase) match(addr net.Addr) Proxy {
	maddr := s.ruleAddr
	if maddr == nil {
		maddr = addr
	}

	for _, t := range s.proxys {
		if t.Match(maddr) {
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
