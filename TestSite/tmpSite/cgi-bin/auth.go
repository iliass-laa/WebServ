package main

import (
	"fmt"
	"os"
	"strings"
	"github.com/google/uuid"
)
func GenerateSessionID() string {
    return uuid.New().String()
}
type Auth struct{
	F_name, L_name string
	//Age int
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
		}else{
			index = strings.Index(miniStr, "lname")
		
			if index >= 0{
				usr.L_name = strings.Trim(splited[i + 2], "\n")

			}
		} 
	}
	fmt.Fprintf(os.Stderr, "Usr first Name:%s\n", usr.F_name)
	fmt.Fprintf(os.Stderr, "Usr last Name:%s\n", usr.L_name)
}

func checkUser(usr Auth)bool{
	res := false
	usr.F_name = strings.TrimSpace(usr.F_name)
	// usr.F_name = strings.Trim(usr.F_name, "\n")
	usr.L_name = strings.TrimSpace(usr.L_name)
	fmt.Fprintf (os.Stderr, "CHECK USR>>>>LNAME :%s\n", usr.L_name)
	fmt.Fprintf (os.Stderr, "CHECK USR>>>>LNAME len:%d\n", len(usr.L_name))
	if usr.F_name=="said" || usr.F_name == "hamid" {
		fmt.Fprintf(os.Stderr,">>>ITS EITHER SAID OR HAMID <<\n")
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
	res := "Set-Cookie: session-id=" + sid+",fname="+s.F_name + ",lname=" + s.L_name
	return res 
	// return fmt.Sprintf("Set-Cookie: session-id=%s,fname=%s,lname=%s",sid, s.F_name, s.L_name) 
	//"Set-Cookie: " +"session-id=45" +",fname=" + s.F_name +",lname=" + s.L_name 
}


func main(){
	var s Auth
	ct_header := "Content-Type: text/html\n\n" 
	//var headers []string = []string{"", "Content-Type: text/html\n\n"}
	b := isAlreadyLogedIn()
	if b {
		msg := ct_header + "ur Already logged In!"
		fmt.Println(msg)
	}else{	
		fillStruct(&s)
		fmt.Fprintf(os.Stderr, "F>>%s\n", s.F_name)
		fmt.Fprintf(os.Stderr, "L>>%s\n", s.L_name)
		if checkUser(s){
			cookie_header := setCookie(s)
			// cookie_header := "Set-Cookie: session-id=45,fname=said,lname=lmachakil"
			fmt.Println(cookie_header)
			fmt.Println(ct_header)
			fmt.Println("Logged Successfully")
			fmt.Fprintf(os.Stderr, "header[0]:%s\n", cookie_header)
			fmt.Fprintf(os.Stderr, "header[1]:%s\n", ct_header)
		}else{
			fmt.Fprintf(os.Stderr, ">>>>fname ::%s\n", s.F_name)
			fmt.Fprintf(os.Stderr, ">>>>fname ::%d\n", len(s.F_name))
			fmt.Println(ct_header, "This User Is not Allowed!")
		}
	}
}
