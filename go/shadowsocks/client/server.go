package client

import (
	"math/rand"
	"net"
	"time"
	"ss/utils"
)

type Server interface {
	Shadow(addr utils.RawAddr) (net.Conn, error)
	Match(addr utils.RawAddr) bool
	SetRules(rules []string)
	SetDNS(ips []string)
	Close() error
}

type ServerNoProxy struct {
	dns     *utils.Dns
	rule    utils.Rules
	proxy   bool
	timeout time.Duration
}

func (s *ServerNoProxy) Match(addr utils.RawAddr) bool {
	if s.rule.MatchRawAddr(addr) {
		return s.proxy
	}
	return !s.proxy
}

func (s *ServerNoProxy) Shadow(addr utils.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		host := net.JoinHostPort(ipaddr[n].IP.String(), addr.PortString())

		return net.DialTimeout(addr.Network(), host, s.timeout)
	}

	return net.DialTimeout(addr.Network(), addr.String(), s.timeout)
}

func (s *ServerNoProxy) SetRules(rules []string) {
	for _, t := range rules {
		s.rule.Add(t)
	}
}

func (s *ServerNoProxy) SetDNS(ips []string) {
	s.dns = utils.NewDNS(ips)
}

func (s *ServerNoProxy) Close() error {
	if s.dns != nil {
		s.dns.Close()
	}
	return nil
}

type ShadowProxy struct {
	network string
	address string
	cipher  string

	shadow utils.Creater

	dns   *utils.Dns
	rule  utils.Rules
	proxy bool

	timeout time.Duration
}

func (s *ShadowProxy) Match(addr utils.RawAddr) bool {
	if s.rule.MatchRawAddr(addr) {
		return s.proxy
	}
	return !s.proxy
}

func (s *ShadowProxy) Shadow(addr utils.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		host := utils.IP2RawAddr(ipaddr[n].IP, addr.Port())
		return s.dialShadow(host)
	}

	return s.dialShadow(addr)
}

func (s *ShadowProxy) dialShadow(addr utils.RawAddr) (net.Conn, error) {
	to, err := net.DialTimeout(s.network, s.address, s.timeout)
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

func (s *ShadowProxy) SetRules(rules []string) {
	for _, t := range rules {
		s.rule.Add(t)
	}
}

func (s *ShadowProxy) SetDNS(ips []string) {
	s.dns = utils.NewDNS(ips)
	s.dns.Dial = func(network, addr string) (net.Conn, error) {
		raw, err := utils.Parse2RawAddr(addr)
		if err != nil {
			return nil, err
		}

		return s.dialShadow(raw)
	}
}

func (s *ShadowProxy) Close() error {
	if s.dns != nil {
		s.dns.Close()
	}
	return nil
}

type SocksProxy struct {
	socks utils.SocksServer
	dns   *utils.Dns
	rule  utils.Rules
	proxy bool
}

func (s *SocksProxy) Match(addr utils.RawAddr) bool {
	if s.rule.MatchRawAddr(addr) {
		return s.proxy
	}
	return !s.proxy
}

func (s *SocksProxy) Shadow(addr utils.RawAddr) (net.Conn, error) {
	if s.dns != nil && addr.ToIP() == nil {
		ipaddr, err := s.dns.LookupIPAddr(addr.Host())
		if err != nil {
			return nil, err
		}
		n := rand.Intn(len(ipaddr))

		host := net.JoinHostPort(ipaddr[n].IP.String(), addr.PortString())
		return s.socks.Dial(host)
	}

	return s.socks.Dial(addr.String())
}

func (s *SocksProxy) SetRules(rules []string) {
	for _, t := range rules {
		s.rule.Add(t)
	}
}

func (s *SocksProxy) SetDNS(ips []string) {
	s.dns = utils.NewDNS(ips)
	s.dns.Dial = func(network, addr string) (net.Conn, error) {
		return s.socks.Dial(addr)
	}
}

func (s *SocksProxy) Close() error {
	if s.dns != nil {
		s.dns.Close()
	}
	return nil
}
