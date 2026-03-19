#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

struct leap_event{
    int time;
    int gesture;
    int hand;
    int x;
    int y;

};

static const char *gesture_commands[] = {"lsusb","lspci","lsblk", "df -h", "free -h", "uptime", "uname -a", "ip link show"};

int main(void){

    pthread_t reader_tid;
    pthread_t dispatcher_tid;

    cmd_fd = open(CMD_DEVICE, O_RDWR);
        if(cmd_fd <0){
            perror("open", CMD_DEVICE);
            fprintf(stderr, "The module has not been loaded\n");
            return 1;
        }

        
    printf("UserSpace app started\n");

    pthread_create(&reader_tid, NULL, reader_thread, NULL);
    pthread_create(&dispatcher_tid, NULLm dispatcher_thread, NULL);

    pthread_join(reader_tid, NULL);
    pthread_join(dispatcher_tid, NULL);

    close(cmd_fd);
    return 0;


}