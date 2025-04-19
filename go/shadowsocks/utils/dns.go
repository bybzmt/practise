package utils

import (
	"context"
	"math/rand"
	"net"
	"sync"
	"time"

	lru "github.com/hashicorp/golang-lru"
)

type DNS interface {
	LookupIPAddr(ctx context.Context, host string) ([]net.IPAddr, error)
}

type DNSDialer func(network, addr string) (net.Conn, error)

type dnsVal struct {
	ipaddr []net.IPAddr
	expire time.Time
}

type dnsLocker struct {
	l   sync.Mutex
	num int
}

type dns struct {
	dns      []string
	lru      *lru.Cache
	resolver net.Resolver
	l        sync.Mutex
	lh       map[string]*dnsLocker
}

func NewDNS(ips []string, dialer DNSDialer) DNS {
	if len(ips) == 0 {
		return net.DefaultResolver
	}

	if dialer == nil {
		return &net.Resolver{
			Dial: func(ctx context.Context, network, address string) (net.Conn, error) {
				ip := ips[rand.Intn(len(ips))]

				_, port, _ := net.SplitHostPort(address)
				addr := net.JoinHostPort(ip, port)

				return net.Dial(network, addr)
			},
		}
	}

	lru, _ := lru.New(2000)

	d := &dns{
		dns: ips,
		lru: lru,
		lh:  make(map[string]*dnsLocker),
	}

	d.resolver.Dial = func(ctx context.Context, network, address string) (net.Conn, error) {
		ip := ips[rand.Intn(len(ips))]

		_, port, _ := net.SplitHostPort(address)
		addr := net.JoinHostPort(ip, port)

		return dialer(network, addr)
	}

	return d
}

func (d *dns) _lookupIPAddr(ctx context.Context, host string) ([]net.IPAddr, error) {
	ipaddr, err := d.resolver.LookupIPAddr(ctx, host)
	if err != nil {
		Debug.Println("Lookup", host, err)
		return nil, err
	}

	Debug.Println("Lookup", host, ipaddr)

	v := dnsVal{
		ipaddr: ipaddr,
		expire: time.Now().Add(2 * time.Minute),
	}

	d.lru.Add(host, v)

	return ipaddr, nil
}

func (d *dns) LookupIPAddr(ctx context.Context, host string) ([]net.IPAddr, error) {
	d.l.Lock()
	l, ok := d.lh[host]
	if !ok {
		l = &dnsLocker{}
		d.lh[host] = l
	}
	l.num++
	d.l.Unlock()

	defer func() {
		d.l.Lock()
		defer d.l.Unlock()

		l, ok := d.lh[host]
		if ok {
			l.num--
			if l.num <= 0 {
				delete(d.lh, host)
			}
		}
	}()

	l.l.Lock()
	defer l.l.Unlock()

	if t, ok := d.lru.Get(host); ok {
		v := t.(dnsVal)

		if time.Now().After(v.expire) {
			return d._lookupIPAddr(ctx, host)
		}
	}

	return d._lookupIPAddr(ctx, host)
}
