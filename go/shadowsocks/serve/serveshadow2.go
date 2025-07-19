package serve

import (
	"bytes"
	"errors"
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type serveShadow2 struct {
	serveBase
	shadow utils.Creater
}

func (s *serveShadow2) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	from = s.shadow(from)

	cmd, addr, err := socks.ReadCmd(from)
	if err != nil {
		s.watcher.HandShake(from.RemoteAddr(), err)
		return
	}

	if cmd == socks.CMD_CONNECT {
		s.serveTCP(addr, from)
		return
	}

	socks.SendCmd(from, socks.FAIL, nil)
	return
}

func (s *serveShadow2) serveTCP(addr socks.RawAddr, conn net.Conn) {
	if s.forbid.MatchRawAddr(addr) {
		s.watcher.HandShake(conn.RemoteAddr(), errors.New("forbidden "+addr.String()))
		socks.SendCmd(conn, socks.ERR_FORBIDDEN, nil)
		return
	}

	maddr := s.ruleAddr
	if maddr == nil {
		maddr = addr
	}

	server := s.match(maddr)
	if server == nil {
		s.watcher.NoServer(maddr)
		socks.SendCmd(conn, socks.ERR_FORBIDDEN, nil)
		return
	}

	pw := s.watcher.Create(server.Name(), conn.RemoteAddr(), addr)

	to, err := server.Shadow(addr)
	if err != nil {
		socks.SendCmd(conn, socks.FAIL, nil)
		pw.ShadowInvalid(err)
		return
	}
	defer to.Close()

	buf := new(bytes.Buffer)
	socks.SendCmd(buf, socks.SUCCESS, nil)

	c2 := &delayConn{
		Conn: conn,
		req:  buf.Bytes(),
	}

	pw.Proxy()
	err = utils.Relay(s.connTick(c2), s.connTick(to))
	pw.Relay(err)
}
