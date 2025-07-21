package serve

import (
	"net"
	"ss/socks"
	"ss/utils"
	"time"
)

type proxyBase struct {
	name        string
	dns         utils.DNS
	dnsDialer   utils.DNSDialer
	rule        utils.Rules
	defaultRule bool
	timeout     time.Duration
}

func newProxyBase(timeout int, v *ProxyConfig) proxyBase {
	timeout2 := time.Duration(timeout) * time.Second

	base := proxyBase{
		defaultRule: v.DefaultRule,
		timeout:     timeout2,
	}
	base.SetRules(v.Rules)
	base.SetDNS(v.DNS)

	return base
}

func (s *proxyBase) Name() string {
	return s.name
}

func (s *proxyBase) Match(addr net.Addr) bool {
	if s.defaultRule {
		return true
	}

	raddr, err := socks.Addr2RawAddr(addr)
	if err != nil {
		return false
	}

	if s.rule.MatchRawAddr(raddr) {
		return true
	}

	return false
}

func (s *proxyBase) SetRules(rules []string) {
	s.rule.Init()

	for _, t := range rules {
		s.rule.Add(t)
	}
}

func (s *proxyBase) SetDNS(ips []string) {
	if len(ips) == 0 {
		s.dns = nil
		return
	}

	s.dns = utils.NewDNS(ips, s.dnsDialer)
}
