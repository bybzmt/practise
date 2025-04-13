package main

import (
	"flag"
	"io"
	"log"
	"os"
	"ss/server"
	ss "ss/server"
	"ss/utils"
	"sync/atomic"
	"time"

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

	if f.Addr == "" {
		f.Addr = "127.0.0.1:1080"
	}

	if *debug {
		log.Printf("%#v", f)
	}

	runClient(f)
}

func loadConfig(file string) server.ServerConfig {
	var f server.ServerConfig

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

func runClient(f ss.ServerConfig) {
	client, err := ss.NewServer(&f)

	if *debug {
		log.Printf("%#v", client)
	}

	log.Println("Starting Shadowsocks Client At", f.Addr)

	go cliState(client)

	err = client.ListenAndServe()
	if err != nil {
		log.Println(err)
		os.Exit(1)
	}
}

func cliState(c *ss.Server) {
	for range time.Tick(10 * time.Second) {
		num := atomic.LoadInt32(&utils.DefaultWatcher.Counter)

		t := c.Traffic.Clone()
		c.Traffic.Sub(&t)

		Outgoing := utils.FmtSize(10*time.Second, t.Outgoing)
		Incoming := utils.FmtSize(10*time.Second, t.Incoming)

		log.Println("state", "Conn", num, "Outgoing", Incoming, "Incoming", Outgoing)
	}
}
