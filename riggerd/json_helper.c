/*
 * json_helper.c
 *
 *  Created on: 14. 7. 2016
 *      Author: fcap
 */

#include "json_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../vendor/ccan/json/json.h"

/*
 *
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
                    charChain_append(zn, zone->string_);

                    zone = zone->next;
                }
                con->zones = zn;
            } else if (param->tag == JSON_ARRAY && strcmp(param->key, "servers") == 0) {
                JsonNode *server = param->children.head;
                CharChain *sv = newCharChain();

                while (NULL != server) {
                    charChain_append(sv, server->string_);

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
    return cons;
}

void printType(JsonNode *node) {
    if (node->tag == JSON_ARRAY)
        printf("array\n");
    else if (node->tag == JSON_STRING)
        printf("string\n");
    else if (node->tag == JSON_OBJECT)
        printf("object\n");
}

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

ConnectionChain* newConnectionChain() {
    ConnectionChain *cn = calloc(1, sizeof (ConnectionChain));
    if (NULL == cn) {
        outOfMemory();
    }
    return cn;
}

void freeConnectionChain(ConnectionChain *cn, _Bool preserveValues) { // we're freeing in just one direction
    if (NULL != cn->next) {
        freeConnectionChain(cn->next, preserveValues);
        free(cn->next);
    }

    if (!preserveValues && NULL != cn->current)
        freeConnection(cn->current);

    if (NULL == cn->prev)
        free(cn);
}

Connection* newConnection() {
    Connection *con = calloc(1, sizeof (Connection));
    if (NULL == con) {
        outOfMemory();
    }
    return con;
}

void freeConnection(Connection *con) {
    if (con) {
        //doesn't have to be freeed?
        //		free(&con->default_con); //everything should be allocated
        //		free(&con->type);
        freeCharChain(con->servers);
        freeCharChain(con->zones);
    }
}

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

void outOfMemory() {
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

ConnectionChain* connectionChain_append(ConnectionChain *chain, Connection *value) {
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

_Bool isEmpty(ConnectionChain *cn) { // only one direction checking
    if (NULL == cn || (NULL == cn->current && NULL == cn->next))
        return true;
    return false;
}

_Bool valueInCharChain(CharChain *cn, char* value) {
    for (CharChain *i = cn; NULL != i; i = i->next) { // po inicializaci se take provede kontrola podminky?
        if (NULL == i->current)
            return false;
        if (strcmp(i->current, value) == 0)
            return true;
    }

    return false;
}


