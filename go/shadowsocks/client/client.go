package client

import (
	"errors"
	"net"
	"ss/shadow"
	"ss/socks"
	"ss/utils"
	"strings"
	"time"
)

type Proxy interface {
	Name() string
	Shadow(addr socks.RawAddr) (net.Conn, error)
	Match(addr socks.RawAddr) bool
}

type Client interface {
	Listen() error
	Accept() (net.Conn, error)
	Close() error
	Addr() net.Addr
	Serve(from net.Conn)
	Traffic() *utils.Traffic
	SetWatcher(w utils.Watcher)
}

func NewClient(f *Config) ([]Client, error) {
	var cs []Client

	for _, cfg := range f.Client {
		c, err := newClient(&cfg, f.Server)
		if err != nil {
			return nil, err
		}

		cs = append(cs, c)
	}

	if len(cs) == 0 {
		return nil, errors.New("Client empty")
	}

	return cs, nil
}

func newClient(f *ClientConfig, servers []ServerConfig) (Client, error) {
	var ss []Proxy
	for _, v := range servers {
		s, err := newServer(f.Timeout, &v)
		if err != nil {
			return nil, err
		}

		ss = append(ss, s)
	}

	if len(ss) == 0 {
		return nil, errors.New("servers empty")
	}

	timeout := time.Duration(f.Timeout) * time.Second
	idleTimeout := time.Duration(f.IdleTimeout) * time.Second

	base := clientBase{}
	base.addr = f.Addr
	base.timeout = timeout
	base.idleTimeout = idleTimeout
	base.servers = ss
	base.watcher = utils.DefaultWatcher
	base.forbid.Init()
	for _, t := range f.Forbid {
		base.forbid.Add(t)
	}

	switch strings.ToUpper(f.Type) {
	case "":
		fallthrough
	case SOCKS:
		c := &clientSocks{
			clientBase: base,
		}
		c.auth = f.Auth
		return c, nil

	case RELAY:
		c := &clientRelay{
			clientBase: base,
		}
		addr, err := socks.ParseRawAddr(f.RelayTo)
		if err != nil {
			return nil, err
		}
		c.RelayTo = addr
		return c, nil

	case SHADOWSOCKS:
		c := &clientShadow{
			clientBase: base,
		}
		var key []byte
		t, err := shadow.PickCipher(f.Auth.Username, key, f.Auth.Password)
		if err != nil {
			return nil, err
		}
		c.shadow = t.StreamConn
		return c, nil

	default:
		return nil, errors.New("Client Type Error")
	}
}

func newServer(timeout int, v *ServerConfig) (Proxy, error) {
	timeout2 := time.Duration(timeout) * time.Second

	base := proxyBase{
		defaultRule: v.DefaultRule,
		timeout:     timeout2,
	}
	base.SetRules(v.Rules)
	base.SetDNS(v.DNS)

	switch strings.ToUpper(v.Type) {
	case NATIVE:
		s2 := &proxyNative{
			proxyBase: base,
		}
		s2.init()
		return s2, nil

	case SOCKS:
		s2 := &proxySocks{
			proxyBase: base,
			addr:      v.Addr,
			auth:      v.Auth,
		}
		s2.init()
		return s2, nil

	case "":
		fallthrough
	case SHADOWSOCKS:
		if v.Auth == nil {
			return nil, errors.New("shadowsocks Auth nil")
		}

		var key []byte
		t, err := shadow.PickCipher(v.Auth.Username, key, v.Auth.Password)
		if err != nil {
			return nil, err
		}

		s2 := &proxyShadow{
			proxyBase: base,
			addr:      v.Addr,
			shadow:    t.StreamConn,
		}
		s2.init()
		return s2, nil

	default:
		return nil, errors.New("Server Type Error")
	}
}
