package client

import (
	"net"
	"ss/utils"
	"time"
)

type relayClient struct {
	baseClient
	RelayTo utils.RawAddr
}

func (s *relayClient) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	server := s.match(s.RelayTo)
	if server == nil {
		s.watcher.NoServer(s.RelayTo)
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
