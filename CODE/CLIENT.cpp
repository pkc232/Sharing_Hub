/****


This is the CLIENT implementation, i.e. the code which 
would be run on the student's system.

Functionalities to be incorporated:
1.	Establish the connection with the server
2.	Send the cpp program file which needs to be checked.
3.	Wait for the response from the server regarding the result 
	of the submission

@author: PKC


****/


/*include all the necessary libraries*/
#include "DEFINITIONS.h"
#include "CONSTANTS.h"

using namespace std;


void set_reuse_addr(int sockfd){
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	cout<<"setsockopt(SO_REUSEADDR) failed"<<endl;
}


int establishConnection(string IPServ, int port_no){
	/**
		
		This method is used to establish a TCP connection between the server and client.
		

		Params:
		@IPServ:	The IP of the server in the form of string.
		@port_no:	An integer indicating the port number on which the service is
					to be provided.
		Return:
		sfd: 		The integer socket file descriptor through which the exchanges 
					will take place.
	**/
	
	int sfd;

	struct sockaddr_in serv_addr;

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_STREAM , 0)) == -1){
		//Socket failed to create.
		perror("\n socket");
		return -1;
	}

	//Socket created succesfully
	set_reuse_addr(sfd);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = inet_addr(IPServ.c_str());

	if(connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr)) == -1){
		//Connect failed to the server. 
		perror("\n connect : ");
	}

	//Connection successfully established.
	return sfd;
}

int sendMsg(int fd, char buf[BUFFER_SIZE]){
	/**
		This method is used to send a message 
		or a character bytestream on the sfd
		sent as parameter.
		
		Params:
		@fd:	The file descriptor on which the message has 
				to be sent.
		@buf:	The character buffer or message which needs to be sent.

		Include error checking.
	**/

	int er = write(fd, buf, BUFFER_SIZE);

	if(er == -1){
		//Some error occurred in writing the buffer.
		return -1; 
	}

	return 0;
}

int sendFile(int sfd, string fileName){
	/**
		This method is used to send a file.
		add error checking functionality.
	**/

	ifstream fin;
	fin.open(fileName.c_str());

	char buf[BUFFER_SIZE];
	bzero(buf, sizeof(buf));
	
	while(!fin.eof()){
		fin.read(buf, BUFFER_SIZE);
		sendMsg(sfd, buf);
		bzero(buf, sizeof(buf));
		sleep(0.1);
	}

	fin.close();
}

string recvMsg(int connfd){
	/*
		This method reads the buffer and returns it as a 
		string.

		Params:
		@connfd: The file descriptor on which the server and client communicates.

		Return:

	*/

	char buf[BUFFER_SIZE];

	bzero(buf, sizeof(buf)); // Very important for clearing the buffer. 
							 // Without this the buffer contains random garbage data
	int p = read(connfd,buf,BUFFER_SIZE);
	if(p<=0)
		return "ENDIT";
	string s(buf);
	return s;
}

void check(int sfd){
	// sendMsg(sfd, "Hello from client");
	string p = recvMsg(sfd);
	cout<<p<<endl;
	close(sfd);
}

int main(int argc, char const *argv[])
{
	string IPServ = "192.168.43.44";
	int port_no = 8080;

	int sfd = establishConnection(IPServ, port_no);

	
	

	string client_file_path = FILE_BASE_PATH;
	client_file_path += "CLIENT_FILES/";
	client_file_path += "sample.cpp";
	cout<<"Sending file\n";
	sendFile(sfd, client_file_path);
	sendMsg(sfd, "ENDIT");


	string msg = recvMsg(sfd);
	cout<<msg<<endl;
	close(sfd);
	return 0;
}