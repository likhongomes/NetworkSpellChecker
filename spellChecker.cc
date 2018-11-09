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


int main(int argc, char* argv[]){

    vector<string> words;

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



/*

    pthread_t threadPool[WORKER_COUNT];
    int threadIds[WORKER_COUNT];

    cout << "launching threads" << endl;

*/





    //sockaddr_in holds information about the user connection.
    //We don't need it, but it needs to be passed into accept().
    struct sockaddr_in client;
    int clientLen = sizeof(client);
    int connectionPort = atoi(argv[1]);
    int connectionSocket, clientSocket, bytesReturned;
    char recvBuffer[BUF_LEN];
    recvBuffer[0] = '\0';

    connectionPort = atoi(argv[1]);

    //We can't use ports below 1024 and ports above 65535 don't exist.
    if (connectionPort < 1024 || connectionPort > 65535)
    {
        printf("Port number is either too low(below 1024), or too high(above 65535).\n");
        return -1;
    }

    //Does all the hard work for us.
    connectionSocket = open_listenfd(connectionPort);
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
    if ((clientSocket = accept(connectionSocket, (struct sockaddr *)&client, (socklen_t*)&clientLen)) == -1)
    {
        printf("Error connecting to client.\n");
        return -1;
    }

    printf("Connection success!\n");
    char *clientMessage = "Hello! I hope you can see this.\n";
    char *msgRequest = "Send me some text and I'll respond with something interesting!\nSend the escape key to close the connection.\n";
    char *msgResponse = "Ret: ";
    char *msgPrompt = ">>>";
    char *msgError = "I didn't get your message. ):\n";
    char *msgClose = "Goodbye!\n";

    //send()...sends a message.
    //We specify the socket we want to send, the message and it's length, the
    //last parameter are flags.
    send(clientSocket, clientMessage, strlen(clientMessage), 0);
    send(clientSocket, msgRequest, strlen(msgRequest), 0);

    //Begin sending and receiving messages.
    while (1)
    {
        send(clientSocket, msgPrompt, strlen(msgPrompt), 0);
        //recv() will store the message from the user in the buffer, returning
        //how many bytes we received.
        bytesReturned = recv(clientSocket, recvBuffer, BUF_LEN, 0);

        //Check if we got a message, send a message back or quit if the
        //user specified it.
        if (bytesReturned == -1)
        {
            send(clientSocket, msgError, strlen(msgError), 0);
        }
        //'27' is the escape key.
        else if (recvBuffer[0] == 27)
        {
            send(clientSocket, msgClose, strlen(msgClose), 0);
            close(clientSocket);
            break;
        }
        else
        {

            


            send(clientSocket, msgResponse, strlen(msgResponse), 0);
            string str(recvBuffer);
            cout << recvBuffer << endl;
            string result = compare(str, words);
            char s[100];
            strcpy(s, result.c_str());
            send(clientSocket, s, bytesReturned, 0);
        }
    }

    } else {
        cout << "Dictionary not found" << endl;
    }

    
}
