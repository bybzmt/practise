
TOP=$(shell pwd)
export GOPATH=$(TOP)/gopath
export PATH:=$(GOPATH)/bin:$(PATH)

.PHONY: go godoc go-init js-init ui ui-debug all run env

all: go-init js-init go

go: ui
	go build

go-init:
	go get github.com/integrii/flaggy
	go get github.com/hashicorp/golang-lru
	go get github.com/aead/chacha20
	go get github.com/bybzmt/bolthold
	go get github.com/elazarl/go-bindata-assetfs
	go get github.com/jteeuwen/go-bindata/go-bindata

js-init:
	cd ./webUI && npm i

ui:
	cd ./webUI && npm run build
	go-bindata -pkg=ui -o ./ui/bindata.go ./webUI/dist/

#----debug tools ----
ui-debug:
	cd ./webUI && npm run dev

run:
	go-bindata -pkg=ui -debug -o ./ui/bindata.go ./webUI/dist/
	go run *.go -d

env:
	@set -e; \
	echo "#run this code in shell"
	export GOPATH="$(GOPATH)"
	export PATH="$(PATH)"

godoc:
	godoc -http=:6060
