package server

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type serverShadow2 struct {
	serverSocks
	shadow utils.Creater
}

func (s *serverShadow2) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	from = s.shadow(from)

	ss := socks.NewServer(from, nil)

	err := ss.HandShake(false)
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
		return
	}

	s.serveSocks(ss)
}
