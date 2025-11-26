package main

import (
	"os"
	"fmt"
	"strings"
)

func main (){
	fmt.Println("Content-Type: text/html\n\n")
	value , ok := os.LookupEnv("HTTP_COOKIE")
	if ok {
		fmt.Println("This User is already Loged In\n")
	}else {
		fmt.Println("Need to LogIn First !")
	}
	if strings.Index(value, "name=said") == 0{
		fmt.Println("Good Evenning Said")
	}else{
		fmt.Println("Good Mornning New User")
	}
}
