package server

import (
	"net"
	shadow "ss/shadow"
	"ss/utils"
	"time"
)

type ServerConfig struct {
	Addr        string
	Cipher      string
	Password    string
	Timeout     int
	IdleTimeout int
}

type Server struct {
	shadow utils.Creater
	Addr   string

	Watcher utils.Watcher

	Traffic     utils.Traffic
	idleTimeout time.Duration
	timeout     time.Duration
}

func NewServer(c *ServerConfig) (*Server, error) {
	s := &Server{
		Watcher:     utils.DefaultWatcher,
		timeout:     time.Duration(c.Timeout) * time.Second,
		idleTimeout: time.Duration(c.IdleTimeout) * time.Second,
	}

	var key []byte
	t, err := shadow.PickCipher(c.Cipher, key, c.Password)
	if err != nil {
		return nil, err
	}

	s.shadow = t.StreamConn

	return s, nil
}

func (s *Server) ListenAndServe() error {
	l, err := net.Listen("tcp", s.Addr)
	if err != nil {
		return err
	}

	for {
		c, e := l.Accept()
		if e != nil {
			continue
		}
		go s.Serve(c)
	}
}

func (s *Server) Serve(c net.Conn) {
	defer c.Close()

	c.SetReadDeadline(time.Now().Add(s.timeout))

	from := s.shadow(s.trafficConn(c))

	addr, err := utils.ReadRawAddr(from)
	if err != nil {
		s.Watcher.OnShadowInvalid(c.RemoteAddr(), err)
		return
	}

	s.Watcher.OnProxyStart("on", c.RemoteAddr(), addr)
	defer func() {
		s.Watcher.OnProxyStop("on", c.RemoteAddr(), addr, err)
	}()

	to, err := net.DialTimeout(addr.Network(), addr.String(), s.timeout)
	if err != nil {
		utils.Debug.Println("Dial", err)
		return
	}
	defer to.Close()

	err = utils.Relay(s.tickConn(from), s.tickConn(to))
	return
}

func (s *Server) trafficConn(c net.Conn) *utils.TrafficConn {
	return &utils.TrafficConn{
		Conn: c,
		Now:  &s.Traffic,
	}
}

func (s *Server) tickConn(c net.Conn) *utils.TickConn {
	return &utils.TickConn{
		Conn:         c,
		ReadTimeout:  s.idleTimeout,
		WriteTimeout: s.timeout,
	}
}
