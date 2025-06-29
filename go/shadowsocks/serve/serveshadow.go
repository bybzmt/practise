package serve

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type serveShadow struct {
	serveSocks
	shadow utils.Creater
}

func (s *serveShadow) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	from = s.shadow(from)

	ss := socks.NewServer(from, s.auth)

	err := ss.HandShake(false)
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
		return
	}

	s.serveSocksCmd(ss)
}
