#include <iostream>
#include <pthread.h>

using namespace std;


void *b(void *args){
    long x = (long)args;
    for(int x = 0; x<100; x++)
        cout << "OOOOO" << endl;
    
    pthread_exit(NULL);
}

int main(){

    int x = 0;
    pthread_t a;


        pthread_create(&a, NULL, b, (void *)x);

    for(int x = 0; x<100; x++)
        cout << "XXXXX" << endl;
    
    
    //pthread_join(&a, NULL);
    //cout << "Hello World" << endl;
    pthread_exit(NULL);
 }