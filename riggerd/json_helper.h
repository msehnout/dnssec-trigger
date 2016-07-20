/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   json_helper.h
 * Author: fcap
 *
 * Created on 18. července 2016, 12:00
 */

#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../vendor/ccan/json/json.h"

typedef struct connection Connection;
typedef struct charChain CharChain;
typedef enum type Type;
typedef struct connectionChain ConnectionChain;

void printType(JsonNode *node);
void handleErr();
CharChain* charChain_append(CharChain *chain, char *value);
ConnectionChain* connectionChain_append(ConnectionChain *chain, Connection *value);
char** charChain_to_array(CharChain *chain);
Connection* connectionChain_to_array(ConnectionChain *chain);
CharChain* newCharChain();
ConnectionChain* newConnectionChain();
void structPrint(Connection *con);
Connection* newConnection();
void outOfMemory();
ConnectionChain* parseConnections(char* input);
void freeConnection(Connection *con);
void freeConnectionChain(ConnectionChain *cn, _Bool preserveValues);
void freeCharChain(CharChain *cn);
_Bool isEmpty(ConnectionChain *cn);
_Bool valueInCharChain(CharChain *cn, char* value);
ConnectionChain* onlyVPN(ConnectionChain *cn);
ConnectionChain* onlyDefault(ConnectionChain *cn);

enum type {
    VPN,
    WIFI,
    OTHER,
    IGNORE,
    DELIMITER
};

struct connection {
    bool default_con;
    CharChain *zones;
    Type type;
    CharChain *servers;
};

struct charChain {
    CharChain *prev;
    char *current;
    CharChain *next;
};

struct connectionChain {
    ConnectionChain *prev;
    Connection *current;
    ConnectionChain *next;
};



#endif /* JSON_HELPER_H */

