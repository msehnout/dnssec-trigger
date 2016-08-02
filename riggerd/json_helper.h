/*
 * util/json_helper.h - helper for parsing json input to Connection
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


/*
 * File:   json_helper.h
 * Author: fcap
 *
 * Created on 18. července 2016, 12:00
 */

#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../vendor/ccan/json/json.h"

typedef struct connection Connection;
typedef struct charChain CharChain;
typedef enum connection_type ConnectionType;
typedef struct connectionChain ConnectionChain;

ConnectionChain* parseConnections(char* input);
void handleErr();
CharChain* newCharChain();
void freeCharChain(CharChain *cn);
static ConnectionChain* newConnectionChain();
void freeConnectionChain(ConnectionChain *cn, bool preserveValues);
static Connection* newConnection();
static void freeConnection(Connection *con);
void structPrint(Connection *con);
static void outOfMemory();
CharChain* charChain_append(CharChain *chain, char *value);
static ConnectionChain* connectionChain_append(ConnectionChain *chain, Connection *value);
//char** charChain_to_array(CharChain *chain);
//Connection* connectionChain_to_array(ConnectionChain *chain);
ConnectionChain* onlyVPN(ConnectionChain *cn);
ConnectionChain* onlyDefault(ConnectionChain *cn);
bool isEmpty(ConnectionChain *cn);
bool valueInCharChain(CharChain *cn, char* value);
static int charChainLength(CharChain *cn);
bool charChainsEqual(CharChain *cn1, CharChain *cn2);

enum connection_type {
    VPN,
    WIFI,
    OTHER,
    IGNORE,
    DELIMITER
};

struct connection {
    bool default_con;
    CharChain *zones;
    ConnectionType type;
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

