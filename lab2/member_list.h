//
// Created by root on 5.3.17.
//

#include <stdbool.h>
#include "types.h"

#ifndef LANSS_MEMBER_LIST_H
#define LANSS_MEMBER_LIST_H

void add_member(const char* name);

bool member_exists(const char* name);

void delete_member(const char* name);

void get_member(const char* name, struct member* member);

#endif //LANSS_MEMBER_LIST_H

