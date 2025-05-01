package utils

import (
	"log"
	"net"
	"regexp"
	"regexp/syntax"
	"ss/socks"
)

type Rules struct {
	Host Domain
	IP   IPNets
	Regs []regexp.Regexp
}

func (r *Rules) Init() {
	r.Host = make(Domain)
	r.IP = NewIPNets()
}

// 192.168.0.0/16
// .github.com
// www.github.com
func (r *Rules) Add(rule string) {
	_, ipnet, err := net.ParseCIDR(rule)
	if err == nil {
		r.IP.Add(*ipnet)
	} else if isValidDomain(rule) {
		r.Host.Add(rule)
	} else if isSuffixDomain(rule) {
		suffix := regexp.QuoteMeta(rule[1:])

		r.Regs = append(r.Regs, *regexp.MustCompile("^.+" + suffix + "$"))
	} else {
		reg, err := syntax.Parse(rule, syntax.Perl)
		if err != nil {
			Debug.Println("Rule Regexp", err)
		} else {
			r.Regs = append(r.Regs, *regexp.MustCompile(reg.String()))
		}
	}
}

func (r *Rules) Match(host string) bool {
	ip := net.ParseIP(host)

	if ip != nil {
		return r.IP.Match(ip)
	}
	if r.Host.Match(host) {
		return true
	}
	for _, reg := range r.Regs {
		if reg.MatchString(host) {
			return true
		}
	}

	return false
}

func (r *Rules) MatchRawAddr(addr socks.RawAddr) bool {
	switch addr.Type() {
	case socks.ATYP_IPV4, socks.ATYP_IPV6:
		return r.IP.Match(addr.ToIP())

	case socks.ATYP_DOMAINNAME:
		host := addr.Host()
		if r.Host.Match(host) {
			return true
		}
		for _, reg := range r.Regs {
			if reg.MatchString(host) {
				return true
			}
		}
	}
	return false
}

func isSuffixDomain(domain string) bool {
	log.Println(domain)
	pattern := `^\*\.([a-zA-Z0-9-]+\.)+[a-zA-Z]{2,}$`
	matched, err := regexp.MatchString(pattern, domain)
	if err != nil {
		log.Println(err)
		return false
	}
	log.Println(matched)
	return matched
}

func isValidDomain(domain string) bool {
	log.Println(domain)
	pattern := `^([a-zA-Z0-9-]+\.)+[a-zA-Z]{2,}$`
	matched, err := regexp.MatchString(pattern, domain)
	if err != nil {
		log.Println(err)
		return false
	}
	log.Println(matched)
	return matched
}
