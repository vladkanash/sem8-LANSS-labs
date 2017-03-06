//
// Created by root on 5.3.17.
//

#include <stdbool.h>
#include "types.h"

#ifndef LANSS_MEMBER_LIST_H
#define LANSS_MEMBER_LIST_H

long add_member(const char* name);

bool member_exists(long id);

void delete_member(long id);

void get_member(long id, struct member* member);

void print_all_names();

#endif //LANSS_MEMBER_LIST_H

