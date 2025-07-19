package serve

import (
	"context"
	"fmt"
	"math/rand"
	"net"
	"ss/socks"
	"strings"
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

func newProxySSH(f *ServeConfig, v *ProxyConfig) (*proxySSH, error) {
	var c *ssh.ClientConfig

	if strings.Contains(v.Auth.Password, "KEY-----") {
		signer, err := ssh.ParsePrivateKey([]byte(v.Auth.Password))
		if err != nil {
			return nil, err
		}

		c = &ssh.ClientConfig{
			User: v.Auth.Username,
			Auth: []ssh.AuthMethod{
				ssh.PublicKeys(signer),
			},
			HostKeyCallback: ssh.InsecureIgnoreHostKey(),
		}
	} else {
		c = &ssh.ClientConfig{
			User: v.Auth.Username,
			Auth: []ssh.AuthMethod{
				ssh.Password(v.Auth.Password),
			},
			HostKeyCallback: ssh.InsecureIgnoreHostKey(),
		}
	}

	s2 := &proxySSH{
		proxyBase: newProxyBase(f.Timeout, v),
		addr:      v.Addr,
		sshConfig: c,
	}
	s2.init()
	return s2, nil
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

func (s *proxySSH) Dial(a net.Addr) (net.Conn, error) {
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
