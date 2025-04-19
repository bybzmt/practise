package main

import (
	"flag"
	"io"
	"log"
	"os"
	"os/signal"
	"ss/utils"
	"sync/atomic"
	"time"

	ss "ss/client"

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

	if *debug {
		log.Printf("%#v", f)
	}

	runClient(f)

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

func runClient(f ss.Config) {
	clients, err := ss.NewClient(&f)
	if err != nil {
		log.Fatalln(err)
	}

	for _, c := range clients {
		err := c.Listen()
		if err != nil {
			log.Fatalln(err)
		}
	}

	for _, c := range clients {
		log.Println("Starting Shadowsocks Client At", c.Addr())

		go cliState(c)
		go func(c ss.Client) {
			for {
				conn, err := c.Accept()
				if err != nil {
					log.Fatalln(err)
				}
				go c.Serve(conn)
			}
		}(c)
	}
}

func cliState(c ss.Client) {
	for range time.Tick(10 * time.Second) {
		num := atomic.LoadInt32(&utils.DefaultWatcher.Counter)

		t := c.Traffic().Clone()
		c.Traffic().Sub(&t)

		Outgoing := utils.FmtSize(10*time.Second, t.Outgoing)
		Incoming := utils.FmtSize(10*time.Second, t.Incoming)

		log.Println("state", "Conn", num, "Outgoing", Incoming, "Incoming", Outgoing)
	}
}
