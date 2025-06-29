package serve

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type serveRelay struct {
	serveBase
	RelayTo socks.RawAddr
}

func (s *serveRelay) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	maddr := s.ruleAddr
	if maddr == nil {
		maddr = s.RelayTo
	}

	server := s.match(maddr)
	if server == nil {
		s.watcher.NoServer(maddr)
		return
	}

	pw := s.watcher.Create(server.Name(), from.RemoteAddr(), s.RelayTo)

	to, err := server.Shadow(s.RelayTo)
	if err != nil {
		pw.ShadowInvalid(err)
		return
	}
	defer to.Close()

	pw.Proxy()
	err = utils.Relay(s.connTick(from), s.connTick(to))
	pw.Relay(err)
}
