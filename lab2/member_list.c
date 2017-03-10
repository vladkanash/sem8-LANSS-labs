//
// Created by root on 5.3.17.
//

#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "member_list.h"

static struct member members[MAX_MEMBERS];

long get_uuid();

static int last = 0;

long add_new_member(const char *name) {
    if (name == NULL) {
        return 0;
    }

    struct member new_member;
    memcpy(&new_member.name, name, USERNAME_SIZE);
    new_member.id = get_uuid();
    memcpy(&members[last++], &new_member, sizeof(struct member));
    return new_member.id;
}

long get_uuid() {
#ifdef WIN32
    return rand();
#elif __linux__
    return random();
#endif
}

bool member_exists(long id) {
    for (int i = 0; i < last; i++) {
        if (members[i].id == id) {
            return true;
        }
    }
    return false;
}

void delete_member(long id) {
    for (int i = 0; i < last; i++) {
        if (members[i].id == id) {
            for (int k = i; k < last; k++) {
                memcpy(&members[k], &members[k + 1], sizeof(struct member));
            }
            last--;
            return;
        }
    }
}

void get_member(long id, struct member* member) {
    if (member == NULL) {
        return;
    }

    for (int i = 0; i < last; i++) {
        if (members[i].id == id) {
            memcpy(member, &members[i], sizeof(struct member));
        }
    }
    return;
}

void print_all_names() {
    puts("All chat members:");
    for (int i = 0; i < last; i++) {
        printf("%d: %s\n", i+1, members[i].name);
    }
}

void add_existing_member(const struct chat_packet* member) {
    if (member == NULL) {
        return;
    }

    struct member new_member;
    memcpy(&new_member.name, member->name, USERNAME_SIZE);
    new_member.id = member->id;
    memcpy(&members[last++], &new_member, sizeof(struct member));
    return;
}

