package socks

import (
	"encoding/binary"
	"io"
	"net"
	"strconv"
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
	case ATYP_IPV4:
		l = net.IPv4len
	case ATYP_IPV6:
		l = net.IPv6len
	case ATYP_DOMAINNAME:
		l = int(h[idLen]) + 1
	default:
		return nil, ErrAddrType
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
	case ATYP_IPV4, ATYP_IPV6:
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
	case ATYP_IPV4, ATYP_IPV6:
		host = net.IP(r[1 : len(r)-2]).String()
	case ATYP_DOMAINNAME:
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
		raw[0] = ATYP_IPV4
		copy(raw[1:], t)
	} else {
		raw = make([]byte, 1+net.IPv6len+2)
		raw[0] = ATYP_IPV6
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
	return ParseRawAddr(a.String())
}

func ParseRawAddr(addr string) (RawAddr, error) {
	h, p, err := net.SplitHostPort(addr)
	if err != nil {
		return nil, err
	}

	port, _ := strconv.ParseUint(p, 10, 16)

	if ip := net.ParseIP(h); ip != nil {
		return IP2RawAddr(ip, uint16(port)), nil
	}

	raw := make([]byte, 1+1+len(h)+2)
	raw[0] = ATYP_DOMAINNAME
	raw[1] = uint8(len(h))
	raw[len(raw)-2] = uint8((port & 0xff00) >> 8)
	raw[len(raw)-1] = uint8(port & 0xff)

	return RawAddr(raw), nil
}
