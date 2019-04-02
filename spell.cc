#ifndef _SIMPLE_SERVER_H
#define _SIMPLE_SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
//#include <string.h>
#include <string>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "simpleServer.h"
#define WORKER_COUNT 4

#define BUF_LEN 1024
int open_listenfd(int);
#endif






#define DEFAULT_DICTIONARY "/usr/share/dict/words"

using namespace std;

//Global Variables
vector<string> words;
//int connectionSocket, clientSocket, bytesReturned, connectionPort;
//char* recvBuffer;

//function headers
string compare(string str1, vector<string> words);




int main(int argc, char* argv[]){

    struct sockaddr_in client;
	int clientLen = sizeof(client);
	int connectionPort;
	int connectionSocket, clientSocket, bytesReturned;
	char recvBuffer[BUF_LEN];
	recvBuffer[0] = '\0';


    //connectionPort = 3207; // default port number
    string dictionary = DEFAULT_DICTIONARY;

    //doing some house keeping over here, checking for port number and dictionary.
    if (argc == 1){
        cout << "No port number entered. Switching to default port 3207\n";
        connectionPort = 3207;
    } else {
        connectionPort = atoi(argv[1]);
    }

    if (argc < 3) {
        cout << "No dictionary name entered, switching to DEFAULT_DICTIONARY\n";
    } else{
        dictionary = argv[2];
    }

    //cout << "Please use the command \"telnet localhost " + (string)connectionPort + "\"to connect to the server.\n";

    //Reading the dictionary and loading it into the queue.
    std::ifstream file(DEFAULT_DICTIONARY);
    if (file.is_open()) {
    std::string line;

        //reading and populating data into the vector
        while (getline(file, line)) {
            words.push_back(line.c_str());
        }
        file.close();
    }



    //cout << compare(input, words);

    cout << "Starting spell check server\n" << endl;
    //Does all the hard work for us.
	connectionSocket = open_listenfd(connectionPort);
	if(connectionSocket == -1){
		printf("Could not connect to %s, maybe try another port number?\n", argv[1]);
		return -1;
	}

    if((clientSocket = accept(connectionSocket, (struct sockaddr*) &client, (socklen_t *) &clientLen)) == -1){
		printf("Error connecting to client.\n");
		return -1;
	}


    printf("Connection success!\n");
    
	char* clientMessage = "Status Code: 200\nSuccessfully connected to Spell Server\n";
	char* msgRequest = "Type in a spelling and click return key to get it checked by the server\n";
	char* msgResponse = "I actually don't have anything interesting to say...but I know you sent ";
	char* msgPrompt = ">>>";
	char* msgError = "I didn't get your message. ):\n";
	char* msgClose = "Goodbye!\n";


    //send()...sends a message.
	//We specify the socket we want to send, the message and it's length, the 
	//last parameter are flags.
	send(clientSocket, clientMessage, strlen(clientMessage), 0);
	send(clientSocket, msgRequest, strlen(msgRequest), 0);


    while(1){
		send(clientSocket, msgPrompt, strlen(msgPrompt), 0);
		//recv() will store the message from the user in the buffer, returning
		//how many bytes we received.
		bytesReturned = recv(clientSocket, recvBuffer, BUF_LEN, 0);

		//Check if we got a message, send a message back or quit if the
		//user specified it.
		if(bytesReturned == -1){
			send(clientSocket, msgError, strlen(msgError), 0);
		}
		//'27' is the escape key.
		else if(recvBuffer[0] == 27){
			send(clientSocket, msgClose, strlen(msgClose), 0);
			close(clientSocket);
			break;
		}
		else{
            string comparator(recvBuffer);
            string temp = compare(comparator, words);
            cout << temp << endl;
            char result[temp.length()];
            strcpy(result,temp.c_str());
            //cout << result;
            //cout << recvBuffer;
			//send(clientSocket, recvBuffer, strlen(recvBuffer), 0);
			send(clientSocket, result, strlen(result), 0);
		}
	}


    /* //testing the vector
    for(int x = 0; x<words.size(); x++){
        cout << words[x] << endl;
    }*/

    return 0;
}


string compare(string str1, vector<string> words){
    string str = "";
    for(int i = 0; i<str1.size(); i++){
        if(isalpha(str1[i]))
            str += str1[i];
    }

    string ret = str1 + " Misspelled\n";
    for(int i = 0; i<words.size();i++){
        if(str.compare(words[i]) == 0){
            ret = str1 + "  OK\n";
            break;
        }
    }
    return ret;
}
