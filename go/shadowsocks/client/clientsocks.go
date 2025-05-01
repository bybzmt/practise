package client

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type clientSocks struct {
	baseClient
	auth *socks.SimpleAuth
}

func (s *clientSocks) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)

	from.SetDeadline(time.Now().Add(s.timeout))

	ss := socks.NewServer(from, s.auth)

	addr, err := ss.HandShake()
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
		return
	}

	if s.forbid.MatchRawAddr(addr) {
		utils.Debug.Println("forbidden", addr.String())
		ss.RespDial(socks.ERR_FORBIDDEN)
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
		ss.RespDial(socks.FAIL)
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
