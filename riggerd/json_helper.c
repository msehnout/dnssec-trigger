/*
 * util/json_helper.c - helper for parsing json input to Connection
 *
 * Copyright (c) 2016, NLnet Labs. All rights reserved.
 *
 * This software is open source.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * Neither the name of the NLNET LABS nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * This file contains functions needed for proper parsing json input as Connection
 * 
 */

#include "json_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../vendor/ccan/json/json.h"

/**
 * Parses input json char* to ConnectionChain
 * @Example //TODO:
 * 
 * @param input
 * @return ConnectionChain*
 */
ConnectionChain* parseConnections(char* input) {
    printf(input);
    printf("\n");

    if (!input || !json_validate(input)) {
        handleErr();

    }

    JsonNode *node = json_decode(input); //shouldn't be null because we checked if it is valid
    if (node->tag != JSON_OBJECT) {
        handleErr();

    }

    node = node->children.head; // now it should be the first dictionary value e.g. connections

    if (!node || strcmp(node->key, "connections") != 0) { // and also must be array
        handleErr();

    }
    // node is array called connections

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connection *con;
    ConnectionChain *cons = newConnectionChain();
    JsonNode *connection = node->children.head;

    while (NULL != connection) {
        con = newConnection();
        JsonNode *param = connection->children.head;
        //zpracovani paramu

        while (NULL != param) {
            if (param->tag == JSON_BOOL && strcmp(param->key, "default") == 0) {
                con->default_con = param->bool_;
            } else if (param->tag == JSON_STRING && strcmp(param->key, "type") == 0) {
                if (strcmp(param->string_, "wifi") == 0) {
                    con->type = WIFI;
                } else if (strcmp(param->string_, "vpn") == 0) {
                    con->type = VPN;
                } else if (strcmp(param->string_, "other") == 0) {
                    con->type = OTHER;
                } else {
                    con->type = IGNORE;
                }
            } else if (param->tag == JSON_ARRAY && strcmp(param->key, "zones") == 0) {
                JsonNode *zone = param->children.head;
                CharChain *zn = newCharChain();

                while (NULL != zone) {
                    char *znc = calloc(strlen(zone->string_) + 1, sizeof (char));
                    if (NULL == znc)
                        outOfMemory();
                    strcpy(znc, zone->string_);
                    charChain_append(zn, znc);

                    zone = zone->next;
                }
                con->zones = zn;
            } else if (param->tag == JSON_ARRAY && strcmp(param->key, "servers") == 0) {
                JsonNode *server = param->children.head;
                CharChain *sv = newCharChain();

                while (NULL != server) {
                    char *svc = calloc(strlen(server->string_) + 1, sizeof (char));
                    if (NULL == svc)
                        outOfMemory();
                    strcpy(svc, server->string_);
                    charChain_append(sv, svc);

                    server = server->next;
                }
                con->servers = sv;
            }

            param = param->next;

        }
        connection = connection->next;
        connectionChain_append(cons, con);
        // structPrint(con);
        // free(con);
    }

    // should free chain pointers to pointers?


    json_delete(node); // not sure if it deletes the whole tree
    return cons;
}

//void printType(JsonNode *node) {
//    if (node->tag == JSON_ARRAY)
//        printf("array\n");
//    else if (node->tag == JSON_STRING)
//        printf("string\n");
//    else if (node->tag == JSON_OBJECT)
//        printf("object\n");
//}

void handleErr() {
    printf("bad json input");
    exit(-1);
}

CharChain* newCharChain() {
    CharChain *cn = calloc(1, sizeof (CharChain));
    if (NULL == cn) {
        outOfMemory();
    }
    return cn;
}

void freeCharChain(CharChain *cn) {
    if (NULL != cn->next) {
        freeCharChain(cn->next);
    }
    free(cn->next);
    free(cn->current);

    if (NULL == cn->prev)
        free(cn);
}

static ConnectionChain* newConnectionChain() {
    ConnectionChain *cn = calloc(1, sizeof (ConnectionChain));
    if (NULL == cn) {
        outOfMemory();
    }
    return cn;
}

void freeConnectionChain(ConnectionChain *cn, bool preserveValues) { // we're freeing in just one direction
    if (NULL != cn->next) {
        freeConnectionChain(cn->next, preserveValues);
        free(cn->next);
    }

    if (!preserveValues && NULL != cn->current)
        freeConnection(cn->current);

    if (NULL == cn->prev)
        free(cn);
}

static Connection* newConnection() {
    Connection *con = calloc(1, sizeof (Connection));
    if (NULL == con) {
        outOfMemory();
    }
    return con;
}

static void freeConnection(Connection *con) {
    if (con) {
        //doesn't have to be freeed?
        //		free(&con->default_con); //everything should be allocated
        //		free(&con->type);
        freeCharChain(con->servers);
        freeCharChain(con->zones);
    }
}


/*DEBUG ONLY*/
void structPrint(Connection *con) {
    if (con->default_con) {
        printf("default: true\n");
    } else {
        printf("default: false\n");
    }
    printf("type: ");
    switch (con->type) {
        case WIFI:
            printf("wifi\n");
            break;

        case VPN:
            printf("vpn\n");
            break;

        default:
        case OTHER:
            printf("other\n");
            break;

    }

    printf("servers: \n");
    // it is possible, the servers are null we have to catch it
    for (CharChain *i = con->servers; i != NULL; i = i->next) {
        printf(i->current);
        printf("\n");
    }

    printf("\n");
    printf("zones: \n");
    // zones also can be null so here we have to catch it, too.
    for (CharChain *i = con->zones; i != NULL; i = i->next) {
        printf(i->current);
        printf("\n");
    }
    printf("\n");
}

static void outOfMemory() {
    printf("OUT OF MEMORY!");
    exit(-1);
}

CharChain* charChain_append(CharChain *chain, char *value) {
    CharChain *tmp = chain;
    if (NULL == tmp->current) {
        tmp->current = value;
        tmp->prev = NULL;
        tmp->next = NULL;
        return chain;
    }

    while (NULL != tmp->next) {
        tmp = tmp->next;
    }

    CharChain *vl = newCharChain();
    vl->current = value;
    vl->prev = tmp;
    tmp->next = vl;

    return chain;

}

static ConnectionChain* connectionChain_append(ConnectionChain *chain, Connection *value) {
    ConnectionChain *tmp = chain;
    if (NULL == tmp->current) {
        tmp->current = value;
        tmp->prev = NULL;
        tmp->next = NULL;
        return chain;
    }

    while (NULL != tmp->next) {
        tmp = tmp->next;
    }

    ConnectionChain *vl = newConnectionChain();
    vl->current = value;
    vl->prev = tmp;
    vl->next = NULL;
    tmp->next = vl;

    return chain;
}

//char** charChain_to_array(CharChain *chain) {
//    CharChain *tmp = chain;
//    int length = 1; // because we need one for null pointer as an array delimiter
//
//    if (NULL != chain->current) {
//        length++;
//    }
//
//    while (tmp->next) {
//        tmp = tmp->next;
//        length++;
//    }
//
//    char **arry = calloc(length, sizeof (char*));
//    if (NULL == arry) {
//        outOfMemory();
//    }
//
//    tmp = chain;
//    for (int i = 0; i < length - 1; i++) {
//        arry[i] = tmp->current;
//        tmp = tmp->next;
//    }
//
//    return arry;
//}

//Connection* connectionChain_to_array(ConnectionChain *chain) {
//    ConnectionChain *tmp = chain;
//    int length = 1; // because we need one for null pointer as an array delimiter
//
//    if (NULL != chain->current) {
//        length++;
//    }
//
//    while (tmp->next) {
//        tmp = tmp->next;
//        length++;
//    }
//
//    Connection *arry = calloc(length, sizeof (Connection*));
//    if (NULL == arry) {
//        outOfMemory();
//    }
//
//    tmp = chain;
//    for (int i = 0; i < length - 1; i++) {
//        arry[i] = *tmp->current;
//        tmp = tmp->next;
//    }
//    Connection *nl = calloc(1, sizeof(Connection));
//    nl->type = DELIMITER;
//    arry[length] = *nl;
//    return arry;
//}

ConnectionChain* onlyVPN(ConnectionChain *cn) {
    ConnectionChain *toRet = newConnectionChain();

    for (ConnectionChain *i = cn; i != NULL; i = i->next) {
        if (i->current->type == VPN) {
            connectionChain_append(toRet, i->current);
        }
    }
    return toRet;
}

ConnectionChain* onlyDefault(ConnectionChain *cn) {
    ConnectionChain *toRet = newConnectionChain();

    for (ConnectionChain *i = cn; i != NULL; i = i->next) {
        if (i->current->default_con) {
            connectionChain_append(toRet, i->current);
        }
    }
    return toRet;
}

bool isEmpty(ConnectionChain *cn) { // only one direction checking
    if (NULL == cn || (NULL == cn->current && NULL == cn->next))
        return true;
    return false;
}

bool valueInCharChain(CharChain *cn, char* value) {
    if (NULL == value) {
        return false;
    }
    for (CharChain *i = cn; NULL != i; i = i->next) { // po inicializaci se take provede kontrola podminky?
        if (NULL == i->current)
            return false;
        if (strcmp(i->current, value) == 0)
            return true;
    }

    return false;
}

static int charChainLength(CharChain *cn) {
    int len = 0;

    CharChain *tmp = cn;
    while (NULL != tmp) {
        tmp = tmp->next;
        len++;
    }

    return len;
}

bool charChainsEqual(CharChain *cn1, CharChain *cn2) { // not the order of values, just "is it also there?"
    if (NULL == cn1 && NULL == cn2)
        return true;

    if ((NULL == cn1 && NULL != cn2) || (NULL == cn2 && NULL != cn1))
        return false;

    if (charChainLength(cn1) != charChainLength(cn2)) { // we assume every value is unique
        return false;
    }

    for (CharChain *i = cn1; NULL != i->next; i = i->next) {
        if (!valueInCharChain(cn2, i->current))
            return false;
    }

    return true;

}

