/*
 * File: client.cpp
 * TCP client 
 */

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

#define PORT "3490" // Port users will connect to
#define MAXDATASIZE 150 // max number of bytes we can get at once

int main(int argc, char* argv[]) {
	int status, sockfd, numbytes;
	char buf[MAXDATASIZE];
	char s[INET6_ADDRSTRLEN];
	struct addrinfo hints, *servinfo, *p;

	if (argc != 2) {
		cout << "usage: ./client hostname" << endl;
		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// get ready to connect
	if ((status = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		cerr << "getaddrinfo: " << gai_strerror(status) << endl;
	}	

	// Loop through all results and connect to first
	for (p = servinfo; p != NULL; p = p->ai_next) {
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1) {
			cerr << "client: socket" << endl;
			continue;
		}

		status = connect(sockfd, p->ai_addr, p->ai_addrlen);
		if (status == -1) {
			close(sockfd);
			cerr << "client: connect" << endl;
			continue;
		}
		break;
	}

	if (p == NULL) {
		cerr << "client: failed to connect" << endl;
		return 2;
	}

	// inet_ntop to get network address data??
	// cout << "client: connecting to " << s
	
	freeaddrinfo(servinfo); // all done with this structure

	while(1) {
		cout << ">";
		string data;
		getline(cin, data);
		memset(&buf, 0, sizeof(buf));
		strcpy(buf, data.c_str());
		send(sockfd, (char*)&buf, strlen(buf), 0);

		// recieve message
		memset(&buf, 0, sizeof(buf));
		recv(sockfd, buf, sizeof(buf), 0);
		cout << "server: " << buf << endl;


	}	
//	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
//		cerr << "rcv" << endl;
//		return 1;
//	}
//
//	buf[numbytes] = '\0'; // Null terminate char array to mark end of textual data
//	
//	cout << "client: recieved " << buf << endl;
	
	close(sockfd);
	
	return 0;	
}
