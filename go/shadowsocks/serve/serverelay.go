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

func newServeRelay(f *ServeConfig, proxy []Proxy) (*serveRelay, error) {
	c := &serveRelay{
		serveBase: newServeBase(f, proxy),
	}

	addr, err := socks.ParseRawAddr(f.RelayTo)
	if err != nil {
		return nil, err
	}

	c.RelayTo = addr

	if f.RuleAddr != "" {
		addr, err := socks.ParseRawAddr(f.RuleAddr + ":0")
		if err != nil {
			return nil, err
		}
		c.ruleAddr = addr
	}
	return c, nil
}

func (s *serveRelay) Serve(from net.Conn) {
	defer from.Close()

	from = s.connTraffic(from)
	from.SetDeadline(time.Now().Add(s.timeout))

	server := s.match(s.RelayTo)
	if server == nil {
		s.watcher.NoServer(s.RelayTo)
		return
	}

	pw := s.watcher.Create(server.Name(), from.RemoteAddr(), s.RelayTo)

	to, err := server.Dial(s.RelayTo)
	if err != nil {
		pw.ShadowInvalid(err)
		return
	}
	defer to.Close()

	pw.Proxy()
	err = utils.Relay(s.connTick(from), s.connTick(to))
	pw.Relay(err)
}
