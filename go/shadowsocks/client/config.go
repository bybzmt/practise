package client

import (
	"errors"
	shadow "ss/shadow"
	"ss/utils"
	"strings"
	"time"
)

const (
	NOPROXY     = "NOPROXY"
	SOCKS       = "SOCKS"
	SHADOWSOCKS = "SHADOWSOCKS"
	RELAY       = "RELAY"
)

type Config struct {
	Setuid string
	Setgid string
	Client []ClientConfig
	Server []ServerConfig
}

type ClientConfig struct {
	Type        string
	Addr        string
	RelayTo     string
	Auth        *utils.SocksAuth
	Timeout     int
	IdleTimeout int
}

type ServerConfig struct {
	Type     string
	Addr     string
	Cipher   string
	Password string
	Disable  bool
	Proxy    bool
	Rules    []string
	DNS      []string
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
	if f.Timeout == 0 {
		f.Timeout = 10
	}
	if f.IdleTimeout == 0 {
		f.IdleTimeout = 120
	}

	var ss []Server
	for _, v := range servers {
		if v.Disable {
			continue
		}

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

	switch strings.ToUpper(f.Type) {
	case "":
		fallthrough
	case SOCKS:
		c := &socksClient{}
		c.addr = f.Addr
		c.timeout = timeout
		c.idleTimeout = idleTimeout
		c.servers = ss
		c.auth = f.Auth
		c.watcher = utils.DefaultWatcher
		return c, nil

	case RELAY:
		c := &relayClient{}
		c.addr = f.Addr
		c.timeout = timeout
		c.idleTimeout = idleTimeout
		c.servers = ss
		c.watcher = utils.DefaultWatcher

		addr, err := utils.Parse2RawAddr(f.RelayTo)
		if err != nil {
			return nil, err
		}
		c.RelayTo = addr
		return c, nil

	case SHADOWSOCKS:
		c := &shadowClient{}
		c.addr = f.Addr
		c.timeout = timeout
		c.idleTimeout = idleTimeout
		c.servers = ss
		c.watcher = utils.DefaultWatcher

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

func newServer(timeout int, v *ServerConfig) (Server, error) {
	var s Server
	timeout2 := time.Duration(timeout) * time.Second

	switch strings.ToUpper(v.Type) {
	case NOPROXY:
		s2 := &serverNoProxy{
			baseProxy: baseProxy{
				proxy:   v.Proxy,
				timeout: timeout2,
			},
		}
		s2.init()
		s = s2

	case SOCKS:
		s2 := &socksProxy{
			baseProxy: baseProxy{
				proxy:   v.Proxy,
				timeout: timeout2,
			},
			socks: utils.SocksServer{
				Addr:    v.Addr,
				Timeout: timeout2,
			},
		}

		if v.Cipher != "" {
			s2.socks.Auth = &utils.SocksAuth{
				Username: v.Cipher,
				Password: v.Password,
			}
		}
		s2.init()
		s = s2

	case "":
		fallthrough
	case SHADOWSOCKS:
		var key []byte
		t, err := shadow.PickCipher(v.Cipher, key, v.Password)
		if err != nil {
			return nil, err
		}

		s2 := &shadowProxy{
			addr: v.Addr,
			baseProxy: baseProxy{
				proxy:   v.Proxy,
				timeout: timeout2,
			},
			shadow: t.StreamConn,
		}
		s2.init()
		s = s2

	default:
		return nil, errors.New("Server Type Error")
	}

	s.SetRules(v.Rules)

	s.SetDNS(v.DNS)

	return s, nil
}
