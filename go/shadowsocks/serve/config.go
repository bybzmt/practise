package serve

import (
	"ss/socks"
)

const (
	NATIVE       = "NATIVE"
	SOCKS        = "SOCKS"
	SHADOWSOCKS  = "SHADOWSOCKS"
	SHADOWSOCKS2 = "SHADOWSOCKS2"
	RELAY        = "RELAY"
	SSH          = "SSH"
)

type Config struct {
	Setuid        string
	Setgid        string
	StateTick     *int
	Serve         []*ServeConfig
	Proxy         []*ProxyConfig
	DefaultServer *bool
}

type ServeConfig struct {
	Type        string
	Addr        string
	RelayTo     string
	RuleAddr    string
	Auth        *socks.SimpleAuth
	Timeout     int
	IdleTimeout int
	Disable     bool
}

type ProxyConfig struct {
	Type        string
	Addr        string
	Auth        *socks.SimpleAuth
	Disable     bool
	DefaultRule bool
	Rules       []string
	DNS         []string
	Forbid      []string
}

func (c *Config) FillDefault() {
	defaultServer := false

	if c.StateTick == nil {
		a := 10
		c.StateTick = &a
	}

	var serve []*ServeConfig
	var proxy []*ProxyConfig

	for _, cf := range c.Serve {
		if cf.Disable {
			continue
		}

		cf.fillDefault()
		serve = append(serve, cf)
	}

	if len(serve) == 0 {
		t := &ServeConfig{
			Addr: "127.0.0.1:1080",
		}
		t.fillDefault()
		serve = append(serve, t)
	}

	for _, cs := range c.Proxy {
		if cs.Disable {
			continue
		}

		cs.fillDefault()

		proxy = append(proxy, cs)

		if cs.DefaultRule {
			defaultServer = true
		}
	}

	if !defaultServer {
		if c.DefaultServer == nil || *c.DefaultServer == true {
			cs := &ProxyConfig{
				Type:        NATIVE,
				DefaultRule: true,
			}
			cs.fillDefault()
			proxy = append(proxy, cs)
		}
	}

	c.Serve = serve
	c.Proxy = proxy
}

func (c *ServeConfig) fillDefault() {
	if c.Timeout == 0 {
		c.Timeout = 10
	}

	if c.IdleTimeout == 0 {
		c.IdleTimeout = 120
	}

	if c.Type == "" {
		if c.RelayTo != "" {
			c.Type = RELAY
		} else {
			c.Type = SOCKS
		}
	}
}

func (c *ProxyConfig) fillDefault() {
	if c.Type == "" {
		c.Type = SHADOWSOCKS2
	}

	if c.Type == NATIVE {
		if c.Forbid == nil {
			c.Forbid = []string{"127.0.0.1/8", "10.0.0.1/8", "172.16.0.1/12", "192.168.0.1/16"}
		}
	}
}
