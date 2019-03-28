#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <string>
#include "simpleServer.h"
#include <pthread.h>

#define DEFAULT_DICTIONARY "words"

using namespace std;
//Globals
pthread_t threadPool[WORKER_COUNT];
int threadIDs[WORKER_COUNT], bytesReturned;
struct sockaddr_in client;
int clientLen = sizeof(client);
int connectionSocket,connectionSocket2, clientSocket;

char recvBuffer[BUF_LEN];
vector<string> words;
queue<int> sockets;
queue<string> log;
int threadId = 0;

sem_t lmutex;
sem_t pmutex;
sem_t cmutex;
sem_t wmutex;
condition_variable cv;



//Server Messages
char *clientMessage = "Hi, welcome to Leo's SpellChecker.\n";
char *msgRequest = "Send me a word and I'll respond with, whether the word is spelled correctly or not.\n";
char *msgResponse = "Ret: ";
char *msgPrompt = ">>>";
char *msgError = "I didn't get your message. ):\n";
char *msgClose = "Goodbye!\n";


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

void *threadFunction2(void *arg)
{

    while (!log.empty())
    {
        cout << log.front() << endl;
        log.pop();
    }
}

void *threadFunction(void *args){
    sem_wait(&wmutex);
    pthread_t threadId = (pthread_t)args;
    cout << "thread " << threadId <<"fired up thread "  << endl;
    int clientSocket;

    //sem_wait(wmutex);
    //pthread_cond_t_wait(&cv,wmutex);
    //cv.wait(wmutex);

    connectionSocket = sockets.front();
    sockets.pop();

    if ((clientSocket = accept(connectionSocket, (struct sockaddr *)&client, (socklen_t *)&clientLen)) == -1)
    {
        printf("Error connecting to client.\n");
        //break;
    }
    

    

    send(clientSocket, clientMessage, strlen(clientMessage), 0);
    send(clientSocket, msgRequest, strlen(msgRequest), 0);

    while (1){
        send(clientSocket, msgPrompt, strlen(msgPrompt), 0);
        //recv() will store the message from the user in the buffer, returning
        //how many bytes we received.
        bytesReturned = recv(clientSocket, recvBuffer, BUF_LEN, 0);

        //Check if we got a message, send a message back or quit if the
        //user specified it.
        if (bytesReturned == -1)
        {
            send(clientSocket, msgError, strlen(msgError), 0);
        } else if (recvBuffer[0] == 27) {//'27' is the escape key.

            send(clientSocket, msgClose, strlen(msgClose), 0);
            close(clientSocket);
            break;
        } else {





            send(clientSocket, msgResponse, strlen(msgResponse), 0);
            string str(recvBuffer);
            cout << recvBuffer << endl;
            string result = compare(str, words);
            log.push(result);
            char s[100];
            strcpy(s, result.c_str());
            cout << s << endl;
            send(clientSocket, s, 100, 0);
            pthread_t thread2Id;
            //pthread_create(thread2Id, NULL, threadFunction2,NULL);
            pthread_join(thread2Id, NULL);
        }
    }

    cout << "Thread function quitting" << endl;
    sem_post(&wmutex);

    while(!log.empty()){
        cout << log.front() << endl;
        log.pop();
    }

}



int main(int argc, char* argv[]){

    sem_init(&lmutex, 0, 1);
    sem_init(&pmutex, 0, 1);
    sem_init(&cmutex, 0, 1);
    sem_init(&wmutex, 0, WORKER_COUNT);

    std::ifstream file(DEFAULT_DICTIONARY);
    if (file.is_open()) {
    std::string line;

    //reading and populating data into the vector
    while (getline(file, line)) {

        words.push_back(line.c_str());

    }
    file.close();

    if (argc == 1)
    {
        printf("No port number entered.\n");
        return -1;
    }


    //sockaddr_in holds information about the user connection.
    //We don't need it, but it needs to be passed into accept().
    int connectionPort = atoi(argv[1]);
    recvBuffer[0] = '\0';

    connectionPort = atoi(argv[1]);

    //We can't use ports below 1024 and ports above 65535 don't exist.
    if (connectionPort < 1024 || connectionPort > 65535)
    {
        printf("Port number is either too low(below 1024), or too high(above 65535).\n");
        return -1;
    }


    while(1){
        //Does all the hard work for us.

        for (int i = 0; i < WORKER_COUNT; i++)
        {
            connectionSocket = open_listenfd(connectionPort+i);
            sockets.push(connectionSocket);
        }

        
        //connectionSocket2 = open_listenfd(connectionPort + 1);
        if (connectionSocket == -1)
        {
            printf("Could not connect to %s, maybe try another port number?\n", argv[1]);
            return -1;
    }

    //accept() waits until a user connects to the server, writing information about that server
    //into the sockaddr_in client.
    //If the connection is successful, we obtain A SECOND socket descriptor.
    //There are two socket descriptors being used now:
    //One by the server to listen for incoming connections.
    //The second that was just created that will be used to communicate with
    //the connected user.



    //send()...sends a message.
    //We specify the socket we want to send, the message and it's length, the
    //last parameter are flags.


    
    

    

    

    
    cv.notify_all();

    printf("Connection success!\n");

    //This is where all the threads are created.
    for(int i = 0; i < WORKER_COUNT; i++){
		threadIDs[i] = i;
        threadId = i;
		//Start running the threads.
		pthread_create(&threadPool[i], NULL, &threadFunction, &threadIDs[i]);
	}

    printf("All threads launched, waiting for them to quit.\n");
	for(int i = 0; i < WORKER_COUNT; i++){
		//Wait for all threads to finish executing.
		pthread_join(threadPool[i], NULL);
	}

    

    }




    //Begin sending and receiving messages.


    } else {
        cout << "Dictionary not found" << endl;
    }


}
