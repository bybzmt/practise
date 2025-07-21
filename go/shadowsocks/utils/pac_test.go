package utils

import (
	"net"
	"testing"
)

func TestPacIP(ft *testing.T) {
	forbid := []string{"127.0.0.1/8", "10.0.0.1/8", "172.16.0.1/12", "192.168.0.1/16"}
	yes := []string{"127.0.0.2", "10.1.0.1", "172.16.0.100", "192.168.2.3"}
	no := []string{"8.8.8.8", "192.169.2.3"}

	ips := NewIPNets()

	for _, t := range forbid {
		_, ipnet, err := net.ParseCIDR(t)
		if err != nil {
			ft.Errorf("TestPacIP ParseCIDR %s", err)
		}

		ips.Add(*ipnet)
	}

	for _, t := range yes {
		ip := net.ParseIP(t)
		if ip == nil {
			ft.Errorf("TestPacIP ParseIP %s error", t)
		}

		ok := ips.Match(ip)

		if !ok {
			ft.Errorf("Match IP %s false", ip)
		}
	}

	for _, t := range no {
		ip := net.ParseIP(t)
		if ip == nil {
			ft.Errorf("TestPacIP ParseIP %s error", t)
		}

		ok := ips.Match(ip)
		if ok {
			ft.Errorf("Match IP %s false", ip)
		}
	}
}
