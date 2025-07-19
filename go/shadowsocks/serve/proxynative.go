package serve

import (
	"context"
	"math/rand"
	"net"
	"ss/socks"
)

type proxyNative struct {
	proxyBase
}

func newProxyNative(f *ServeConfig, v *ProxyConfig) (*proxyNative, error) {
	s2 := &proxyNative{
		proxyBase: newProxyBase(f.Timeout, v),
	}
	s2.init()
	return s2, nil
}

func (s *proxyNative) init() {
	s.name = "NoProxy"
}

func (s *proxyNative) Dial(a net.Addr) (net.Conn, error) {
	addr, err := socks.Addr2RawAddr(a)
	if err != nil {
		return nil, err
	}

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
