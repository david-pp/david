package push

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"strings"
)

var xiaomi_notify_id = 0

func push_by_device(device, title, content, platform, callback) {

	xiaomi_notify_id += 1
 	
	client := &http.Client{}

	req, err := http.NewRequest("POST", "https://api.xmpush.xiaomi.com/v3/message/regid",
		strings.NewReader("title=标题&description=测试一下！&registration_id=TkYIuuPIu5D3FS0jS9/Jpch4kX2+23KjvuG5pYGNZ6g=&restricted_package_name=com.ztgame.ztas&notify_type=2&time_to_live=3600000&notify_id=1"))
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

	fmt.Println(string(body))
}

