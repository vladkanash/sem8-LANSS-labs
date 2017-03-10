//
// Created by root on 1.3.17.
//

#ifndef LANSS_TYPES_H
#define LANSS_TYPES_H

#define USERNAME_SIZE 32
#define MSG_SIZE 300
#define MAX_MEMBERS 512

#define EXIT_COMMAND "exit\n"
#define PRINT_COMMAND "print\n"

enum packet_type {MESSAGE, MEMBER_ANNOUNCE, MEMBER_RESPONSE, MEMBER_REMOVE};

struct chat_packet {
    enum packet_type type;
    int id;
    int seq;
    char name[USERNAME_SIZE];
    char message[MSG_SIZE];
} packet;

struct member {
    int id;
    int seq;
    char name[USERNAME_SIZE];
    char ip[20];
};

#endif //LANSS_TYPES_H
