package client

import (
	"errors"
	"ss/utils"
	shadow "ss/shadow"
	"time"
)

type Protocol int

const (
	NOPROXY     Protocol = 1
	SOCKS5      Protocol = 2
	SHADOWSOCKS Protocol = 3
)

type Config struct {
	Addr        string
	Server      []ServerConfig
	Timeout     int
	IdleTimeout int
}

type ServerConfig struct {
	Addr     string
	Cipher   string
	Password string
	Protocol Protocol
	Disable  bool
	Proxy    bool
	Rules    []string
	DNS      []string
}

func NewClient(f *Config) (*Client, error) {
	if f.Timeout == 0 {
		f.Timeout = 10
	}
	if f.IdleTimeout == 0 {
		f.IdleTimeout = 120
	}

	c := &Client{
		addr:        f.Addr,
		timeout:     time.Duration(f.Timeout) * time.Second,
		idleTimeout: time.Duration(f.IdleTimeout) * time.Second,
		Watcher:     utils.DefaultWatcher,
	}

	for _, v := range f.Server {
		if v.Disable {
			continue
		}

		var s Server

		switch v.Protocol {
		case NOPROXY:
			s = &ServerNoProxy{
				proxy:   v.Proxy,
				timeout: c.timeout,
			}

		case SOCKS5:
			s2 := &SocksProxy{
				proxy: v.Proxy,
				socks: utils.SocksServer{
					Addr:    v.Addr,
					Timeout: c.timeout,
				},
			}

			if v.Cipher != "" {
				s2.socks.Auth = &utils.SocksAuth{
					Username: v.Cipher,
					Password: v.Password,
				}
			}

			s = s2

		case SHADOWSOCKS:
			var key []byte
			t, err := shadow.PickCipher(v.Cipher, key, v.Password)
			if err != nil {
				return nil, err
			}

			s = &ShadowProxy{
				network: "tcp",
				address: v.Addr,
				proxy:   v.Proxy,
				timeout: c.timeout,
				shadow:  t.StreamConn,
			}

		default:
			return nil, errors.New("Server Protocol Error")
		}

		s.SetRules(v.Rules)

		s.SetDNS(v.DNS)

		c.servers = append(c.servers, s)
	}

	return c, nil
}
