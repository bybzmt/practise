package server

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type clientShadow struct {
	clientBase
	shadow utils.Creater
}

func (s *clientShadow) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	from = s.shadow(from)

	addr, err := socks.ReadRawAddr(from)
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
	}

	if s.forbid.MatchRawAddr(addr) {
		utils.Debug.Println("forbidden", addr.String())
		return
	}

	maddr := s.ruleAddr
	if maddr == nil {
		maddr = addr
	}

	server := s.match(maddr)
	if server == nil {
		s.watcher.NoServer(maddr)
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
