package client

import (
	"net"
	"ss/utils"
	"ss/socks"
	"time"
)

type socksClient struct {
	baseClient
	auth *socks.SimpleAuth
}

func (s *socksClient) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)

	from.SetDeadline(time.Now().Add(s.timeout))

	ss := socks.NewServer(from, s.auth)

	addr, err := ss.HandShake()
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
		return
	}

	server := s.match(addr)
	if server == nil {
		s.watcher.NoServer(addr)
		ss.RespDial(socks.ERR_FORBIDDEN)
		return
	}

	pw := s.watcher.Create(server.Name(), from.RemoteAddr(), addr)

	to, err := server.Shadow(addr)
	if err != nil {
		ss.RespDial(socks.ERR_NET)
		pw.ShadowInvalid(err)
		return
	}
	defer to.Close()

	if err := ss.RespDial(socks.SUCCESS); err != nil {
		pw.ShadowInvalid(err)
		return
	}

	pw.Proxy()
	err = utils.Relay(s.connTick(ss), s.connTick(to))
	pw.Relay(err)
}
