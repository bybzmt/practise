package serve

import (
	"errors"
	"net"
	"ss/utils"
	"strings"
)

type Proxy interface {
	Name() string
	Dial(net.Addr) (net.Conn, error)
	Match(net.Addr) bool
}

type Serve interface {
	Listen() error
	Accept() (net.Conn, error)
	Close() error
	Addr() net.Addr
	Serve(net.Conn)
	Traffic() *utils.Traffic
	SetWatcher(utils.Watcher)
}

func NewServe(f *Config) ([]Serve, error) {
	var cs []Serve

	for _, cfg := range f.Serve {
		c, err := newServer(cfg, f.Proxy)
		if err != nil {
			return nil, err
		}

		cs = append(cs, c)
	}

	if len(cs) == 0 {
		return nil, errors.New("server empty")
	}

	return cs, nil
}

func newServer(f *ServeConfig, proxy []*ProxyConfig) (Serve, error) {
	ss, err := newProxys(f, proxy)
	if err != nil {
		return nil, err
	}

	switch strings.ToUpper(f.Type) {
	case SOCKS:
		return newServeSocks(f, ss)
	case RELAY:
		return newServeRelay(f, ss)
	case SHADOWSOCKS:
		return newServeShadow(f, ss)
	case SHADOWSOCKS2:
		return newServeShadow2(f, ss)
	default:
		return nil, errors.New("Client Type Error")
	}
}

func newProxys(f *ServeConfig, proxy []*ProxyConfig) ([]Proxy, error) {
	var ss []Proxy
	for _, v := range proxy {
		s, err := newProxy(f, v)
		if err != nil {
			return nil, err
		}

		ss = append(ss, s)
	}

	if len(ss) == 0 {
		return nil, errors.New("proxy server empty")
	}

	return ss, nil
}

func newProxy(f *ServeConfig, v *ProxyConfig) (Proxy, error) {
	switch strings.ToUpper(v.Type) {
	case NATIVE:
		return newProxyNative(f, v)
	case SOCKS:
		return newProxySocks(f, v)
	case SSH:
		return newProxySSH(f, v)
	case SHADOWSOCKS:
		return newProxyShadow(f, v)
	case SHADOWSOCKS2:
		return newProxyShadow2(f, v)
	default:
		return nil, errors.New("Server Type Error")
	}
}
