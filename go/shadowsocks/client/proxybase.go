package client

import (
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

func (s *proxyBase) Name() string {
	return s.name
}

func (s *proxyBase) Match(addr socks.RawAddr) bool {
	if s.rule.MatchRawAddr(addr) {
		return !s.defaultRule
	}
	return s.defaultRule
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
