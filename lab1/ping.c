#ifdef __linux__
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <ifaddrs.h>
#elif _WIN32
#include<stdio.h>
#include<winsock2.h>
#include <ws2tcpip.h>
#include "types.h"
#endif

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "types.h"

int sockfd, pid=-1;
bool running = true;

int init_socket() {
#ifdef _WIN32
    WSADATA wsa;
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");

    if((sockfd = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, 0, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");
    return sockfd;
#elif __linux__
    struct protoent *proto = getprotobyname("ICMP");
    sockfd = socket(PF_INET, SOCK_RAW, proto->p_proto);
        if ( sockfd < 0 ) {
        perror("socket");
        exit(0);
    }
#else
    return 0;
#endif
}

void set_socket_options() {
#ifdef __linux__
    const int val=255;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, (const char *) &val, sizeof(val)) != 0) {
        perror("Set TTL option");
    }
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, (const char *) &val, sizeof(val)) != 0) {
        perror("Set IP option");
    }
    if ( fcntl(sockfd, F_SETFL, O_NONBLOCK) != 0 ) {
        perror("Request nonblocking I/O");
    }
#elif _WIN32
    int ttl = DEFAULT_TTL;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, (const char*)&ttl,
                   sizeof(ttl)) == SOCKET_ERROR) {
        printf("TTL setsockopt failed: %d\n", WSAGetLastError());
        exit(-1);
    }
#endif
}

void get_my_addr(struct sockaddr_in *myAddr) {
#ifdef __linux__
    struct ifaddrs* addr, *tmp;
    struct sockaddr_in *pAddr = NULL;
    getifaddrs(&addr);
    tmp = addr;
    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            pAddr = (struct sockaddr_in *) tmp->ifa_addr;
        }

        tmp = tmp->ifa_next;
    }

    memcpy(myAddr, pAddr, sizeof(struct sockaddr_in));
    freeifaddrs(addr);
#elif _WIN32
    //not implemented yet
#endif
}

void init_packet(struct packet *pckt,
                 const struct sockaddr_in *addr,
                 const struct sockaddr_in *src_addr,
                 int* cnt) {
    int i;
    memset(pckt, 0, sizeof(struct packet));

#ifdef __linux__
    pckt->ip.tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
    pckt->ip.ihl = 5;
    pckt->ip.version = 4;
    pckt->ip.ttl = 64;
    pckt->ip.tos = 0;
    pckt->ip.frag_off = 0;
    pckt->ip.protocol = IPPROTO_ICMP;
    pckt->ip.saddr = src_addr->sin_addr.s_addr;
    pckt->ip.daddr = addr->sin_addr.s_addr;
    pckt->ip.check = checksum(&pckt->ip, sizeof(struct iphdr));
#endif

    for ( i = 0; i < sizeof(pckt->msg) - 1; i++ ) {
        pckt->msg[i] = (char) (i + '0');
    }
    pckt->msg[i] = 0;

    pckt->icmp.type = ICMP_ECHO;
    pckt->icmp.un.echo.id = pid;
    pckt->icmp.un.echo.sequence = (*cnt)++;
    pckt->icmp.checksum = checksum(pckt, sizeof(struct packet));
}


void ping(struct sockaddr_in *addr, struct sockaddr_in *src_addr) {
    int cnt=1;
    struct packet pckt;
    struct sockaddr_in r_addr;
    struct response_packet response;

    set_socket_options();

    while (running) {
        int len=sizeof(r_addr);

        init_packet(&pckt, addr, src_addr, &cnt);

        if (sendto(sockfd, (const char *) &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 ) {
            perror("sendto error");
        } else {
            printf("Message sent (seq=%d)\n", pckt.icmp.un.echo.sequence);
        }

        if (recvfrom(sockfd, (char *) &response, sizeof(struct response_packet), 0,
                     (struct sockaddr *) &r_addr,
                     (socklen_t *) &len) > 0) {

            if (response.icmp.type == ICMP_ECHOREPLY) {
                printf("Got Message (seq=%d)\n", response.icmp.un.echo.sequence);
            }
        }
        sleep(1);
    }
}

void get_addr(char *host, struct sockaddr_in *addr) {
    struct hostent *hname;
    hname = gethostbyname(host);
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = hname->h_addrtype;
    addr->sin_port = 0;
    memcpy(&(addr->sin_addr), hname->h_addr, hname->h_length);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in dest_addr, src_addr;

    if ( argc < 2 || argc > 3) {
        printf("usage: %s <addr> [dest_addr]\n", argv[0]);
        exit(0);
    }
    init_socket();
    if (argc >= 2) {
        get_addr(argv[1], &dest_addr);
        get_my_addr(&src_addr);
    }
    if (argc == 3) {
        get_addr(argv[2], &src_addr);
    }

    pid = getpid();
    ping(&dest_addr, &src_addr);
    return 0;
}