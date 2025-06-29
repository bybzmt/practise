package client

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
	Client        []*ClientConfig
	Server        []*ServerConfig
	DefaultServer *bool
}

type ClientConfig struct {
	Type        string
	Addr        string
	RelayTo     string
	Auth        *socks.SimpleAuth
	Timeout     int
	IdleTimeout int
	Forbid      []string
	Disable     bool
}

type ServerConfig struct {
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

	for _, cf := range c.Client {
		cf.fillDefault()
	}

	var clients []*ClientConfig
	var servers []*ServerConfig

	for _, cf := range c.Client {
		if cf.Disable {
			continue
		}
		clients = append(clients, cf)
	}

	for _, cs := range c.Server {
		if cs.Disable || (!cs.DefaultRule && cs.Rules == nil) {
			continue
		}

		if !cs.Disable && cs.DefaultRule {
			defaultServer = true
		}

		servers = append(servers, cs)
	}

	if len(clients) == 0 {
		clients = append(clients, &ClientConfig{
			Addr: "127.0.0.1:1080",
		})
		clients[0].fillDefault()
	}

	if !defaultServer {
		if c.DefaultServer == nil || *c.DefaultServer == true {
			servers = append(servers, &ServerConfig{
				Type:        NATIVE,
				DefaultRule: true,
			})
		}
	}

	c.Client = clients
	c.Server = servers
}

func (c *ClientConfig) fillDefault() {
	if c.Timeout == 0 {
		c.Timeout = 10
	}

	if c.IdleTimeout == 0 {
		c.IdleTimeout = 120
	}

	if c.Forbid == nil {
		c.Forbid = []string{"127.0.0.1/8", "10.0.0.0/8", "172.16.0.0/12", "192.168.0.0/16"}
	}
}
