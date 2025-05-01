package client

import (
	"ss/utils"
	"ss/socks"
	"time"
)

type baseProxy struct {
	name      string
	dns       utils.DNS
	dnsDialer utils.DNSDialer
	rule      utils.Rules
	proxy     bool
	timeout   time.Duration
}

func (s *baseProxy) Name() string {
	return s.name
}

func (s *baseProxy) Match(addr socks.RawAddr) bool {
	if s.rule.MatchRawAddr(addr) {
		return !s.proxy
	}
	return s.proxy
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
