package serve

import (
	"errors"
	"net"
	"ss/shadow"
	"ss/socks"
	"ss/utils"
	"strings"
	"time"

	"golang.org/x/crypto/ssh"
)

type Proxy interface {
	Name() string
	Shadow(addr socks.RawAddr) (net.Conn, error)
	Match(addr socks.RawAddr) bool
}

type Serve interface {
	Listen() error
	Accept() (net.Conn, error)
	Close() error
	Addr() net.Addr
	Serve(from net.Conn)
	Traffic() *utils.Traffic
	SetWatcher(w utils.Watcher)
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
	var ss []Proxy
	for _, v := range proxy {
		s, err := newProxy(f.Timeout, v)
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

	base := serveBase{}
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
	case SOCKS:
		c := &serveSocks{
			serveBase: base,
		}
		c.auth = f.Auth

		if f.RuleAddr != "" {
			addr, err := socks.ParseRawAddr(f.RuleAddr + ":0")
			if err != nil {
				return nil, err
			}
			c.ruleAddr = addr
		}

		return c, nil

	case RELAY:
		c := &serveRelay{
			serveBase: base,
		}
		addr, err := socks.ParseRawAddr(f.RelayTo)
		if err != nil {
			return nil, err
		}
		c.RelayTo = addr

		if f.RuleAddr != "" {
			addr, err := socks.ParseRawAddr(f.RuleAddr + ":0")
			if err != nil {
				return nil, err
			}
			c.ruleAddr = addr
		}
		return c, nil

	case SHADOWSOCKS:
		c := &serveShadow{
			serveBase: base,
		}

		if f.RuleAddr != "" {
			addr, err := socks.ParseRawAddr(f.RuleAddr + ":0")
			if err != nil {
				return nil, err
			}
			c.ruleAddr = addr
		}

		var key []byte
		t, err := shadow.PickCipher(f.Auth.Username, key, f.Auth.Password)
		if err != nil {
			return nil, err
		}
		c.shadow = t.StreamConn
		return c, nil

	case SHADOWSOCKS2:
		c := &serveShadow2{
			serveBase: base,
		}

		if f.RuleAddr != "" {
			addr, err := socks.ParseRawAddr(f.RuleAddr + ":0")
			if err != nil {
				return nil, err
			}
			c.ruleAddr = addr
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

func newProxy(timeout int, v *ProxyConfig) (Proxy, error) {
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

	case SSH:
		var c *ssh.ClientConfig

		if strings.Contains(v.Auth.Password, "KEY-----") {
			signer, err := ssh.ParsePrivateKey([]byte(v.Auth.Password))
			if err != nil {
				return nil, err
			}

			c = &ssh.ClientConfig{
				User: v.Auth.Username,
				Auth: []ssh.AuthMethod{
					ssh.PublicKeys(signer),
				},
				HostKeyCallback: ssh.InsecureIgnoreHostKey(),
			}
		} else {
			c = &ssh.ClientConfig{
				User: v.Auth.Username,
				Auth: []ssh.AuthMethod{
					ssh.Password(v.Auth.Password),
				},
				HostKeyCallback: ssh.InsecureIgnoreHostKey(),
			}
		}

		s2 := &proxySSH{
			proxyBase: base,
			addr:      v.Addr,
			sshConfig: c,
		}
		s2.init()
		return s2, nil

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

	case SHADOWSOCKS2:
		if v.Auth == nil {
			return nil, errors.New("shadowsocks Auth nil")
		}

		var key []byte
		t, err := shadow.PickCipher(v.Auth.Username, key, v.Auth.Password)
		if err != nil {
			return nil, err
		}

		s2 := &proxyShadow2{
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
