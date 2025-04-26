package client

import (
	"context"
	"fmt"
	"math/rand"
	"net"
	"ss/utils"
	"time"
)

type socksProxy struct {
	baseProxy
	addr string
	auth *utils.SimpleAuth
}

func (s *socksProxy) init() {
	s.name = fmt.Sprintf("SocksProxy(%s)", s.addr)
	s.dnsDialer = func(network, addr string) (net.Conn, error) {
		raw, err := utils.Parse2RawAddr(addr)
		if err != nil {
			return nil, err
		}
		return s.dial(raw)
	}
}

func (s *socksProxy) Shadow(addr utils.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(context.Background(), addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		raw := utils.IP2RawAddr(ipaddr[n].IP, addr.Port())
		return s.dial(raw)
	}

	return s.dial(addr)
}

func (s *socksProxy) dial(addr utils.RawAddr) (net.Conn, error) {
	to, err := net.DialTimeout("tcp", s.addr, s.timeout)
	if err != nil {
		return nil, err
	}

	to.SetDeadline(time.Now().Add(s.timeout))

	ss := utils.NewSocks(to, s.auth)

	if err = ss.Dial(addr); err != nil {
		to.Close()
		return nil, err
	}

	return ss, nil
}
