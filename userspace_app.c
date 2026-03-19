#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <LeapC.h>
 
#define CMD_DEVICE "/dev/leapcmd"
 
#define LEAP_GESTURE_SWIPE_LEFT  0
#define LEAP_GESTURE_SWIPE_RIGHT 1
#define LEAP_GESTURE_SWIPE_UP    2
#define LEAP_GESTURE_SWIPE_DOWN  3
#define LEAP_GESTURE_PINCH       4
#define LEAP_GESTURE_GRAB        5
#define LEAP_GESTURE_OPEN        6
#define LEAP_GESTURE_CIRCLE      7
 
struct leap_event {
    uint32_t time;
    uint8_t  gesture;
    uint8_t  hand;
    int16_t  x;
    int16_t  y;
};
 
static const char *gesture_commands[] = {
    "lsusb",
    "lspci",
    "lsblk",
    "df -h",
    "free -h",
    "uptime",
    "uname -a",
    "ip link show",
};
 
static const char *gesture_names[] = {
    "Swipe Left", "Swipe Right", "Swipe Up", "Swipe Down",
    "Pinch", "Grab", "Open Hand", "Circle"
};
 
#define NUM_GESTURES 8
 
static int cmd_fd;
 
static uint32_t now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
 
static int classify_gesture(LEAP_HAND *hand, LEAP_HAND *prev)
{
    float grab  = hand->grab_strength;
    float pinch = hand->pinch_strength;
 
    if (pinch > 0.7f) return LEAP_GESTURE_PINCH;
    if (grab  > 0.7f) return LEAP_GESTURE_GRAB;
 
    if (prev) {
        float vx = hand->palm.velocity.x;
        float vy = hand->palm.velocity.y;
        if (vx >  200.0f) return LEAP_GESTURE_SWIPE_RIGHT;
        if (vx < -200.0f) return LEAP_GESTURE_SWIPE_LEFT;
        if (vy >  200.0f) return LEAP_GESTURE_SWIPE_UP;
        if (vy < -200.0f) return LEAP_GESTURE_SWIPE_DOWN;
    }
 
    return -1;
}
 
static void *reader_thread(void *arg)
{
    LEAP_CONNECTION conn;
    LEAP_CONNECTION_CONFIG cfg = {0};
    cfg.size = sizeof(cfg);
 
    if (LeapCreateConnection(&cfg, &conn) != eLeapRS_Success) {
        fprintf(stderr, "[reader] LeapCreateConnection failed\n");
        return NULL;
    }
 
    LeapOpenConnection(conn);
    printf("[reader] Waiting for device...\n");
 
    while (1) {
        LEAP_CONNECTION_MESSAGE msg;
        if (LeapPollConnection(conn, 1000, &msg) != eLeapRS_Success)
            continue;
        if (msg.type == eLeapEventType_Device) {
            printf("[reader] Device ready\n");
            break;
        }
    }
 
    LEAP_HAND prev_hands[2]   = {0};
    int       prev_valid[2]   = {0};
    int       last_gesture[2] = {-1, -1};
    uint32_t  last_time[2]    = {0, 0};
 
    while (1) {
        LEAP_CONNECTION_MESSAGE msg;
        if (LeapPollConnection(conn, 100, &msg) != eLeapRS_Success)
            continue;
        if (msg.type != eLeapEventType_Tracking)
            continue;
 
        const LEAP_TRACKING_EVENT *frame = msg.tracking_event;
 
        for (uint32_t i = 0; i < frame->nHands; i++) {
            LEAP_HAND *h    = &frame->pHands[i];
            int        side = (h->type == eLeapHandType_Right) ? 1 : 0;
            LEAP_HAND *prev = prev_valid[side] ? &prev_hands[side] : NULL;
 
            int g = classify_gesture(h, prev);
 
            if (g < 0) {
                last_gesture[side] = -1;
                goto next;
            }
 
            uint32_t now = now_ms();
            if (g == last_gesture[side] && (now - last_time[side]) < 500)
                goto next;
 
            struct leap_event evt = {
                .time    = now_ms(),
                .gesture = (uint8_t)g,
                .hand    = (uint8_t)side,
                .x       = (int16_t)h->palm.position.x,
                .y       = (int16_t)h->palm.position.y,
            };
            write(cmd_fd, &evt, sizeof(evt));
            last_gesture[side] = g;
            last_time[side]    = now;
 
next:
            prev_hands[side] = *h;
            prev_valid[side] = 1;
        }
    }
 
    LeapCloseConnection(conn);
    LeapDestroyConnection(conn);
    return NULL;
}
 
static void *dispatcher_thread(void *arg)
{
    struct leap_event evt;
 
    printf("[dispatcher] Waiting for gestures...\n");
 
    while (1) {
        if (read(cmd_fd, &evt, sizeof(evt)) != sizeof(evt))
            continue;
 
        if (evt.gesture < NUM_GESTURES) {
            printf("\n[%s | %s hand | palm (%d,%d)] Running: %s\n",
                   gesture_names[evt.gesture],
                   evt.hand ? "right" : "left",
                   evt.x, evt.y,
                   gesture_commands[evt.gesture]);
            fflush(stdout);
            system(gesture_commands[evt.gesture]);
        }
 
        fflush(stdout);
    }
 
    return NULL;
}
 
int main(void)
{
    pthread_t reader_tid, dispatcher_tid;
 
    cmd_fd = open(CMD_DEVICE, O_RDWR);
    if (cmd_fd < 0) {
        perror("open " CMD_DEVICE);
        fprintf(stderr, "Is the module loaded? Run: sudo insmod leap.ko\n");
        return 1;
    }
 
    printf("Leap Motion command app started.\n");
    printf("Gestures: swipe L/R/U/D, pinch, grab, open, circle\n\n");
 
    pthread_create(&reader_tid,     NULL, reader_thread,     NULL);
    pthread_create(&dispatcher_tid, NULL, dispatcher_thread, NULL);
 
    pthread_join(reader_tid,     NULL);
    pthread_join(dispatcher_tid, NULL);
 
    close(cmd_fd);
    return 0;
}
