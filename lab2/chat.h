//
// Created by root on 8.3.17.
//
#include "types.h"

#ifndef LANSS_CHAT_H
#define LANSS_CHAT_H

void prepare_packet();

void send_packet(struct chat_packet *packet);

void init_listener();

unsigned long receive_packet(struct chat_packet *packet);

void announce_member();

void announce_delete_member();

void send_member_response();

int init_socket();

void init_addr();

void set_seed();

#endif //LANSS_CHAT_H
