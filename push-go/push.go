package main

import (
	"fmt"
	"flag"
	"os"
	"database/sql"
	"encoding/json"
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
	"github.com/garyburd/redigo/redis"
	_ "github.com/go-sql-driver/mysql"
	
)

// 推送的Redis Channel
var push_channel string = "push"

func init() {
	
	debug := flag.Bool("debug", false, "sets log level to debug")
	flag.Parse()

	log.Logger = log.Output(zerolog.ConsoleWriter{Out: os.Stderr})

	zerolog.TimeFieldFormat = "2018-01-01 00:00:00"
	// Default level for this example is info, unless debug flag is present
	zerolog.SetGlobalLevel(zerolog.InfoLevel)
	if *debug {
		zerolog.SetGlobalLevel(zerolog.DebugLevel)
	}

	if len(flag.Args()) > 0 {
		push_channel = flag.Arg(0)
	}

	fmt.Println(push_channel)
}

var mysql *sql.DB

func init_mysql() *sql.DB {
    // mysql
    db, err := sql.Open("mysql", "david:123456@tcp(127.0.0.1)/david")
	if err != nil {
	    fmt.Println("Connect to mysql error", err)
        return nil
	}

	err = db.Ping()
	if err != nil {
		fmt.Println("Connect to mysql error", err.Error())
		return nil
	}

	return db
}

//
// 推送类型
//
const (
	MsgType_Private = 1
	MsgType_Sept    = 5
	MsgType_Union   = 6
)

//
// 推送消息
//
type PushMessage struct {
	Type int `json:"type"`
	Title string `json:"title"`
	Content string `json:"content"`
	Id uint32 `json:"id"`
}

//
// 设备信息
//
type DeviceInfo struct {
	CID string
	Platform uint32
	PushType uint32
}

func main() {
    redis_cli, err := redis.Dial("tcp", "127.0.0.1:6379")
    if err != nil {
        fmt.Println("Connect to redis error", err)
        return
    }
    defer redis_cli.Close()

    mysql = init_mysql()
    defer mysql.Close()

	psc := redis.PubSubConn{Conn: redis_cli}
	psc.Subscribe(push_channel)
	for {
	    switch v := psc.Receive().(type) {
	    case redis.Message:
	        fmt.Printf("%s: message: %T,%s\n", v.Channel, v.Data, v.Data)
	   
	       	message := &PushMessage{}
	        if err := json.Unmarshal(v.Data, &message); err != nil {
	        	fmt.Println("json error:", err)
        	} else {
        		process_message(message)
        	}
	    case redis.Subscription:
	        fmt.Printf("%s: %s %d\n", v.Channel, v.Kind, v.Count)
	    case error:
	    	fmt.Printf("Error..\n")
	        // return v
	    }
	}
}

func process_message(message *PushMessage) {
	fmt.Println(*message)

	sql := "SELECT CID,PLATFORM,PUSHTYPE FROM APP_DEVICE WHERE "
	switch message.Type {
		case MsgType_Private:
			sql += fmt.Sprintf("CHARID=%d ORDER BY UPDATETIME DESC LIMIT 1", message.Id)
		case MsgType_Sept:
			sql += fmt.Sprintf("SEPTID=%d LIMIT 50", message.Id)
		case MsgType_Union:
			sql += fmt.Sprintf("UNIONID=%d", message.Id)
	}

	fmt.Println(sql)

    rows, err := mysql.Query(sql)
	if err != nil {
		fmt.Println("Connect to mysql error", err)
        return
	}

	for rows.Next() {

       var (
                cid   string
                platform uint32
                pushtype uint32
        )

		err := rows.Scan(&cid, &platform, &pushtype)
        if  err != nil {
                fmt.Printf(err.Error())
        } else {
        	device := &DeviceInfo{CID: cid, Platform:platform, PushType: pushtype}

        	push_to_device(device, message)
        }
	}
}

func push_to_device(device *DeviceInfo, message *PushMessage) {
	fmt.Printf("%s %d %d\n", device.CID, device.Platform, device.PushType)	

	if 1 == device.Platform {  // Andriod
		switch device.PushType {
		case 3:   // 华为
		default:  // 小米(默认)
		}
	} else {                   // iOS
		switch device.PushType {
		default:  // 小米(默认)
		}
	}
}
