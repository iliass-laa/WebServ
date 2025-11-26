package main

import "fmt"


func main(){
	var nbrs [2]int
	//nbrs[0] = 1
	//nbrs[1] = 15
	nbrs =[2]int{1, 15}
	
	//{1, 55}
	fmt.Println(nbrs[0])
	fmt.Println(nbrs[1])

	a := nbrs[0:2]
	//a[2] = 78
	fmt.Println(a)
}
