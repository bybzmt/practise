package server

import (
	"ss/socks"
)

const (
	NATIVE      = "NATIVE"
	SOCKS       = "SOCKS"
	SHADOWSOCKS = "SHADOWSOCKS"
	RELAY       = "RELAY"
	SSH         = "SSH"
)

type Config struct {
	Setuid        string
	Setgid        string
	Server        []*ServerConfig
	Proxy         []*ProxyConfig
	DefaultServer *bool
}

type ServerConfig struct {
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

	for _, cf := range c.Server {
		cf.fillDefault()
	}

	var server []*ServerConfig
	var proxy []*ProxyConfig

	for _, cf := range c.Server {
		if cf.Disable {
			continue
		}
		server = append(server, cf)
	}

	for _, cs := range c.Proxy {
		if cs.Disable || (!cs.DefaultRule && cs.Rules == nil) {
			continue
		}

		if !cs.Disable && cs.DefaultRule {
			defaultServer = true
		}

		proxy = append(proxy, cs)
	}

	if len(server) == 0 {
		server = append(server, &ServerConfig{
			Addr: "127.0.0.1:1080",
		})
		server[0].fillDefault()
	}

	if !defaultServer {
		if c.DefaultServer == nil || *c.DefaultServer == true {
			proxy = append(proxy, &ProxyConfig{
				Type:        NATIVE,
				DefaultRule: true,
			})
		}
	}

	c.Server = server
	c.Proxy = proxy
}

func (c *ServerConfig) fillDefault() {
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
