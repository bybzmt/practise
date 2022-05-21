package main

import (
    "rsa"
    "log"
    "os"
    "flag"
)

var server = flag.String("server", "", "OpenVPN ServerName")
var name = flag.String("name", "", "OpenVPN UserName")

func isNotExist(path string) bool {
  _, err := os.Stat(path)

  return os.IsNotExist(err)
}

func main() {
    flag.Parse()

    if *server == "" {
        log.Fatalln("arg -server empty")
    }

    prefix := *server

    root_crt := prefix + "_ca.crt"
    root_key := prefix + "_ca.key"

    baseinfo := rsa.CertInformation{
        Country: []string{"CN"},
        Organization: []string{"WS"},
        IsCA: true,
        OrganizationalUnit: []string{"company"},
        EmailAddress: []string{"example@mail.com"},
        Locality: []string{"Locality"},
        Province: []string{"Province"},
        CommonName: prefix+"_ca",
        CrtName: root_crt,
        KeyName: root_key,
    }

    if (isNotExist(root_crt)) {
        err := rsa.CreateCRT(nil, nil, baseinfo)
        if err != nil {
            log.Fatalln("Create crt error,Error info:", err)
        }
        log.Println("Create root crt file:", root_crt, root_key)
    }

    baseinfo.CrtName = root_crt
    baseinfo.KeyName = root_key

    crt, pri, err := rsa.Parse(baseinfo.CrtName, baseinfo.KeyName)
    if err != nil {
        log.Fatalln("Parse crt error,Error info:", err)
    }

    server_crt := prefix + "_server.crt"
    server_key := prefix + "_server.key"

    if (isNotExist(server_crt)) {
        baseinfo.IsCA = false;
        baseinfo.CommonName = prefix
        baseinfo.CrtName = server_crt
        baseinfo.KeyName = server_key

        err = rsa.CreateCRT(crt, pri, baseinfo)
        if err != nil {
            log.Fatalln("Create crt error,Error info:", err)
        }

        log.Println("Create crt file:", server_crt, server_key)
    }

    if *name == "" {
        log.Fatalln("arg -name empty")
    }

    user_crt := prefix + "_" + *name + ".crt"
    user_key := prefix + "_" + *name + ".key"

    if (!isNotExist(user_crt)) {
        log.Fatalln("User Exist", user_crt)
    }

    baseinfo.IsCA = false;
    baseinfo.CommonName = *name
    baseinfo.CrtName = user_crt
    baseinfo.KeyName = user_key

    err = rsa.CreateCRT(crt, pri, baseinfo)
    if err != nil {
         log.Fatalln("Create crt error,Error info:", err)
    }

    log.Println("Create crt file:", user_crt, user_key)

}
