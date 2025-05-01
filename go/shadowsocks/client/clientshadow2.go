package client

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type clientShadow2 struct {
	clientBase
	shadow utils.Creater
}

func (s *clientShadow2) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	from = s.shadow(from)

	cmd, addr, err := socks.ReadCmd(from)
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
	}

	if cmd != socks.CMD_CONNECT {
		s.watcher.HandShake(from.RemoteAddr(), socks.ErrCmd)
		return
	}

	if s.forbid.MatchRawAddr(addr) {
		utils.Debug.Println("forbidden", addr.String())
		socks.SendCmd(from, socks.ERR_FORBIDDEN, nil)
		return
	}

	server := s.match(addr)
	if server == nil {
		s.watcher.NoServer(addr)
		socks.SendCmd(from, socks.ERR_FORBIDDEN, nil)
		return
	}

	pw := s.watcher.Create(server.Name(), from.RemoteAddr(), addr)

	to, err := server.Shadow(addr)
	if err != nil {
		pw.ShadowInvalid(err)
		socks.SendCmd(from, socks.FAIL, nil)
		return
	}
	defer to.Close()

	if err := socks.SendCmd(from, socks.SUCCESS, nil); err != nil {
		return
	}

	pw.Proxy()
	err = utils.Relay(s.connTick(from), s.connTick(to))
	pw.Relay(err)
}
