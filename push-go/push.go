package main

import (
	"fmt"
	"flag"
	"strings"
	"time"
	// "os"
	"io/ioutil"
	"net/http"
	"net/url"
	"database/sql"
	"encoding/json"
	"log"
	// "github.com/rs/zerolog"
	// "github.com/rs/zerolog/log"
	"github.com/garyburd/redigo/redis"
	_ "github.com/go-sql-driver/mysql"
)

// 推送的Redis Channel
var push_channel string = "push"

func init() {
	
	// debug := flag.Bool("debug", false, "sets log level to debug")
	flag.Parse()

	// log.Logger = log.Output(zerolog.ConsoleWriter{Out: os.Stderr})

	// zerolog.TimeFieldFormat = "2018-01-01 00:00:00"
	// // Default level for this example is info, unless debug flag is present
	// zerolog.SetGlobalLevel(zerolog.InfoLevel)
	// if *debug {
	// 	zerolog.SetGlobalLevel(zerolog.DebugLevel)
	// }

	if len(flag.Args()) > 0 {
		push_channel = flag.Arg(0)
	}

	fmt.Println(push_channel)

	go getTokenRoutine()
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
// 设备类型
//
const (
	Platform_Andriod        = 1
	Platform_iOS_Enterprise = 2
	Platform_iOS_AppStore   = 3
	Platform_iOS_Sandbox    = 4
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

	if Platform_Andriod == device.Platform {  // Andriod
		switch device.PushType {
		case 3:   // 华为
		push_to_huawei(device, message)
		default:  // 小米(默认)
		push_to_xiaomi(device, message)
		}
	} else {                   // iOS
		switch device.PushType {
		default:  // 小米(默认)
		push_to_xiaomi(device, message)
		}
	}
}

var xiaomi_notify_id = 0

func push_to_xiaomi(device *DeviceInfo, message *PushMessage) {
	fmt.Printf("%s %d %d\n", device.CID, device.Platform, device.PushType)	

	xiaomi_notify_id += 1

	var key string
	var postValue url.Values

	switch device.Platform {
		case Platform_Andriod: {
			key = "key=nwSa9gFZB3MIWsqrRrsu9w=="

			postValue = url.Values {
				"title" : { message.Title },
				"description" : { message.Content },
				"registration_id" : { device.CID },
				"restricted_package_name" : {"com.ztgame.ztas"},
				"notify_type" : {"2"},
				"time_to_live" : {"3600000"}, 
				"notify_id" : { fmt.Sprintf("%d",xiaomi_notify_id) },
			}
		}
		case Platform_iOS_Enterprise: {
			key = "key=pWR57EFBrRFgJHmg+oKQ5w=="

			postValue = url.Values {
				"aps_proper_fields.title" : { message.Title },
				"aps_proper_fields.body" : { message.Content },
				"registration_id" : { device.CID },
				"restricted_package_name" : {"com.ztgame.ztas"},
				"notify_type" : {"2"},
				"time_to_live" : {"3600000"}, 
				"notify_id" : { fmt.Sprintf("%d",xiaomi_notify_id) },
			}	
		}
		case Platform_iOS_AppStore: {
			key = "key=2nYdULuilijYKQdHykf1Vg=="

			postValue = url.Values {
				"aps_proper_fields.title" : { message.Title },
				"aps_proper_fields.body" : { message.Content },
				"registration_id" : { device.CID },
				"restricted_package_name" : {"com.ztgame.ztas"},
				"notify_type" : {"2"},
				"time_to_live" : {"3600000"}, 
				"notify_id" : { fmt.Sprintf("%d",xiaomi_notify_id) },
			}	
		}
		default:
			return
	}
	
	req, err := http.NewRequest("POST", "https://api.xmpush.xiaomi.com/v3/message/regid",
		strings.NewReader(postValue.Encode()))
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	req.Header.Set("Content-Type",  "application/x-www-form-urlencoded")
	req.Header.Set("Authorization", key)

	client := &http.Client{}

 	resp, err := client.Do(req)

	defer resp.Body.Close()
	
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	fmt.Println(postValue)
	fmt.Println(string(body))
}

func push_to_huawei(device *DeviceInfo, message *PushMessage) {
	fmt.Printf("%s %d %d\n", device.CID, device.Platform, device.PushType)

	cids := [1]string {
		device.CID,
	}

	data, err := json.Marshal(cids)
	if err != nil {
	        log.Fatal(err)
	}

	fmt.Println(string(data))

	pushMessage(huawei_token.AccessToken, message.Title, message.Content, string(data))
}


type TokenInfo struct {
        AccessToken string `json:"access_token"`
        ExpiresIn   int `json:"expires_in"`
}

var huawei_token TokenInfo

func getTokenRoutine() {

	for {
		huawei_token = getTokenInfo()

		fmt.Println(huawei_token)

		seconds := 1000
		if huawei_token.ExpiresIn > 0 {
			seconds = huawei_token.ExpiresIn*1000/2
		}

		// seconds = 2 // TODO: DELETE...

		timer1 := time.NewTimer(time.Duration(seconds) * time.Second)
		<-timer1.C
	}
}

func getTokenInfo() TokenInfo {
	tokenInfo := TokenInfo{}

    client := &http.Client{}
    var r http.Request
    r.ParseForm()
    r.Form.Add("grant_type", "client_credentials")
    r.Form.Add("client_secret", "c863103135cd5077c63cf7d2d5c7139b")
    r.Form.Add("client_id", "100114425")
    bodystr := strings.TrimSpace(r.Form.Encode())
    req, err := http.NewRequest("POST", "https://login.vmall.com/oauth2/token", strings.NewReader(bodystr))
    if err != nil {
            log.Fatal(err)
            return tokenInfo
    }
    req.Header.Set("Content-Type", "application/x-www-form-urlencoded")
    resp, err := client.Do(req)
    if err != nil {
            log.Fatal(err)
            return tokenInfo
    }

    defer resp.Body.Close()
    result, err := ioutil.ReadAll(resp.Body)
    if err != nil {
            log.Fatal(err)
            return tokenInfo
    }

    err = json.Unmarshal(result, &tokenInfo)
    if err != nil {
            log.Fatal(err)
            return tokenInfo
    }
    return tokenInfo
}


var  payloadTpl = `{
  "hps": {
    "msg": {
      "type": 3,
      "body": {
        "content": "%v",
        "title": "%v"
      },
      "action": {
        "type": 1,
        "param": {
           "intent": "#Intent;compo=com.ztgame.ztas/com.ztgame.tw.activity.LoadingActivity;"
        }
      }
    }
  }
}`

/**
 * 将数据发送至华为平台
 */
func pushMessage(accessToken, title, content, tokenList string) *error {

        url := "https://api.push.hicloud.com/pushsend.do?nsp_ctx=%7b%22ver%22%3a%221%22%2c+%22appId%22%3a%22100114425%22%7d"
        payload := fmt.Sprintf(payloadTpl, content, title)

        now := time.Now()
        nsp_ts := now.UnixNano() / 1000000000
        expireTime := fmt.Sprintf("%d-%d-%dT23:50", now.Year(), now.Month(), now.Day())

        client := &http.Client{}
        var r http.Request
        r.ParseForm()
        r.Form.Add("payload", payload)
        r.Form.Add("access_token", accessToken)
        r.Form.Add("nsp_svc", "openpush.message.api.send")
        r.Form.Add("nsp_ts", fmt.Sprintf("%v", nsp_ts))
        r.Form.Add("expire_time", expireTime)
        r.Form.Add("device_token_list", tokenList)
        bodystr := strings.TrimSpace(r.Form.Encode())
        req, err := http.NewRequest("POST", url, strings.NewReader(bodystr))
        if err != nil {
                log.Fatal(err)
                return &err
        }
        req.Header.Set("Content-Type", "application/x-www-form-urlencoded")
        resp, err := client.Do(req)
        if err != nil {
                log.Fatal(err)
                return &err
        }

        defer resp.Body.Close()
        result, err := ioutil.ReadAll(resp.Body)
        if err != nil {
                log.Fatal(err)
                return &err
        }
        fmt.Println(string(result))
        return nil
}
