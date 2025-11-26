package main

import (
	"fmt"
//	"maps"
)

func main(){
	m := make(map[int]string)

	m[0] = "alo"
	m[1] = "walo"

	//clear(m)
	v,boo := m[0]
	fmt.Println(v, boo)
}
