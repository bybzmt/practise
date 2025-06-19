package client

import (
	"context"
	"fmt"
	"math/rand"
	"net"
	"ss/socks"
	"sync"

	"golang.org/x/crypto/ssh"
)

type proxySSH struct {
	proxyBase
	addr      string
	sshConfig *ssh.ClientConfig
	ssh       *ssh.Client
	l         sync.Mutex
}

func (s *proxySSH) init() {
	s.name = fmt.Sprintf("SocksProxy(%s)", s.addr)
	s.dnsDialer = func(network, addr string) (net.Conn, error) {
		raw, err := socks.ParseRawAddr(addr)
		if err != nil {
			return nil, err
		}
		return s.dial(raw)
	}
}

func (s *proxySSH) Shadow(addr socks.RawAddr) (net.Conn, error) {
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

func (s *proxySSH) dial(addr socks.RawAddr) (net.Conn, error) {
	t, err := s.dialSSH()
	if err != nil {
		return nil, err
	}

	return t.Dial("tcp", addr.String())
}

func (s *proxySSH) dialSSH() (*ssh.Client, error) {
	s.l.Lock()
	defer s.l.Unlock()

	if s.ssh == nil {
		t, err := ssh.Dial("tcp", s.addr, s.sshConfig)
		if err != nil {
			return nil, err
		}
		s.ssh = t

		go func() {
			t.Wait()

			s.l.Lock()
			defer s.l.Unlock()
			s.ssh = nil
		}()
	}

	return s.ssh, nil
}
