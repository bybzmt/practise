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
	Forbid      []string
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
}

func (c *Config) FillDefault() {
	defaultServer := false

	if c.StateTick == nil {
		a := 10
		c.StateTick = &a
	}

	for _, cf := range c.Serve {
		cf.fillDefault()
	}

	var serve []*ServeConfig
	var proxy []*ProxyConfig

	for _, cf := range c.Serve {
		if cf.Disable {
			continue
		}
		serve = append(serve, cf)
	}

	for _, cs := range c.Proxy {
		if cs.Disable {
			continue
		}

		if !cs.Disable && cs.DefaultRule {
			defaultServer = true
		}

		if cs.Type == "" {
			cs.Type = SHADOWSOCKS2
		}

		proxy = append(proxy, cs)
	}

	if len(serve) == 0 {
		serve = append(serve, &ServeConfig{
			Addr: "127.0.0.1:1080",
		})
		serve[0].fillDefault()
	}

	if !defaultServer {
		if c.DefaultServer == nil || *c.DefaultServer == true {
			proxy = append(proxy, &ProxyConfig{
				Type:        NATIVE,
				DefaultRule: true,
			})
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

	if c.Forbid == nil {
		c.Forbid = []string{"127.0.0.1/8", "10.0.0.0/8", "172.16.0.0/12", "192.168.0.0/16"}
	}

	if c.Type == "" {
		if c.RelayTo != "" {
			c.Type = RELAY
		} else {
			c.Type = SOCKS
		}
	}
}
