package serve

import (
	"net"
	"ss/shadow"
	"ss/socks"
	"ss/utils"
	"time"
)

type serveShadow struct {
	serveBase
	shadow utils.Creater
}

func newServeShadow(f *ServeConfig, proxy []Proxy) (*serveShadow, error) {
	c := &serveShadow{
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

	server := s.match(taddr)
	if server == nil {
		s.watcher.NoServer(taddr)
		return
	}

	pw := s.watcher.Create(server.Name(), faddr, taddr)

	to, err := server.Dial(taddr)
	if err != nil {
		pw.ShadowInvalid(err)
		return
	}
	defer to.Close()

	pw.Proxy()
	err = utils.Relay(s.connTick(from), s.connTick(to))
	pw.Relay(err)
}
