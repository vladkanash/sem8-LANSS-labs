#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "types.h"

#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"

char buf[MSG_SIZE];
char name[USERNAME_SIZE];

int main(int argc, char *argv[]) {
    struct sockaddr_in addr;
    int fd, cnt;
    struct ip_mreq mreq;

    memset(buf, 0, MSG_SIZE * sizeof(char));
    memset(&packet, 0, sizeof(struct chat_packet));

    /* create what looks like an ordinary UDP socket */
    if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        perror("socket");
        exit(1);
    } else {
        puts("Socket successfully created!\n");
    }

    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
    addr.sin_port=htons(HELLO_PORT);

    printf("Enter your name, stranger: ");
    fgets(name, USERNAME_SIZE, stdin);
    printf("Welcome to chat %s\n", HELLO_GROUP);

    while (1) {
        printf(">");
        fgets(buf, MSG_SIZE, stdin);

        memset(&packet, 0, sizeof(struct chat_packet));
        memcpy(packet.message, buf, MSG_SIZE);
        memcpy(packet.name, name, USERNAME_SIZE);
        packet.msg_size = strlen(packet.message);

        if (strcmp(EXIT_COMMAND, buf) == 0) {
            puts("Goodbye!");
            close(fd);
            return 0;
        }

        if (sendto(fd, &packet, PACKET_SIZE,
                   0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            perror("sendto error");
            exit(1);
        }
    }
}