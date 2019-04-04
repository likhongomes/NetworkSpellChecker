#ifndef _SIMPLE_SERVER_H
#define _SIMPLE_SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
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
#include <queue>
#include <mutex>
#include "simpleServer.h"

#define NUM_WORKER 4
#define DEFAULT_DICTIONARY "/usr/share/dict/words"
#define BUF_LEN 1024

bool debug = false;
/*
Likhon D. Gomes
CIS 3207
Prof Eugene Kwatney
April 2nd, 2019
*/

struct sockaddr_in client;
int clientLen = sizeof(client);
int connectionPort;
int connectionSocket, bytesReturned;
char recvBuffer[BUF_LEN];
//recvBuffer[0] = '\0';

//Messages
char* clientMessage = "Status Code: 200\nSuccessfully connected to Spell Server\n";
char* msgRequest = "Type in a spelling and click return key to get it checked by the server\n";
char* msgResponse = "I actually don't have anything interesting to say...but I know you sent ";
char* msgPrompt = ">>>";
char* msgError = "I didn't get your message. ):\n";
char* msgClose = "Goodbye!\n";



int open_listenfd(int);
#endif


int clientSock;

using namespace std;

//Global Variables
vector<string> words;
queue<int> sockets;
mutex mtx;


//function headers
string compare(string str1, vector<string> words);
void *getConnections(void *arg);
void *worker(void *arg);


int main(int argc, char* argv[]){

    


    pthread_t connectionGetter;
    


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

    /*
    pthread_t threadID[NUM_WORKER];
        for(int i = 0; i<NUM_WORKER; i++){
            pthread_create(&threadID[i], NULL, worker, (void *)i);
        }*/

    //Starting the tread connectionGetter that accepts connections
    pthread_create(&connectionGetter, NULL, getConnections, (void *)connectionSocket);
    
	pthread_join(connectionGetter, NULL);
    
    


    


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

    string ret = "Misspelled\n";
    for(int i = 0; i<words.size();i++){
        if(str.compare(words[i]) == 0){
            ret = "OK\n";
            break;
        }
    }
    return ret;
}


void *getConnections(void *arg){


    
    while(true){
        if(debug)cout << "Getting Connections" << endl;
        int clientSocket;
        int connectionSocket = (long)arg;
        if((clientSocket = accept(connectionSocket, (struct sockaddr*) &client, (socklen_t *) &clientLen)) == -1){
            printf("Error connecting to client.\n");
        }
        printf("Connection success!\n");

        //send()...sends a message.
        //We specify the socket we want to send, the message and it's length, the 
        //last parameter are flags.
        send(clientSocket, clientMessage, strlen(clientMessage), 0);
        send(clientSocket, msgRequest, strlen(msgRequest), 0);
        sockets.push(clientSocket);
        cout << endl;

        pthread_t thread;
        int i = 0;
        pthread_create(&thread, NULL, worker, (void *)i);
        
    }

    


    return NULL;
}


void *worker(void *arg){
    int threadID = (long)arg;
    //cout << "Creating Thread " << threadID << endl;
    int clientSocket;
    while(1){
        //cout << threadID << " " << clientSocket << endl;

        
        while(!sockets.empty()){
            
            mtx.lock();

            if(!sockets.empty()){
                cout << "Q Size "<<sockets.size() << endl;
                clientSocket = sockets.front();
                sockets.pop();
            }
            mtx.unlock();
            //cout << "Q Size "<<sockets.size() << endl;
            cout << "Thread " << threadID << " Servicing clientSocket " << clientSocket << endl; 

            while(true){
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

            string temp = "";
            temp = compare(comparator, words);
            //cout << "You have entered " <<temp << endl;
            char result[temp.length()];
            strcpy(result,temp.c_str());
            //cout << result;
            //cout << recvBuffer;
			//send(clientSocket, recvBuffer, strlen(recvBuffer), 0);
			send(clientSocket, result, strlen(result), 0);
		}
            }
            cout << "Code broke " << threadID << endl;
        }

        
        
	}
    cout << "done execution client socket " << clientSocket << endl;
}
