#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

// ICMP packet types
#define ICMP_ECHOREPLY 0
#define ICMP_DEST_UNREACH 3
#define ICMP_TTL_EXPIRE 11
#define ICMP_ECHO 8

// Minimum ICMP packet size, in bytes
#define ICMP_MIN 8

// The IP header
struct iphdr {
    BYTE ihl:4;           // Length of the header in dwords
    BYTE version:4;         // Version of IP
    BYTE tos;               // Type of service
    USHORT tot_len;       // Length of the packet in dwords
    USHORT id;           // unique identifier
    USHORT frag_off;           // Flags
    BYTE ttl;               // Time to live
    BYTE protocol;             // Protocol number (TCP, UDP etc)
    USHORT check;        // IP checksum
    ULONG saddr;
    ULONG daddr;
};

struct icmphdr {
    BYTE type;          // ICMP packet type
    BYTE code;          // Type sub code
    USHORT checksum;
    union {
        struct {
            USHORT id;
            USHORT sequence;
        } echo;
    } un;
};

#endif