package main

import (
	"fmt"
	"os"
	"strings"
	"github.com/google/uuid"
	"bufio"
	// "crypto/rand"
    // "encoding/base64"
    // "encoding/hex"

)

type Auth struct{
	F_name, L_name string
	//Age int
}


func GenerateSessionID() string {
    return uuid.New().String()
}



func isAlreadyLogedIn() bool{
	res := false	
	str, is:= os.LookupEnv("HTTP_IS_LOGGED")
	if is {
		if str == "true"{
			res = true
		}
	}
	return res
}

func fillStruct(usr *Auth){
	var str string
	const BS = 1024
	buf := make([]byte, BS)
	for {
		n, err := os.Stdin.Read(buf)
		if n > 0{
			str += string (buf[:n])
		}
		if err != nil{
			break
		}
		if n != BS {
			break
		}
	}
	splited := strings.Split(str, "\n")
	for i,miniStr := range splited{
		index := strings.Index(miniStr, "fname")
		if index >=0 {
			usr.F_name = strings.Trim(splited[i + 2], "\n")
			// usr.F_name = strings.Trim(usr.F_name, "\r")
			usr.F_name = strings.TrimSpace(usr.F_name)
		}else{
			index = strings.Index(miniStr, "lname")
		
			if index >= 0{
				usr.L_name = strings.Trim(splited[i + 2], "\n")
				// usr.L_name = strings.Trim(usr.L_name, "\r")
				usr.L_name = strings.TrimSpace(usr.L_name)

			}
		} 
	}
}

func checkUser(usr Auth)bool{
	res := false
	if usr.F_name=="said" || usr.F_name == "hamid" {
		if usr.F_name=="said" && usr.L_name == "lmachakil"{
			res = true;
		}
		if usr.F_name == "hamid" && usr.L_name == "zig"{
			res = true;
		}
	}
	return res
}


func setCookie(s Auth) string{
	sid := GenerateSessionID()
	res := "Set-Cookie: session-id=" + sid + ",fname="+s.F_name + ",lname=" + s.L_name
	return res 
}

func main(){
	var s Auth
	w := bufio.NewWriter(os.Stdout)
    defer w.Flush()
	ct_header := "Content-Type: text/html\n\n" 
	b := isAlreadyLogedIn()
	if b {
		msg := ct_header + "ur Already logged In!"
		fmt.Println(msg)
	}else{	
		fillStruct(&s)
		if checkUser(s){
			cookie_header := setCookie(s)	
			fmt.Fprintln(w, cookie_header)
			fmt.Fprintln(w, ct_header)
			fmt.Fprintln(w, "Logged Successfully")
		}else{
			fmt.Println(ct_header, "This User Is not Allowed!")
		}
	}
}
