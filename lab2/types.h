//
// Created by root on 1.3.17.
//

#ifndef LANSS_TYPES_H
#define LANSS_TYPES_H

#define USERNAME_SIZE 32
#define MSG_SIZE 300

#define EXIT_COMMAND "exit\n"

struct chat_packet {
    char name[USERNAME_SIZE];
    size_t msg_size;
    char message[MSG_SIZE];
    #define PACKET_SIZE packet.msg_size + USERNAME_SIZE + sizeof(size_t)
} packet;

#endif //LANSS_TYPES_H
