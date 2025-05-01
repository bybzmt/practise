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

var (
	ErrAllServerUnavailable = errors.New("Failed connect to all available shadowsocks server")
	ErrDial                 = errors.New("Dial Error")
	errForbidden            = errors.New("Forbidden")
)

type Server interface {
	Name() string
	Shadow(addr socks.RawAddr) (net.Conn, error)
	Match(addr socks.RawAddr) bool
	SetRules(rules []string)
	SetDNS(ips []string)
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
	var ss []Server
	for _, v := range servers {
		s, err := newServer(f.Timeout, &v)
		if err != nil {
			return nil, err
		}

		s.SetRules(v.Rules)
		s.SetDNS(v.DNS)

		ss = append(ss, s)
	}

	if len(ss) == 0 {
		return nil, errors.New("servers empty")
	}

	timeout := time.Duration(f.Timeout) * time.Second
	idleTimeout := time.Duration(f.IdleTimeout) * time.Second

	base := baseClient{}
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
		c := &socksClient{
			baseClient: base,
		}
		c.auth = f.Auth
		return c, nil

	case RELAY:
		c := &relayClient{
			baseClient: base,
		}
		addr, err := socks.ParseRawAddr(f.RelayTo)
		if err != nil {
			return nil, err
		}
		c.RelayTo = addr
		return c, nil

	case SHADOWSOCKS:
		c := &shadowClient{
			baseClient: base,
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

func newServer(timeout int, v *ServerConfig) (Server, error) {
	timeout2 := time.Duration(timeout) * time.Second

	switch strings.ToUpper(v.Type) {
	case NOPROXY:
		s2 := &serverNoProxy{
			baseProxy: baseProxy{
				proxy:   v.DefaultRule,
				timeout: timeout2,
			},
		}
		s2.init()
		return s2, nil

	case SOCKS:
		s2 := &socksProxy{
			baseProxy: baseProxy{
				proxy:   v.DefaultRule,
				timeout: timeout2,
			},
			addr: v.Addr,
		}

		if v.Cipher != "" {
			s2.auth = &socks.SimpleAuth{
				Username: v.Cipher,
				Password: v.Password,
			}
		}
		s2.init()
		return s2, nil
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
				proxy:   v.DefaultRule,
				timeout: timeout2,
			},
			shadow: t.StreamConn,
		}
		s2.init()
		return s2, nil

	default:
		return nil, errors.New("Server Type Error")
	}
}
