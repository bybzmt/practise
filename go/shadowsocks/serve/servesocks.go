package serve

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type serveSocks struct {
	serveBase
	auth *socks.SimpleAuth
}

func (s *serveSocks) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)

	from.SetDeadline(time.Now().Add(s.timeout))

	ss := socks.NewServer(from, s.auth)

	err := ss.HandShake(true)
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
		return
	}

	s.serveSocksCmd(ss)
}

func (s *serveSocks) serveSocksCmd(ss *socks.Server) {
	cmd, addr, err := ss.ReadCmd()
	if err != nil {
		s.watcher.HandShake(ss.RemoteAddr(), err)
		ss.RespDial(socks.FAIL)
		return
	}

	if cmd != socks.CMD_CONNECT {
		ss.RespDial(socks.FAIL)
		return
	}

	if s.forbid.MatchRawAddr(addr) {
		utils.Debug.Println("forbidden", addr.String())
		ss.RespDial(socks.ERR_FORBIDDEN)
		return
	}

	maddr := s.ruleAddr
	if maddr == nil {
		maddr = addr
	}

	server := s.match(maddr)
	if server == nil {
		s.watcher.NoServer(maddr)
		ss.RespDial(socks.ERR_FORBIDDEN)
		return
	}

	pw := s.watcher.Create(server.Name(), ss.RemoteAddr(), addr)

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
