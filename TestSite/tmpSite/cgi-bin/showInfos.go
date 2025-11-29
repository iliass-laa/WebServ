package main


import (
	"fmt"
	"os"
	"strings"
)


func getNameValue( name , str string) string{
	// var res string

	// index := strings.Index(str, name)
	substr := strings.Split(str, ",")
	for _,val := range(substr){
		if strings.Index(val, name) >= 0{
			res := strings.SplitN(val, "=", 2)
			return res[1]
		}
	}
	return ""
}

func main(){
	var msg string
	ct_header := "Content-Type: text/html\n\n"
	// str, i:= os.LookupEnv("HTTP_IS_LOGGED")
	cookies , j:= os.LookupEnv("HTTP_COOKIE")
	
	// if i && str == "true" {
		if j {
			// valCooki := strings.SplitN(cookie, "=",2)
			fname := getNameValue("fname", cookies)
			lname := getNameValue("lname", cookies)
			msg = ct_header + "Hi there  "+ fname + " " + lname
			fmt.Println(msg)
		}else{

			msg = ct_header + "No infos Founded For this User!"
			fmt.Println(msg)
		}
	// }else {
	// 	msg = ct_header + "Need to log In First !"
	// 	fmt.Println(msg)
	// }
}
