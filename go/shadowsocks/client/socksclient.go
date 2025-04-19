package client

import (
	"net"
	"ss/utils"
	"time"
)

type socksClient struct {
	baseClient
	auth *utils.SocksAuth
}

func (s *socksClient) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)

	from.SetDeadline(time.Now().Add(s.timeout))

	addr, err := utils.HandShake(from)
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
		return
	}

	if s.watcher.Hijacker(addr, from) {
		return
	}

	server := s.match(addr)
	if server == nil {
		s.watcher.NoServer(addr)
		return
	}

	pw := s.watcher.Create(server.Name(), from.RemoteAddr(), addr)

	to, err := server.Shadow(addr)
	if err != nil {
		pw.ShadowInvalid(err)
		return
	}
	defer to.Close()

	pw.Proxy()
	err = utils.Relay(s.connTick(from), s.connTick(to))
	pw.Relay(err)
}
