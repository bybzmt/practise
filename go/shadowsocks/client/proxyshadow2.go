package client

import (
	"context"
	"fmt"
	"math/rand"
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type proxyShadow2 struct {
	proxyBase
	addr   string
	shadow utils.Creater
}

func (s *proxyShadow2) init() {
	s.name = fmt.Sprintf("ShadowProxy(%s)", s.addr)

	s.dnsDialer = func(network, addr string) (net.Conn, error) {
		raw, err := socks.ParseRawAddr(addr)
		if err != nil {
			return nil, err
		}

		return s.dialShadow(raw)
	}
}

func (s *proxyShadow2) Shadow(addr socks.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(context.Background(), addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		host := socks.IP2RawAddr(ipaddr[n].IP, addr.Port())
		return s.dialShadow(host)
	}

	return s.dialShadow(addr)
}

func (s *proxyShadow2) dialShadow(addr socks.RawAddr) (_ net.Conn, err error) {
	to, err := net.DialTimeout("tcp", s.addr, s.timeout)
	if err != nil {
		return nil, err
	}

	defer func() {
		if err != nil {
			to.Close()
		}
	}()

	to.SetWriteDeadline(time.Now().Add(s.timeout))

	t2 := s.shadow(to)

	if err = socks.SendCmd(t2, socks.CMD_CONNECT, addr); err != nil {
		return nil, err
	}

	cmd, addr, err := socks.ReadCmd(t2)
	if err != nil {
		return nil, err
	}

	if cmd != socks.SUCCESS {
		return nil, err
	}

	return t2, nil
}
