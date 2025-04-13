package utils

import (
	"net"
	"sync/atomic"
)

type Watcher interface {
	// Client HandShake Error
	OnSocksInvalid(from net.Addr, err error)
	// Server HandShake Error
	OnShadowInvalid(from net.Addr, err error)
	OnProxyStart(mode string, from, to net.Addr)
	OnProxyStop(mode string, from, to net.Addr, err error)
	Hijacker(host string, c net.Conn) bool
}

var DefaultWatcher = &defaultWatcher{}

type defaultWatcher struct {
	Counter int32
}

func (w *defaultWatcher) OnSocksInvalid(from net.Addr, err error) {
	Debug.Println("SocksInvalid", from, err)
}

func (w *defaultWatcher) OnShadowInvalid(from net.Addr, err error) {
	Debug.Println("ShadowInvalid", from, err)
}

func (w *defaultWatcher) OnProxyStart(mode string, from, to net.Addr) {
	Debug.Println("ProxyStart", mode, from, "<=>", to)
	atomic.AddInt32(&w.Counter, 1)
}

func (w *defaultWatcher) OnProxyStop(mode string, from, to net.Addr, err error) {
	Debug.Println("ProxyStop", mode, from, "<=>", to, err)

	atomic.AddInt32(&w.Counter, -1)
}

func (this *defaultWatcher) Hijacker(host string, c net.Conn) bool {
	return false
}
