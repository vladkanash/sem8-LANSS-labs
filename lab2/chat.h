//
// Created by root on 8.3.17.
//
#include "types.h"

#ifndef LANSS_CHAT_H
#define LANSS_CHAT_H

void prepare_packet();

void send_packet(struct chat_packet *packet);

void init_listener();

void receive_packet(struct chat_packet *packet);

void announce_member();

void announce_delete_member();

void send_member_response();

#endif //LANSS_CHAT_H
