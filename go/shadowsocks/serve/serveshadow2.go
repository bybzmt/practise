package serve

import (
	"bytes"
	"net"
	"ss/shadow"
	"ss/socks"
	"ss/utils"
	"time"
)

type serveShadow2 struct {
	serveBase
	shadow utils.Creater
}

func newServeShadow2(f *ServeConfig, proxy []Proxy) (*serveShadow2, error) {
	c := &serveShadow2{
		serveBase: newServeBase(f, proxy),
	}

	if f.RuleAddr != "" {
		addr, err := socks.ParseRawAddr(f.RuleAddr + ":0")
		if err != nil {
			return nil, err
		}
		c.ruleAddr = addr
	}

	var key []byte
	t, err := shadow.PickCipher(f.Auth.Username, key, f.Auth.Password)
	if err != nil {
		return nil, err
	}
	c.shadow = t.StreamConn
	return c, nil
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
	server := s.match(addr)
	if server == nil {
		s.watcher.NoServer(addr)
		socks.SendCmd(conn, socks.ERR_FORBIDDEN, nil)
		return
	}

	pw := s.watcher.Create(server.Name(), conn.RemoteAddr(), addr)

	to, err := server.Dial(addr)
	if err != nil {
		if err == errForbidden {
			socks.SendCmd(conn, socks.ERR_FORBIDDEN, nil)
		} else {
			socks.SendCmd(conn, socks.FAIL, nil)
		}

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
