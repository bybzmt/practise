package client

import (
	"errors"
	"net"
	"ss/utils"
	"time"
)

var (
	ErrAllServerUnavailable = errors.New("Failed connect to all available shadowsocks server")
	ErrDial                 = errors.New("Dial Error")
)

type Client struct {
	addr     string
	idx      uint32
	listener net.Listener

	servers []Server

	Watcher utils.Watcher

	Traffic utils.Traffic

	idleTimeout time.Duration
	timeout     time.Duration
}

func (s *Client) ListenAndServe() (e error) {
	s.listener, e = net.Listen("tcp", s.addr)
	if e != nil {
		return e
	}

	for {
		c, e := s.listener.Accept()
		if e != nil {
			return e
		}
		go s.Serve(c)
	}
}

func (s *Client) Close() {
	s.listener.Close()

	for _, t := range s.servers {
		t.Close()
	}
}

func (s *Client) Serve(from net.Conn) {
	defer from.Close()

	from.SetReadDeadline(time.Now().Add(s.timeout))

	addr, err := utils.HandShake(from)
	if err != nil {
		s.Watcher.OnSocksInvalid(from.RemoteAddr(), err)
		return
	}

	host := addr.Host()

	if s.Watcher.Hijacker(host, from) {
		return
	}

	from = s.trafficConn(from, &s.Traffic, nil)

	server := s.match(addr)

	if server == nil {
		utils.Debug.Println("no match server")
		return
	}

	s.Watcher.OnProxyStart("proxy", from.RemoteAddr(), addr)
	defer func() {
		s.Watcher.OnProxyStop("proxy", from.RemoteAddr(), addr, err)
	}()

	to, err := server.Shadow(addr)
	if err != nil {
		utils.Debug.Println("Dial", err)
		return
	}
	defer to.Close()

	from2 := &utils.TickConn{
		Conn:         from,
		ReadTimeout:  s.idleTimeout,
		WriteTimeout: s.timeout,
	}

	to2 := &utils.TickConn{
		Conn:         to,
		ReadTimeout:  s.idleTimeout,
		WriteTimeout: s.timeout,
	}

	err = utils.Relay(from2, to2)
	return
}

func (s *Client) match(addr utils.RawAddr) Server {
	for _, t := range s.servers {
		if t.Match(addr) {
			return t
		}
	}

	return nil
}

func (s *Client) trafficConn(c net.Conn, now, end *utils.Traffic) *utils.TrafficConn {
	return &utils.TrafficConn{
		Conn: c,
		Now:  now,
		End:  end,
	}
}
