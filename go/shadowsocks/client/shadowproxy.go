package client

import (
	"context"
	"fmt"
	"math/rand"
	"net"
	"ss/utils"
	"time"
)

type shadowProxy struct {
	baseProxy
	addr   string
	shadow utils.Creater
}

func (s *shadowProxy) init() {
	s.name = fmt.Sprintf("ShadowProxy(%s)", s.addr)

	s.dnsDialer = func(network, addr string) (net.Conn, error) {
		raw, err := utils.Parse2RawAddr(addr)
		if err != nil {
			return nil, err
		}

		return s.dialShadow(raw)
	}
}

func (s *shadowProxy) Shadow(addr utils.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(context.Background(), addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		host := utils.IP2RawAddr(ipaddr[n].IP, addr.Port())
		return s.dialShadow(host)
	}

	return s.dialShadow(addr)
}

func (s *shadowProxy) dialShadow(addr utils.RawAddr) (net.Conn, error) {
	to, err := net.DialTimeout("tcp", s.addr, s.timeout)
	if err != nil {
		return nil, err
	}

	to.SetWriteDeadline(time.Now().Add(s.timeout))

	t2 := s.shadow(to)

	if _, err = t2.Write(addr); err != nil {
		return nil, err
	}

	return t2, nil
}
