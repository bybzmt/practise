package main

import (
	"encoding/json"
	"flag"
	"io"
	"log"
	"os"
	"os/signal"
	"ss/utils"
	"sync/atomic"
	"time"

	ss "ss/serve"

	"github.com/BurntSushi/toml"
)

var (
	debug      = flag.Bool("debug", false, "debug mode")
	configFile = flag.String("config", "./config.toml", "config file")
)

func main() {
	flag.Parse()

	if *debug {
		utils.SetDebug(true)
	}

	f := loadConfig(*configFile)
	f.FillDefault()

	if *debug {
		js, _ := json.MarshalIndent(&f, "", "  ")
		log.Printf("%s", string(js))
	}

	runServer(f)

	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, os.Interrupt)
	<-sigChan
}

func loadConfig(file string) ss.Config {
	var f ss.Config

	if file == "" {
		return f
	}

	str, err := loadFile(*configFile)
	if err != nil {
		log.Fatalln(err)
	}

	err = toml.Unmarshal([]byte(str), &f)
	if err != nil {
		log.Fatalln(err)
	}

	return f
}

func loadFile(file string) (string, error) {
	f, err := os.Open(file)
	if err != nil {
		return "", err
	}
	defer f.Close()

	b, err := io.ReadAll(f)
	if err != nil {
		return "", err
	}

	return string(b), nil
}

func runServer(f ss.Config) {
	server, err := ss.NewServe(&f)
	if err != nil {
		log.Fatalln(err)
	}

	for _, c := range server {
		err := c.Listen()
		if err != nil {
			log.Fatalln(err)
		}
	}

	if err := ss.PrivilegeDropping(f.Setgid, f.Setuid); err != nil {
		log.Fatalln(err)
	}

	for _, c := range server {
		log.Println("Starting Shadowsocks Client At", c.Addr())

		go func(c ss.Serve) {
			for {
				conn, err := c.Accept()
				if err != nil {
					log.Fatalln(err)
				}
				go c.Serve(conn)
			}
		}(c)
	}

	go cliStates(&f, server)
}

func cliStates(f *ss.Config, serve []ss.Serve) {
	if f.StateTick == nil || *f.StateTick < 1 {
		return
	}

	for range time.Tick(time.Duration(*f.StateTick) * time.Second) {
		for _, c := range serve {
			cliState(*f.StateTick, c)
		}
	}
}

func cliState(tick int, c ss.Serve) {
	num := atomic.LoadInt32(&utils.DefaultWatcher.Counter)

	t := c.Traffic().Clone()
	c.Traffic().Sub(&t)

	Outgoing := utils.FmtSize(time.Duration(tick)*time.Second, t.Outgoing)
	Incoming := utils.FmtSize(time.Duration(tick)*time.Second, t.Incoming)

	log.Println("state", c.Addr(), "Conn", num, "Outgoing", Incoming, "Incoming", Outgoing)
}
