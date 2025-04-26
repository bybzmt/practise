package client

import (
	"net"
	"ss/utils"
	"time"
)

type socksClient struct {
	baseClient
	auth *utils.SimpleAuth
}

func (s *socksClient) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)

	from.SetDeadline(time.Now().Add(s.timeout))

	ss := utils.NewSocks(from, s.auth)

	addr, err := ss.HandShake()
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
		return
	}

	server := s.match(addr)
	if server == nil {
		s.watcher.NoServer(addr)
		ss.RespDial(utils.SOCKS_ERR_FORBIDDEN)
		return
	}

	pw := s.watcher.Create(server.Name(), from.RemoteAddr(), addr)

	to, err := server.Shadow(addr)
	if err != nil {
		ss.RespDial(utils.SOCKS_ERR_NET)
		pw.ShadowInvalid(err)
		return
	}
	defer to.Close()

	if err := ss.RespDial(utils.SOCKS_SUCCESS); err != nil {
		pw.ShadowInvalid(err)
		return
	}

	pw.Proxy()
	err = utils.Relay(s.connTick(from), s.connTick(ss))
	pw.Relay(err)
}
