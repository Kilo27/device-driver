#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

int main(void){

    pthread_t reader_tid;
    pthread_t dispatcher_tid;

    cmd_fd = open(CMD_DEVICE, O_RDWR);
        if(cmd_fd <0){
            perror("open", CMD_DEVICE);
            fprintf(stderr, "The module has not been loaded");
            return 1;
        }

    
    printf("UserSpace app started\n");
    return 0;


}