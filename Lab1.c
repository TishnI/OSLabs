#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>


typedef struct 
{
    int ready;
    pthread_cond_t cond;
    pthread_mutex_t lock;

}MonitorData;


void* Deliver(void* threadData)
{
    MonitorData *data = (MonitorData*)threadData;
    while(true)
    {
        pthread_mutex_lock(&(data->lock));
        if(data->ready == 1)
        {
            pthread_mutex_unlock(&(data->lock));
            continue;
        }
        data->ready = 1;
        printf("provided\n");
        pthread_cond_signal(&(data->cond));
        pthread_mutex_unlock(&(data->lock));
        //sleep(1);
    }

}

void* Get(void* threadData)
{
    MonitorData* data = (MonitorData*) threadData;
    while(true)
    {
        pthread_mutex_lock(&(data->lock));
        while(data->ready == 0)
        {
            pthread_cond_wait(&(data->cond), &(data->lock));
            printf("awoke\n");
        }
        data->ready = 0;
        printf("consumed\n");
        pthread_mutex_unlock(&(data->lock));
        //sleep(1);
    }
}

int main(int argc, char *argv[])
{
    MonitorData* data = (MonitorData*)malloc(sizeof(MonitorData));
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    data->cond = cond;
    data->lock = lock;
    data->ready = 0;

    pthread_t supplierThread;
    pthread_t consumerThread;

    pthread_create(&supplierThread, NULL, Deliver, data);
    pthread_create(&consumerThread, NULL, Get, data);

    pthread_join(supplierThread,NULL);
    pthread_join(consumerThread, NULL);

    free(data);


    return 0;
}