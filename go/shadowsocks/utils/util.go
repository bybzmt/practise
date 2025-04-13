package utils

import (
	"io"
	"net"
	"strings"
    "time"
    "fmt"
)

type Creater func(net.Conn) net.Conn

func Relay(a, b net.Conn) (err error) {
	if t, ok := a.(*net.TCPConn); ok {
		t.SetNoDelay(true)
		t.SetKeepAlive(true)
	}
	if t, ok := b.(*net.TCPConn); ok {
		t.SetNoDelay(true)
		t.SetKeepAlive(true)
	}

	ch := make(chan error, 2)

	go func() {
		_, e := io.Copy(a, b)
		ch <- e
	}()
	go func() {
		_, e := io.Copy(b, a)
		ch <- e
	}()

	//first err
	return <-ch
}

func StrSplit(str string) (out []string) {
	for _, tm := range strings.Split(str, "\n") {
		for _, t := range strings.Split(tm, "\r") {
			t = strings.TrimSpace(t)
			if len(t) > 0 {
				if t[0] != '#' {
					out = append(out, t)
				}
			}
		}
	}
	return
}


const (
	s_kb = 1024
	s_mb = 1024 * 1024
	s_gb = 1024 * 1024 * 1024
)

func FmtSize(t time.Duration, i int64) string {
	num := float64(i) / (float64(t) / float64(time.Second))

	if num > s_gb {
		return fmt.Sprintf("%.2fGB/s", num/s_gb)
	} else if num > s_mb {
		return fmt.Sprintf("%.2fMB/s", num/s_mb)
	} else if num > s_kb {
		return fmt.Sprintf("%.2fKB/s", num/s_kb)
	} else {
		return fmt.Sprintf("%.2fB/s", num)
	}
}
