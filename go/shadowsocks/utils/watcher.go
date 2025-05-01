package utils

import (
	"net"
	"ss/socks"
	"sync/atomic"
)

type Watcher interface {
	HandShake(from net.Addr, err error)
	NoServer(to socks.RawAddr)
	Create(server string, from, to net.Addr) ProxyWatcher
}

type ProxyWatcher interface {
	ShadowInvalid(err error)
	Proxy()
	Relay(err error)
}

var DefaultWatcher = &watcher{}

type watcher struct {
	Counter int32
}

type proxyWatcher struct {
	w      *watcher
	server string
	from   net.Addr
	to     net.Addr
}

func (w *watcher) HandShake(from net.Addr, err error) {
	Debug.Println("HandShake", from, err)
}

func (this *watcher) NoServer(to socks.RawAddr) {
	Debug.Println(to.String() + " No Match Server")
}

func (w *watcher) Create(server string, from, to net.Addr) ProxyWatcher {
	pw := &proxyWatcher{
		w:      w,
		server: server,
		from:   from,
		to:     to,
	}
	return pw
}

func (pw *proxyWatcher) ShadowInvalid(err error) {
	Debug.Println("ShadowInvalid", pw.server, err)
}

func (pw *proxyWatcher) Proxy() {
	atomic.AddInt32(&pw.w.Counter, 1)
	Debug.Println("ProxyStart", pw.server, pw.from, "<=>", pw.to)
}

func (pw *proxyWatcher) Relay(err error) {
	atomic.AddInt32(&pw.w.Counter, -1)
	Debug.Println("ProxyStop", pw.server, pw.from, "<=>", pw.to, err)
}
