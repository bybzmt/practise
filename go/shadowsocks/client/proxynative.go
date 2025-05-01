package client

import (
	"context"
	"math/rand"
	"net"
	"ss/socks"
)

type proxyNative struct {
	proxyBase
}

func (s *proxyNative) init() {
	s.name = "NoProxy"
}

func (s *proxyNative) Shadow(addr socks.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(context.Background(), addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		host := net.JoinHostPort(ipaddr[n].IP.String(), addr.PortString())

		return net.DialTimeout(addr.Network(), host, s.timeout)
	}

	return net.DialTimeout(addr.Network(), addr.String(), s.timeout)
}
