package utils

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"errors"
	"io"
	"net"
	"strconv"
)

var (
	errVer          = errors.New("socks version not supported")
	errMethod       = errors.New("socks only support 1 method now")
	errMethodFail   = errors.New("socks method negotiation failed")
	errAuthVer      = errors.New("socks server does not support user/password version 1")
	errAuth         = errors.New("socks user/password login failed")
	errReqExtraData = errors.New("socks request get extra data")
	errCmd          = errors.New("socks command not supported")
	errAddrType     = errors.New("socks addr type not supported")
)

const (
	SOCKS_VER              byte = 5
	SOCKS_METHOD_ANONYMOUS byte = 0
	SOCKS_METHOD_USERNAME  byte = 2
	SOCKS_METHOD_INVALID   byte = 0xff
	SOCKS_CMD_CONNECT      byte = 1
	SOCKS_CMD_BIND         byte = 2
	SOCKS_CMD_UDP          byte = 3
	SOCKS_AUTH_VER         byte = 1
	SOCKS_SUCCESS          byte = 0
	SOCKS_ATYP_IPV4        byte = 1
	SOCKS_ATYP_DOMAINNAME  byte = 3
	SOCKS_ATYP_IPV6        byte = 4
	SOCKS_RSV              byte = 0
	SOCKS_FAIL             byte = 1
	SOCKS_ERR_FORBIDDEN    byte = 2
	SOCKS_ERR_NET          byte = 3
	SOCKS_ERR_DOMAIN       byte = 4
	SOCKS_ERR_REJECT       byte = 5
	SOCKS_ERR_ADDR         byte = 8
)

// The RawAddr is formed as follows:
// +------+----------+----------+
// | ATYP | DST.ADDR | DST.PORT |
// +------+----------+----------+
// |  1   | Variable |    2     |
// +------+----------+----------+
type RawAddr []byte

func ReadRawAddr(r io.Reader) (RawAddr, error) {
	idAtyp := 0
	idLen := 1
	h := [2]byte{}

	if _, err := io.ReadFull(r, h[:]); err != nil {
		return nil, err
	}

	var l int
	switch h[idAtyp] {
	case SOCKS_ATYP_IPV4:
		l = net.IPv4len
	case SOCKS_ATYP_IPV6:
		l = net.IPv6len
	case SOCKS_ATYP_DOMAINNAME:
		l = int(h[idLen]) + 1
	default:
		return nil, errAddrType
	}

	// atype(1) + len + prot(2)
	buf := make([]byte, 1+l+2)
	copy(buf, h[:])

	if _, err := io.ReadFull(r, buf[2:]); err != nil {
		return nil, err
	}

	return RawAddr(buf), nil
}

func (r RawAddr) ToIP() net.IP {
	if len(r) < 5 {
		return nil
	}

	switch r[0] {
	case SOCKS_ATYP_IPV4, SOCKS_ATYP_IPV6:
		return net.IP(r[1 : len(r)-2])
	}

	return nil
}

func (r RawAddr) Host() string {
	// atype(1) + len(min 1) + port(2)
	if len(r) < 4 {
		return ""
	}

	var host string
	switch r[0] {
	case SOCKS_ATYP_IPV4, SOCKS_ATYP_IPV6:
		host = net.IP(r[1 : len(r)-2]).String()
	case SOCKS_ATYP_DOMAINNAME:
		host = string(r[2 : len(r)-2])
	}
	return host
}

func (r RawAddr) Type() uint8 {
	if len(r) > 0 {
		return r[0]
	}
	return 0
}

func (r RawAddr) Port() uint16 {
	if len(r) < 2 {
		return 0
	}
	return binary.BigEndian.Uint16(r[len(r)-2:])
}

func (r RawAddr) PortString() string {
	if len(r) < 2 {
		return ""
	}
	return strconv.Itoa(int(r.Port()))
}

func (r RawAddr) String() string {
	return net.JoinHostPort(r.Host(), r.PortString())
}

// interface net.Addr
func (r RawAddr) Network() string {
	return "tcp"
}

func IP2RawAddr(ip net.IP, port uint16) RawAddr {
	var raw []byte

	if t := ip.To4(); t != nil {
		raw = make([]byte, 1+net.IPv4len+2)
		raw[0] = SOCKS_ATYP_IPV4
		copy(raw[1:], t)
	} else {
		raw = make([]byte, 1+net.IPv6len+2)
		raw[0] = SOCKS_ATYP_IPV6
		copy(raw[1:], ip)
	}

	raw[len(raw)-2] = uint8((port & 0xff00) >> 8)
	raw[len(raw)-1] = uint8((port & 0xff))

	return RawAddr(raw)
}

func Addr2RawAddr(a net.Addr) (RawAddr, error) {
	if rel, ok := a.(RawAddr); ok {
		return rel, nil
	}
	return Parse2RawAddr(a.String())
}

func Parse2RawAddr(addr string) (RawAddr, error) {
	h, p, err := net.SplitHostPort(addr)
	if err != nil {
		return nil, err
	}

	port, _ := strconv.ParseUint(p, 10, 16)

	if ip := net.ParseIP(h); ip != nil {
		return IP2RawAddr(ip, uint16(port)), nil
	}

	raw := make([]byte, 1+1+len(h)+2)
	raw[0] = SOCKS_ATYP_DOMAINNAME
	raw[1] = uint8(len(h))
	raw[len(raw)-2] = uint8((port & 0xff00) >> 8)
	raw[len(raw)-1] = uint8(port & 0xff)

	return RawAddr(raw), nil
}

type dalyWriter struct {
	*bufio.Reader
	*bufio.Writer
}

func (b *dalyWriter) Read(p []byte) (n int, err error) {
	if b.Writer.Buffered() > 0 {
		if err := b.Flush(); err != nil {
			return 0, err
		}
	}

	return b.Reader.Read(p)
}

type Socks struct {
	net.Conn
	rw     dalyWriter
	bind   RawAddr
	auth   *SimpleAuth
	method byte
}

func NewSocks(c net.Conn, auth *SimpleAuth) *Socks {
	rb := bufio.NewReader(c)
	wb := bufio.NewWriter(c)

	method := SOCKS_METHOD_ANONYMOUS
	if auth != nil {
		method = SOCKS_METHOD_USERNAME
	}

	return &Socks{
		Conn:   c,
		auth:   auth,
		rw:     dalyWriter{Reader: rb, Writer: wb},
		method: method,
	}
}

func (s *Socks) Dial(addr RawAddr) error {
	if _, err := s.rw.Write([]byte{SOCKS_VER, 1, s.method}); err != nil {
		return err
	}

	if err := s.writeAuth(); err != nil {
		return err
	}

	if _, err := s.rw.Write([]byte{SOCKS_VER, SOCKS_CMD_CONNECT, SOCKS_RSV}); err != nil {
		return err
	}

	if _, err := s.rw.Write(addr); err != nil {
		return err
	}

	resp := [3]byte{}

	if _, err := io.ReadFull(&s.rw, resp[:2]); err != nil {
		return err
	}

	if resp[0] != SOCKS_VER {
		return errVer
	} else if resp[1] != s.method {
		return errMethodFail
	}

	if s.auth != nil {
		if _, err := io.ReadFull(&s.rw, resp[:2]); err != nil {
			return err
		}

		if resp[0] != SOCKS_AUTH_VER {
			return errAuthVer
		} else if resp[1] != SOCKS_SUCCESS {
			return errAuth
		}
	}

	if _, err := io.ReadFull(&s.rw, resp[:]); err != nil {
		return err
	}

	if resp[0] != SOCKS_VER {
		return errVer
	} else if resp[1] != SOCKS_SUCCESS {
		return errMethodFail
	}

	var err error
	s.bind, err = ReadRawAddr(&s.rw)
	if err != nil {
		return err
	}

	return nil
}

func (s *Socks) HandShake() (RawAddr, error) {
	req := [3]byte{}

	if err := s.checkMethod(); err != nil {
		return nil, err
	}

	if err := s.checkAuth(); err != nil {
		return nil, err
	}

	if _, err := io.ReadFull(&s.rw, req[:]); err != nil {
		return nil, err
	}

	if req[0] != SOCKS_VER {
		return nil, errVer
	}
	if req[1] != SOCKS_CMD_CONNECT {
		return nil, errCmd
	}

	addr, err := ReadRawAddr(&s.rw)
	if err != nil {
		return nil, err
	}

	return addr, nil
}

func (s *Socks) RespDial(rep byte) error {
	if _, err := s.rw.Write([]byte{SOCKS_VER, rep, SOCKS_RSV}); err != nil {
		return err
	}

	if _, err := s.rw.Write(s.bind); err != nil {
		return err
	}

	return nil
}

func (s *Socks) Read(b []byte) (n int, err error) {
	return s.rw.Read(b)
}

func (s *Socks) Write(p []byte) (nn int, err error) {
	return s.rw.Write(p)
}

func (s *Socks) writeAuth() error {
	if s.auth == nil {
		return nil
	}

	if _, err := s.rw.Write([]byte{SOCKS_AUTH_VER}); err != nil {
		return err
	}

	if _, err := s.rw.Write([]byte{byte(len(s.auth.Username))}); err != nil {
		return err
	}

	if _, err := s.rw.Write([]byte(s.auth.Username)); err != nil {
		return err
	}

	if _, err := s.rw.Write([]byte{byte(len(s.auth.Password))}); err != nil {
		return err
	}

	if _, err := s.rw.Write([]byte(s.auth.Password)); err != nil {
		return err
	}

	return nil
}

func (s *Socks) checkAuth() error {
	if s.auth == nil {
		return nil
	}

	ver, err := s.rw.ReadByte()
	if err != nil {
		return err
	}

	if ver != SOCKS_AUTH_VER {
		return errAuthVer
	}

	ul, err := s.rw.ReadByte()
	if err != nil {
		return err
	}

	user := make([]byte, ul)
	if _, err := io.ReadFull(&s.rw, user); err != nil {
		return err
	}

	pl, err := s.rw.ReadByte()
	if err != nil {
		return err
	}

	pass := make([]byte, pl)
	if _, err := io.ReadFull(&s.rw, pass); err != nil {
		return err
	}

	if !bytes.Equal([]byte(s.auth.Username), user) ||
		!bytes.Equal([]byte(s.auth.Password), pass) {
		return errAuth
	}

	return nil
}

func (s *Socks) checkMethod() error {
	req := [3]byte{}

	if _, err := io.ReadFull(&s.rw, req[:2]); err != nil {
		return err
	}

	if req[0] != SOCKS_VER {
		return errVer
	}

	methods := make([]byte, int(req[1]))
	if _, err := io.ReadFull(&s.rw, methods); err != nil {
		return err
	}

	if idx := bytes.IndexByte(methods, s.method); idx < 0 {
		if _, err := s.rw.Write([]byte{SOCKS_VER, SOCKS_METHOD_INVALID}); err != nil {
			s.rw.Flush()
			return err
		}
		return errMethodFail
	}

	if _, err := s.rw.Write([]byte{SOCKS_VER, s.method}); err != nil {
		return err
	}

	return nil
}

func splitHostPort(addr string) (host string, port uint16, err error) {
	host, portStr, err := net.SplitHostPort(addr)
	if err != nil {
		return "", 0, err
	}
	portInt, err := strconv.ParseUint(portStr, 10, 16)
	if err != nil {
		return "", 0, err
	}
	port = uint16(portInt)
	return
}
