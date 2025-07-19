package serve

import (
	"errors"
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type serveShadow struct {
	serveBase
	shadow utils.Creater
}

func (s *serveShadow) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	from = s.shadow(from)

	faddr := from.RemoteAddr()

	taddr, err := socks.ReadRawAddr(from)
	if err != nil {
		s.watcher.HandShake(faddr, err)
		return
	}

	if s.forbid.MatchRawAddr(taddr) {
		s.watcher.HandShake(faddr, errors.New("forbidden "+taddr.String()))
		return
	}

	maddr := s.ruleAddr
	if maddr == nil {
		maddr = taddr
	}

	server := s.match(maddr)
	if server == nil {
		s.watcher.NoServer(maddr)
		return
	}

	pw := s.watcher.Create(server.Name(), faddr, taddr)

	to, err := server.Shadow(taddr)
	if err != nil {
		pw.ShadowInvalid(err)
		return
	}
	defer to.Close()

	pw.Proxy()
	err = utils.Relay(s.connTick(from), s.connTick(to))
	pw.Relay(err)
}
