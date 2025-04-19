package client

import (
	"errors"
	"net"
	"ss/utils"
)

var (
	ErrAllServerUnavailable = errors.New("Failed connect to all available shadowsocks server")
	ErrDial                 = errors.New("Dial Error")
)

type Server interface {
	Name() string
	Shadow(addr utils.RawAddr) (net.Conn, error)
	Match(addr utils.RawAddr) bool
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
