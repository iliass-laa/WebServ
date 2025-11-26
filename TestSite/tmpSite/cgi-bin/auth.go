package main



import(
	"fmt"
	//"os"
	//"strings"
)

type Auth struct{
	F_name, L_name string
	Age int
}

func main(){
	var alo Auth

	alo.F_name = "said"
	alo.L_name = "takhamat"
	alo.Age = 45

	fmt.Println(alo.F_name, alo.L_name, alo.Age)

}
