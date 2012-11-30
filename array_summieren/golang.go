package main

import (
	"fmt"
	"log"
	"os"
	"strconv"
	"time"
)

const (
	NumProcs = 2
	N        = 256 * 1024 * 1024
)

func main() {

	numprocs := NumProcs
	n := N
	var err error

	switch len(os.Args) {
	case 3:
		n, err = strconv.Atoi(os.Args[2])
		if err != nil {
			log.Fatalln(err)
		}
		fallthrough
	case 2:
		numprocs, err = strconv.Atoi(os.Args[1])
		if err != nil {
			log.Fatalln(err)
		}
	}

	sl := make([]int, n)
	for i := range sl {
		sl[i] = i + 1
	}

	fmt.Println("Procs:", numprocs, "N =", n)

	t := time.Now()

	c := make(chan int64)

	for i := 0; i < numprocs; i++ {
		first := i * (n / numprocs)
		last := (i + 1) * (n / numprocs)

		go func() {
			slice := sl[first:last]
			sum := int64(0)
			for _, v := range slice {
				sum += int64(v)
			}
			c <- sum
		}()
	}

	result := int64(0)
	for i := 0; i < numprocs; i++ {
		result += <-c
	}

	d := time.Since(t)

	fmt.Println("result:", result)
	fmt.Println("took", d)

}
