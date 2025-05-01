package client

import (
	"ss/socks"
	"ss/utils"
	"time"
)

type baseProxy struct {
	name        string
	dns         utils.DNS
	dnsDialer   utils.DNSDialer
	rule        utils.Rules
	defaultRule bool
	timeout     time.Duration
}

func (s *baseProxy) Name() string {
	return s.name
}

func (s *baseProxy) Match(addr socks.RawAddr) bool {
	if s.rule.MatchRawAddr(addr) {
		return !s.defaultRule
	}
	return s.defaultRule
}

func (s *baseProxy) SetRules(rules []string) {
	s.rule.Init()

	for _, t := range rules {
		s.rule.Add(t)
	}
}

func (s *baseProxy) SetDNS(ips []string) {
	if len(ips) == 0 {
		s.dns = nil
		return
	}

	s.dns = utils.NewDNS(ips, s.dnsDialer)
}
