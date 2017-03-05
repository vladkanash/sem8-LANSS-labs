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

#include "types.h"
#include "member_list.h"

#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"

char buf[MSG_SIZE];
char self_name[USERNAME_SIZE];
struct sockaddr_in addr;
bool running = true;

void prepare_packet();

void send_packet();

void init_listener();

void receive_packet(struct chat_packet *packet);

int fd;

void* listener(void *arg) {
    struct chat_packet packet;

    while (running) {
        receive_packet(&packet);

        if (strcmp(self_name, packet.name)) {
            printf("%s: %s", packet.name, packet.message);
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
    add_member(self_name);
    init_listener();

    while (1) {
        printf(">");
        memset(buf, 0, MSG_SIZE * sizeof(char));
        fgets(buf, MSG_SIZE, stdin);

        prepare_packet();

        if (strcmp(EXIT_COMMAND, buf) == 0) {
            puts("Goodbye!");
            delete_member(self_name);
            close(fd);
            return 0;
        }
        send_packet();
    }
}

void init_listener() {
    pthread_t pthread;
    if (pthread_create(&pthread, NULL, &listener, NULL) != 0) {
        puts("error creating listener thread");
        exit(1);
    }
}

void send_packet() {
    if (sendto(fd, &packet, sizeof(struct chat_packet),
           0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("sendto error");
        exit(1);
    }
}

void prepare_packet() {
    memset(&packet, 0, sizeof(struct chat_packet));
    packet.type = MESSAGE;
    memcpy(packet.message, buf, MSG_SIZE);
    memcpy(packet.name, self_name, USERNAME_SIZE);
}