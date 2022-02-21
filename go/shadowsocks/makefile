
.PHONY: go godoc js-init ui ui-debug all run

all: js-init go

go: ui
	go build

js-init:
	cd ./webUI && npm i

ui:
	cd ./webUI && npm run build

#----debug tools ----
ui-debug:
	cd ./webUI && npm run dev

run:
	go run *.go -d

godoc:
	godoc -http=:6060
