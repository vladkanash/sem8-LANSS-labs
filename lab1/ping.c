#include <fcntl.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ifaddrs.h>

#define PACKETSIZE	64 + sizeof(struct iphdr)
struct packet
{
    struct iphdr ip;
    struct icmphdr icmp;
    char msg[PACKETSIZE - sizeof(struct icmphdr) - sizeof(struct iphdr)];
};

int pid=-1;
bool running = true;
struct protoent *proto=NULL;

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = (unsigned short) ~sum;
    return result;
}

void ping(struct sockaddr_in *addr, struct sockaddr_in *src_addr)
{	const int val=255;
    int i, sockfd, cnt=1;
    struct packet pckt;
    struct sockaddr_in r_addr;

    sockfd = socket(PF_INET, SOCK_RAW, proto->p_proto);

    if ( sockfd < 0 ) {
        perror("socket");
        return;
    }

    if ( setsockopt(sockfd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0) {
        perror("Set TTL option");
    }

    if ( setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val)) != 0) {
        perror("Set IP option");
    }

    if ( fcntl(sockfd, F_SETFL, O_NONBLOCK) != 0 ) {
        perror("Request nonblocking I/O");
    }

    while (running) {
        int len=sizeof(r_addr);

        bzero(&pckt, sizeof(pckt));
        pckt.icmp.type = ICMP_ECHO;
        pckt.icmp.un.echo.id = (u_int16_t) pid;

        pckt.ip.tot_len = htons(sizeof(struct ip) + sizeof(struct icmp));
        pckt.ip.ihl = 5;
        pckt.ip.version = 4;
        pckt.ip.ttl = 64;
        pckt.ip.tos = 0;
        pckt.ip.frag_off = 0;
        pckt.ip.protocol = IPPROTO_ICMP;
        pckt.ip.saddr = src_addr->sin_addr.s_addr;
        pckt.ip.daddr = addr->sin_addr.s_addr;
        pckt.ip.check = checksum(&pckt.ip, sizeof(struct ip));

        for ( i = 0; i < sizeof(pckt.msg)-1; i++ ) {
            pckt.msg[i] = (char) (i + '0');
        }

        pckt.msg[i] = 0;
        pckt.icmp.un.echo.sequence = (u_int16_t) cnt++;
        pckt.icmp.checksum = checksum(&pckt, sizeof(pckt));

        if ( sendto(sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 ) {
            perror("sendto error");
        } else {
            printf("Message sent (seq=%d)\n", pckt.icmp.un.echo.sequence);
        }

        if (recvfrom(sockfd, &pckt, sizeof(pckt), 0,
                     (struct sockaddr *) &r_addr,
                     (socklen_t *) &len) > 0) {
            if (pckt.icmp.type == ICMP_ECHOREPLY) {
                printf("Got Message (seq=%d)\n", pckt.icmp.un.echo.sequence);
            }
        }
        sleep(1);
    }
}

void get_addr(char *host, struct sockaddr_in *addr) {
    struct hostent *hname;
    hname = gethostbyname(host);
    bzero(addr, sizeof((*addr)));
    addr->sin_family = (sa_family_t) hname->h_addrtype;
    addr->sin_port = 0;
    addr->sin_addr.s_addr = (in_addr_t) *(long*)hname->h_addr;
}

void get_my_addr(struct sockaddr_in *myAddr) {
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
}

int main(int argc, char *argv[]) {
    struct sockaddr_in dest_addr, src_addr;

    if ( argc < 2 || argc > 3) {
        printf("usage: %s <addr> [dest_addr]\n", argv[0]);
        exit(0);
    }
    if (argc == 2) {
        get_addr(argv[1], &dest_addr);
        get_my_addr(&src_addr);
    }
    if (argc == 3) {
        get_addr(argv[2], &src_addr);
    }

    pid = getpid();
    proto = getprotobyname("ICMP");
    ping(&dest_addr, &src_addr);
    return 0;
}