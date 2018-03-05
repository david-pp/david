
package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"net/url"
	"strings"
	"log"
)

// function test(callback) {
// 	var formData = {
// 		title : '标题',
// 		description : '测试一下！',
// 		registration_id : 'TkYIuuPIu5D3FS0jS9/Jpch4kX2+23KjvuG5pYGNZ6g=',
// 		restricted_package_name : 'com.ztgame.ztas',
// 		notify_type : 2,
// 		time_to_live : 3600000, 
// 		notify_id : 1,
// 	};

// 	var options = {
// 	  url: 'https://api.xmpush.xiaomi.com/v3/message/regid',
// 	  headers: {
// 	    'Authorization': 'key=nwSa9gFZB3MIWsqrRrsu9w=='
// 	  },
// 	  formData: formData
// 	};

// 	request.post(options, function optionalCallback(err, httpResponse, body) {
// 	  if (err) {
// 	    return console.error(' failed:', err);
// 	  }
// 	  console.log('successful!  Server responded with:', body);
// 	  callback()
// 	});
// }

func init() {
	log.SetPrefix("[push-xiaomi]")
	log.SetFlags(log.LstdFlags)
}

func main() {

	client := &http.Client{}

	postValue := url.Values {
		"title" : {"标题"},
		"description" : {"测试一下！"},
		"registration_id" : {"TkYIuuPIu5D3FS0jS9/Jpch4kX2+23KjvuG5pYGNZ6g="},
		"restricted_package_name" : {"com.ztgame.ztas"},
		"notify_type" : {"2"},
		"time_to_live" : {"3600000"}, 
		"notify_id" : {"1"},
	}

	req, err := http.NewRequest("POST", "https://api.xmpush.xiaomi.com/v3/message/regid",
		strings.NewReader(postValue.Encode()))
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	req.Header.Set("Content-Type",  "application/x-www-form-urlencoded")
	req.Header.Set("Authorization", "key=nwSa9gFZB3MIWsqrRrsu9w==")

 	resp, err := client.Do(req)

	defer resp.Body.Close()
	
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	log.Println(string(body))
}
