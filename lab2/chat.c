#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "chat.h"
#include "types.h"
#include "member_list.h"

#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"

static int fd;
static char buf[MSG_SIZE];
static char self_name[USERNAME_SIZE];
static struct sockaddr_in addr;
static bool running = true;
static long self_id = 0;
static unsigned long self_seq = 0;

void* listener(void *arg) {
    struct chat_packet packet;

    while (running) {
        receive_packet(&packet);

        switch (packet.type) {
            case MEMBER_ANNOUNCE : {
                if (packet.id != self_id) {
                    add_existing_member(&packet);
                    send_member_response();
                }
                break;
            }
            case MEMBER_RESPONSE : {
                if (packet.id != self_id && member_exists(packet.id)) {
                    add_existing_member(&packet);
                }
                break;
            }
            case MESSAGE : {
                if (strcmp(self_name, packet.name)) {
                    printf("%s: %s", packet.name, packet.message);
                }
                break;
            }
            case MEMBER_REMOVE : {
                delete_member(packet.id); //member left the chat
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    struct ip_mreq mreq;
    u_int yes=1;
    char* newline;

    memset(&packet, 0, sizeof(struct chat_packet));

    /* create what looks like an ordinary UDP socket */
    if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        perror("socket");
        exit(1);
    } else {
        puts("Socket successfully created!\n");
    }

    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
        perror("Reusing ADDR failed");
        exit(1);
    }

    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
    addr.sin_port=htons(HELLO_PORT);

    /* bind to receive address */
    if (bind(fd,(struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr=inet_addr(HELLO_GROUP);
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    printf("Enter your name, stranger: ");
    fgets(self_name, USERNAME_SIZE, stdin);
    printf("Welcome to chat %s\n", HELLO_GROUP);

    newline = strchr(self_name, '\n');
    if (newline != NULL) {
        *newline = 0;
    }
    self_id = add_new_member(self_name);
    announce_member();
    init_listener();

    while (1) {
        printf(">");
        memset(buf, 0, MSG_SIZE * sizeof(char));
        fgets(buf, MSG_SIZE, stdin);

        prepare_packet();

        if (strcmp(EXIT_COMMAND, buf) == 0) {
            puts("Goodbye!");
            delete_member(self_id);
            announce_delete_member();
            close(fd);
            return 0;
        } else if (strcmp(PRINT_COMMAND, buf) == 0) {
            print_all_names();
        } else {
            self_seq++;
            send_packet(&packet);
        }
    }
}

void receive_packet(struct chat_packet *packet) {
    struct sockaddr_in rcv_addr = addr;
    int addrlen;
    memset(packet, 0, sizeof(struct chat_packet));
    addrlen=sizeof(addr);
    if ((recvfrom(fd, packet, sizeof(struct chat_packet), 0,
                  (struct sockaddr *) &rcv_addr, (socklen_t *) &addrlen)) < 0) {
        perror("recvfrom");
        exit(1);
    }
}

void announce_delete_member() {
    struct chat_packet packet;
    memset(&packet, 0, sizeof(struct chat_packet));

    packet.type = MEMBER_REMOVE;
    packet.id = self_id;
    strcpy(packet.name, self_name);
    send_packet(&packet);
}

void announce_member() {
    struct chat_packet packet;
    memset(&packet, 0, sizeof(struct chat_packet));

    packet.type = MEMBER_ANNOUNCE;
    packet.id = self_id;
    strcpy(packet.name, self_name);
    send_packet(&packet);
}

void send_member_response() {
    struct chat_packet packet;
    memset(&packet, 0, sizeof(struct chat_packet));

    packet.type = MEMBER_RESPONSE;
    packet.id = self_id;
    strcpy(packet.name, self_name);
    send_packet(&packet);
}

void init_listener() {
    pthread_t pthread;
    if (pthread_create(&pthread, NULL, &listener, NULL) != 0) {
        puts("error creating listener thread");
        exit(1);
    }
}

void send_packet(struct chat_packet* packet) {
    if (sendto(fd, packet, sizeof(struct chat_packet),
           0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("sendto error");
        exit(1);
    }
}

void prepare_packet() {
    memset(&packet, 0, sizeof(struct chat_packet));
    packet.type = MESSAGE;
    packet.id = self_id;
    packet.seq = self_seq;
    memcpy(packet.message, buf, MSG_SIZE);
    memcpy(packet.name, self_name, USERNAME_SIZE);
}