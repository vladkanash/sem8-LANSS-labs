//
// Created by root on 5.3.17.
//

#include <wchar.h>
#include <string.h>
#include "member_list.h"

static struct member members[MAX_MEMBERS];
static int last = 0;

void add_member(const char* name) {
    if (name == NULL || member_exists(name)) {
        return;
    }

    struct member new_member;
    memcpy(&new_member, name, USERNAME_SIZE);
    memcpy(&members[last++], &new_member, sizeof(struct member));
}

bool member_exists(const char* name) {
    if (name == NULL) {
        return false;
    }

    for (int i = 0; i < last; i++) {
        if (strcmp(members[i].name, name) == 0) {
            return true;
        }
    }
    return false;
}

void delete_member(const char* name) {
    if (name == NULL) {
        return;
    }

    for (int i = 0; i < last; i++) {
        if (strcmp(members[i].name, name) == 0) {
            for (int k = i; k < last; k++) {
                memcpy(&members[k], &members[k + 1], sizeof(struct member));
            }
            last--;
            return;
        }
    }
}

void get_member(const char* name, struct member* member) {
    if (name == NULL) {
        return;
    }

    for (int i = 0; i < last; i++) {
        if (strcmp(members[i].name, name) == 0) {
            memcpy(member, &members[i], sizeof(struct member));
        }
    }
    return;
}