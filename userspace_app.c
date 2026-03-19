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
#define numGestures 8

static int cmd_fd;

static int now_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int)(ts.tv_sec *100 + ts.tv_nsec /1000000);
}

static int classify_gesture(LEAP_HAND *hand, LEAP_HAND *prev){
    float grab = hand -> grab_strength;
    float pinch = hand ->pinch_strength;

    if(pinch > 0.7f){
        return LEAP_GESTURE_SWIPE_RIGHT;
    }
    if(grab > 0.7f){
        return LEAP_GESTURE_GRAB;
    }
    if(grab < 0.15f && pinch < 0.15f){
        return LEAP_GESTURE_OPEN;
    }

    if(prev){
        float handx = hand -> palm.velocity.x;
        float handy = hand -> palm.velocity.y;

        if(handx > 200.0f){
            return LEAP_GESTURE_SWIPE_RIGHT;
        }
        if(handx < -200.0f){
            return LEAP_GESTURE_SWIPE_LEFT;
        }
        if(handy > 200.0f){
            return LEAP_GESTURE_SWIPE_UP;
        }
         if(handy < -200.0f){
            return LEAP_GESTURE_SWIPE_DOWN;
        }

        

    }

    return -1;

}


static void *reader_thread(void *arg);
{
    LEAP_CONNECTION conn;
    LEAP_CONNECTION_CONFIG cfg = {0};
    cfg.size() = sizeof(cfg);

    if (LeapCreateConnection(&cfg,&conn) != eLeapRS_Success) {
        fprintf(stderr, "[reader] LeapCreateConnection failed\n");
        return NULL;
    }
    LeapOpenConnection(conn);
    printf("[reader] LeapC Connection opened, waiting for device...\n");

    while(1){
        LEAP_CONNECTION_MESSAGE msg;
        if (LeapPollConnection(conn, 1000, &msg) != eLeapRS_Success)
            continue;
        if (msg.type == eLeapEventType_Device){
            printf("[reader] Device ready\n");
            break;
        }
    }
}

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