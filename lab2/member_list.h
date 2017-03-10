//
// Created by root on 5.3.17.
//

#include <stdbool.h>
#include "types.h"

#ifndef LANSS_MEMBER_LIST_H
#define LANSS_MEMBER_LIST_H

long add_new_member(const char *name);

void add_existing_member(const struct chat_packet *member, unsigned long ip);

bool member_exists(int id);

void delete_member(int id);

void get_member(int id, struct member* member);

void print_all_names();

#endif //LANSS_MEMBER_LIST_H

