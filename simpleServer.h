#ifndef _SIMPLE_SERVER_H
#define _SIMPLE_SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <condition_variable>
#include <chrono>
#define WORKER_COUNT 4

#define BUF_LEN 1024
int open_listenfd(int);
#endif

