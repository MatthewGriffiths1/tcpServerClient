/*
 * File: server.cpp
 * A socket stream server
 */

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

using namespace std;

#define PORT "3490" // Port users will connect to
#define BACKLOG 10 // Number of pending connections queue will hold


int main(int argc, char* argv[]) {
	int sockfd, new_fd, status;
	socklen_t addr_size;
	struct sockaddr_storage their_addr;
	struct addrinfo hints;
	struct addrinfo *servinfo, *p; // will point to results 
	int yes = 1;
	char msg[150];
		
	memset(&hints, 0, sizeof hints); // struct must be empty
	hints.ai_family = AF_UNSPEC; // AF_UNSPEC indicates that getaddrinfo should 
				     // return socket adresses for any family (ipv4 or ipv6)
	hints.ai_socktype = SOCK_STREAM; // preferred socket type
	hints.ai_flags = AI_PASSIVE; // Using returned socket address structure in a call to
				     // bind function
	if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		cerr << "getaddrinfo error: " << gai_strerror(status) << endl;
		return 1;
	}
	
	// do everything until servinfo no longer needed
	// should do error handling and walk servinfo

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
			cerr << "server: socket" << endl;
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			cerr << "server: setsockopt" << endl;
			return 1;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			cerr << "server: bind" << endl;
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // No longer require this structure

	if (p == NULL) {
		cerr << "server: failed to bind" << endl;
		return 1;
	}

	if (listen(sockfd, BACKLOG) == -1) {
		cerr << "server: listen" << endl;
		return 1;
	}	

	// could reap all dead processes here??
	
	cout << "server: waiting for connections..." << endl;

	while (1) {
		addr_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		if (new_fd == -1) {
			cerr << "server: accept" << endl;
			continue;
		}

		cout << "Connecting" << endl;
		
		// Fork process to handle multiple clients -- Dont want this for now
//		if (!fork()) { // this is the child process
//			close(sockfd); // child doesnt need the listener
//			if (send(new_fd, "Hello, world!", 13, 0) ==-1) {
//				cerr << "server: send" << endl;
//			}
//			close(new_fd);
//			return 0;
//		}
		
		while(1) {
			// recieve 
			memset(&msg, 0, sizeof(msg));
			if (recv(new_fd, (char*)&msg, sizeof(msg), 0) == -1) {
				cerr << "server: error recieving message" << endl;
				close(new_fd);
				return 1;
			}
			
			// output clients message
			cout << "Client: " << msg << endl;
			
			// server input
			string data;
			cout << ">";
			getline(cin, data);
			memset(&msg, 0, sizeof(msg));
			strcpy(msg, data.c_str());
			send(new_fd, (char*)&msg, strlen(msg), 0);
		}
		close(new_fd); // parent doesnt need this
	}

	return 0;
}


