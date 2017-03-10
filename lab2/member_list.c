//
// Created by root on 5.3.17.
//

#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "member_list.h"

static struct member members[MAX_MEMBERS];

int get_uuid();

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

int get_uuid() {
    return rand();
}

bool member_exists(int id) {
    for (int i = 0; i < last; i++) {
        if (members[i].id == id) {
            return true;
        }
    }
    return false;
}

void delete_member(int id) {
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

void get_member(int id, struct member* member) {
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
        printf("%d: %s (%s)\n", i+1, members[i].name, members[i].ip);
    }
}

void ip_to_str(unsigned long ip, char *str, size_t size) {
    if (size < 16) {
        return;
    }

    memset(str, 0, size);
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    sprintf(str, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
}

void add_existing_member(const struct chat_packet* member, unsigned long ip) {
    if (member == NULL) {
        return;
    }
    char ip_str[20];
    ip_to_str(ip, ip_str, 20);

    struct member new_member;
    memcpy(&new_member.name, member->name, USERNAME_SIZE);
    memcpy(&new_member.ip, ip_str, 20);
    new_member.id = member->id;
    memcpy(&members[last++], &new_member, sizeof(struct member));
    return;
}

void clear_members() {
    last = 0;
    memset(members, 0, sizeof(struct member) * MAX_MEMBERS);
}

