package client

import (
	"context"
	"math/rand"
	"net"
	"ss/utils"
)

type serverNoProxy struct {
	baseProxy
}

func (s *serverNoProxy) init() {
	s.name = "NoProxy"
}

func (s *serverNoProxy) Shadow(addr utils.RawAddr) (net.Conn, error) {
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
