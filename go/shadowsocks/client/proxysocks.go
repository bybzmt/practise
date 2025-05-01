package client

import (
	"context"
	"fmt"
	"math/rand"
	"net"
	"ss/socks"
	"time"
)

type proxySocks struct {
	baseProxy
	addr string
	auth *socks.SimpleAuth
}

func (s *proxySocks) init() {
	s.name = fmt.Sprintf("SocksProxy(%s)", s.addr)
	s.dnsDialer = func(network, addr string) (net.Conn, error) {
		raw, err := socks.ParseRawAddr(addr)
		if err != nil {
			return nil, err
		}
		return s.dial(raw)
	}
}

func (s *proxySocks) Shadow(addr socks.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(context.Background(), addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		raw := socks.IP2RawAddr(ipaddr[n].IP, addr.Port())
		return s.dial(raw)
	}

	return s.dial(addr)
}

func (s *proxySocks) dial(addr socks.RawAddr) (net.Conn, error) {
	to, err := net.DialTimeout("tcp", s.addr, s.timeout)
	if err != nil {
		return nil, err
	}

	to.SetDeadline(time.Now().Add(s.timeout))

	ss := socks.NewClient(to, s.auth)

	if err = ss.Dial(addr); err != nil {
		to.Close()
		return nil, err
	}

	return ss, nil
}
