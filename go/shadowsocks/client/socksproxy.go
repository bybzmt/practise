package client

import (
	"context"
	"fmt"
	"math/rand"
	"net"
	"ss/utils"
)

type socksProxy struct {
	baseProxy
	socks utils.SocksServer
}

func (s *socksProxy) init() {
	s.name = fmt.Sprintf("SocksProxy(%s)", s.socks.Addr)
	s.dnsDialer = func(network, addr string) (net.Conn, error) {
		return s.socks.Dial(addr)
	}
}

func (s *socksProxy) Shadow(addr utils.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(context.Background(), addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		host := net.JoinHostPort(ipaddr[n].IP.String(), addr.PortString())
		return s.socks.Dial(host)
	}

	return s.socks.Dial(addr.String())
}
