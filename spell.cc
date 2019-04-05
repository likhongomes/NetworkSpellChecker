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
#include <sstream>
#include <vector>
#include <queue>
#include <mutex>
#include "simpleServer.h"

#define NUM_WORKER 4
#define DEFAULT_DICTIONARY "/usr/share/dict/words"
#define BUF_LEN 1024


/*
Likhon D. Gomes
CIS 3207
Prof Eugene Kwatney
April 2nd, 2019
*/

using namespace std;

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

//Global Variables
vector<string> words;
queue<int> sockets;
queue<string> logQ;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mLog = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;;
pthread_cond_t cLog = PTHREAD_COND_INITIALIZER;;
int open_listenfd(int);
#endif


//function headers
string compare(string str1, vector<string> words);
void *getConnections(void *arg);
void *worker(void *arg);
void *writeLog(void *args);


int main(int argc, char* argv[]){

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

    

    
    //Does all the hard work for us.
    //cout << "Starting spell check server\n" << endl;
	connectionSocket = open_listenfd(connectionPort);
	if(connectionSocket == -1){
		printf("Could not connect to %s, maybe try another port number?\n", argv[1]);
		return -1;
	}

    pthread_t threadID[NUM_WORKER];
    for(int i = 0; i<NUM_WORKER; i++){
        pthread_create(&threadID[i], NULL, worker, (void *)i);
    }

    pthread_t logThread;
    pthread_create(&logThread, NULL, writeLog, (void *)connectionSocket);

    //Starting the tread connectionGetter that accepts connections
    pthread_t connectionGetter;
    pthread_create(&connectionGetter, NULL, getConnections, (void *)connectionSocket);
	pthread_join(connectionGetter, NULL);
    
    /* //testing the vector
    for(int x = 0; x<words.size(); x++){
        cout << words[x] << endl;
    }*/
    return 0;
}




void *getConnections(void *arg){
    
    while(true){
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
        

        pthread_mutex_lock(&m);
        sockets.push(clientSocket);
        pthread_cond_signal(&c);
        pthread_mutex_unlock(&m);
        
        cout << endl;
        
    }
    return NULL;
}


void *worker(void *arg){
    int threadID = (long)arg;
    int clientSocket;

    while(1){
        if(sockets.empty()){
            pthread_mutex_lock(&m);

            pthread_cond_wait(&c, &m);
            pthread_mutex_unlock(&m);
        }        
        while(!sockets.empty()){
            
            pthread_mutex_lock(&m);
            clientSocket = sockets.front();
            sockets.pop();
            cout << "Thread " << threadID << " Servicing clientSocket " << clientSocket << endl; 
            pthread_mutex_unlock(&m);

            while(true){
                send(clientSocket, msgPrompt, strlen(msgPrompt), 0);
                //recv() will store the message from the user in the buffer, returning
                //how many bytes we received.
                bytesReturned = recv(clientSocket, recvBuffer, BUF_LEN, 0);
                //Check if we got a message, send a message back or quit if the
                //user specified it.
                if(bytesReturned == -1){
                    send(clientSocket, msgError, strlen(msgError), 0);
                } else if(recvBuffer[0] == 27){//'27' is the escape key.
                    send(clientSocket, msgClose, strlen(msgClose), 0);
                    close(clientSocket);
                    break;
                } else {
                    string comparator(recvBuffer);
                    string temp = "";
                    temp = compare(comparator, words);

                    pthread_mutex_lock(&mLog);
                    logQ.push(temp);
                    pthread_cond_signal(&cLog);
                    pthread_mutex_unlock(&mLog);

                    char result[temp.length()];
                    strcpy(result,temp.c_str());
                    send(clientSocket, result, strlen(result), 0);
		        }
            }
            cout << "Thread " << threadID << " finished serving "<< "Client Socket "<<clientSocket << endl;
            close(clientSocket);
        }

	}
}


void *writeLog(void *args){

    ofstream file;
    file.open("log.txt",ios_base::app);

    while(true){
        pthread_mutex_lock(&mLog);
        while(logQ.empty()){
            pthread_cond_wait(&cLog, &mLog); 
        } 
        string data = logQ.front();
        logQ.pop();
        file << data << endl;
        pthread_mutex_unlock(&mLog);
    }
    
}


string compare(string str1, vector<string> words){

    string str = "";
    for(int i = 0; i<str1.size(); i++){
        if(isalpha(str1[i]))
            str += str1[i];
    }
    string ret = str + " Misspelled\n";
    for(int i = 0; i<words.size();i++){
        if(str.compare(words[i]) == 0){
            ret = str + " OK\n";
            break;
        }
    }
    return ret;
}

