// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"time"
	"strings"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type user struct {
	name 	string
	ip 		string
	admin 	bool
	ch 		chan string
	conn 	net.Conn
	online	bool
	logged	string
}

var (
	entering = make(chan string)
	leaving  = make(chan string)
	messages = make(chan string) // all incoming client messages
	users    = make(map[string]*user)
	admin	 = false
)


//	Return all user currently connected
func getAllUsers() string {
	var allUsers string
	for usr := range users {
		allUsers += "irc-server > " + users[usr].name + " - connected since " + users[usr].logged + "\n"
	}

	return allUsers
}


//	Send a private message
func sendMessage(sendTo string, fromUser *user, message string){
	sendToUser := users[sendTo];
	if sendToUser != nil {
		//	Check if the user exists
		sendToUser.ch <- fromUser.name  + " (private) > " + message
	} else {
		fromUser.ch <- "irc-server > ERROR: User doesn't exists"
	}
}


//	Return the local time of the server
func getLocalTime() string {
	t := time.Now()
	return "irc-server > Local Time: " + t.Location().String() + " " + t.Format("15:04")
}

//	Return the info of a user
func getUser(name string) string {
	
	if val, ok := users[name]; ok{
		return "irc-server > username: " + val.name + ", IP: " + val.ip + " Connected since : " + val.logged 
	} else{
		return "irc-server > User doesn't exists"
	}

}

//	Kick a user
func kickUser(name string, incomeUser *user) {
	if incomeUser.admin {
		//	Check if the user is admin

		userToKick := users[name];
		
		if userToKick != nil {
			//	Check if user to kick exists
			
			if userToKick != incomeUser{
				//	Check if user to kick it's not itself
				
				// Notify the user that was kicked
				userToKick.ch <- "irc-server > You're kicked from this channel"

				// Notify on the server that the user was kicked
				fmt.Println("irc-server > [" + userToKick.name +"] was kicked")

				// Make the user leave the channel
				leaving <- userToKick.name

				// Notify to all users that the user was kicked
				messages <- "irc-server > [" + name + "] was kicked from channel"

			} else {
				incomeUser.ch <- "irc-server > ERROR: You cannot kick yourself"
			}
		} else {
			incomeUser.ch <- "irc-server > ERROR: User to kick doesn't exists"
		}	
	} else {
		incomeUser.ch <- "irc-server > ERROR: Only the ADMIN can kick users"
	}
}

func deleteUser(name string){
	usr := users[name]
	close(usr.ch)

	delete(users, usr.name)

	if usr.admin && len(users) > 0 {
		//	Check if the user is admin and see if there are more users
		for cli := range users {
			users[cli].admin = true
			users[cli].ch <- "irc-server > You're the new IRC Server ADMIN"
			fmt.Println("irc-server > [" + users[cli].name +"] was promoted as the channel ADMIN")
			break
		}
	} else if usr.admin{
		admin = false
	}
}

func broadcaster() {
	//clients := make(map[client]bool) // all connected clients
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range users {
				users[cli].ch <- msg
			}

		case cli := <-entering:
			users[cli].online = true

		case cli := <-leaving:
			deleteUser(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)

	who := conn.RemoteAddr().String()
	input := bufio.NewScanner(conn)

	var incomeUser = user{}

	input.Scan()
	name := input.Text()

	//	Check if user exists
	if _,ok := users[name]; ok{
		ch <- "ERROR: Username already exist"
		conn.Close()
		return
	} else {
		//	Create new user
		incomeUser = user {
			name: 	name,
			ip: 	who,
			admin: 	!admin,
			ch: 	ch,
			conn: 	conn,
			online:	true,
			logged:	time.Now().Format("2006-01-02 15:04:05"),
		}

		users[name] = &incomeUser
	}

	//	Welcome message to new user
	ch <- "irc-server > Welcome to the Simple IRC Server \nirc-server > Your user [" + incomeUser.name + "] is successfully logged"

	//	New login alert for everyone
	fmt.Println("irc-server > New connected user [" + incomeUser.name + "]")
	messages <- "irc-server > New connected user [" + incomeUser.name + "]"

	//	Save new channel
	entering <- name

	//	Verify is there is no admin
	if !admin{
		ch <- "irc-server > Congrats, you were the firts user."
		ch <- "irc-server > You're the new IRC Server ADMIN"
		fmt.Println("irc-server > [" + incomeUser.name +"] was promoted as the channel ADMIN")
		admin = true
	}

	//	Check the text that the user send	
	for input.Scan() {
		command := strings.Split(input.Text(), " ")
		switch command[0]{

		// Command /users
		case "/users":
			ch <- getAllUsers()

		// Command /msg
		case "/msg":
			if len(command) < 3 {
				//	Check syntax of the command
				ch <- "irc-server > Invalid format: /msg <user> <msg>"
			} else {
				//	Send the message
				to := command[1]
				message := strings.Join(command[2:], " ")
				sendMessage(to, &incomeUser, message)
				ch <- "irc-server > Message sended"
			}

		// Command /time
		case "/time":
			ch <- getLocalTime()

		// Command /user
		case "/user":
			if len(command) != 2 {
				//	Check syntax of the command
				ch <- "irc-server > Invalid format: /user <user>"
			} else {
				//	Get user info
				ch <- getUser(command[1])
			}

		// Command /kick
		case "/kick":
			if len(command) != 2 {
				//	Check syntax of the command
				ch <- "irc-server > Invalid format: /kick <user>"
			} else {
				// Kick the user 
				kickUser(command[1], &incomeUser)
			}

		// Default case message to everyone
		default:
			messages <- incomeUser.name + " > " + input.Text()	
		}
	}
	// NOTE: ignoring potential errors from input.Err()

	if _,ok := users[name]; ok{
		messages <- "irc-server > [" + incomeUser.name + "] left channel"
		fmt.Println("irc-server > [" + incomeUser.name + "] left")
		leaving <- incomeUser.name
		conn.Close()
	}
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {

	if len(os.Args) < 5 {
		fmt.Println("ERROR: Missing parameters")
		os.Exit(1)
	}

	var host, port string

	if os.Args[1] == "-host" {
		host = os.Args[2]
	} else{
		fmt.Println("ERROR: Missing flag -host")
		os.Exit(1)
	}

	if os.Args[3] == "-port" {
		port = os.Args[4]
	} else{
		fmt.Println("ERROR: Missing flag -port")
		os.Exit(1)
	}

	server := host + ":" + port

	listener, err := net.Listen("tcp", server)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("irc-server > Simple IRC Server started at %s\n", server)
	fmt.Println("irc-server > Ready for receiving new clients")

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
