package serve

import (
	"context"
	"errors"
	"fmt"
	"math/rand"
	"net"
	"ss/shadow"
	"ss/socks"
	"ss/utils"
	"time"
)

type proxyShadow struct {
	proxyBase
	addr   string
	shadow utils.Creater
}

func newProxyShadow(f *ServeConfig, v *ProxyConfig) (*proxyShadow, error) {
	if v.Auth == nil {
		return nil, errors.New("shadowsocks Auth nil")
	}

	var key []byte
	t, err := shadow.PickCipher(v.Auth.Username, key, v.Auth.Password)
	if err != nil {
		return nil, err
	}

	s2 := &proxyShadow{
		proxyBase: newProxyBase(f.Timeout, v),
		addr:      v.Addr,
		shadow:    t.StreamConn,
	}
	s2.init()
	return s2, nil
}

func (s *proxyShadow) init() {
	s.name = fmt.Sprintf("ShadowProxy(%s)", s.addr)

	s.dnsDialer = func(network, addr string) (net.Conn, error) {
		raw, err := socks.ParseRawAddr(addr)
		if err != nil {
			return nil, err
		}

		return s.dialShadow(raw)
	}
}

func (s *proxyShadow) Dial(a net.Addr) (net.Conn, error) {
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

		host := socks.IP2RawAddr(ipaddr[n].IP, addr.Port())
		return s.dialShadow(host)
	}

	return s.dialShadow(addr)
}

func (s *proxyShadow) dialShadow(addr socks.RawAddr) (net.Conn, error) {
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

	c2 := &delayConn{
		Conn: s.shadow(to),
		req:  addr,
	}

	return c2, nil
}
