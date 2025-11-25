package main

import (
    "fmt"
    "os"
)


// class {

// };

func main() {
    const BS = 1024
    buf := make([]byte, BS)
    fmt.Println("Content-Type: text/html\n\n")
    for {
        n, err := os.Stdin.Read(buf)
        if n > 0 {
            fmt.Print(string(buf[:n]))
        }

        if err != nil {
            // EOF or error
            break
        }

        if n != BS {
            // short read → same logic as your C code
            break
        }
    }
}

