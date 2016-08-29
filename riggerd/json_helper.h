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
#include <unistd.h>
#include "../vendor/ccan/json/json.h"
#include "json_data_types.h"

ConnectionChain * parseConnections(char * input);
void printType(JsonNode * node);
void handleErr();
CharChain * newCharChain();
void freeCharChain(CharChain * cn);
ConnectionChain * newConnectionChain();
void freeConnectionChain(ConnectionChain * cn, bool preserveValues);
Connection * newConnection();
void freeConnection(Connection * con);
void structPrint(Connection * con);
void outOfMemory();
void charChain_append_double(CharChain * * head, char * value);
CharChain * charChain_append(CharChain * chain, char * value);
ConnectionChain * connectionChain_append(ConnectionChain * chain, Connection * value);
void freeConnectionChainCell(ConnectionChain * cn);
ConnectionChain * onlyVPN(ConnectionChain * cn);
ConnectionChain * onlyDefault(ConnectionChain * cn);
bool isEmpty(ConnectionChain * cn);
bool valueInCharChain(CharChain * cn, char * value);
int charChainLength(CharChain * cn);
bool charChainsEqual(CharChain * cn1, CharChain * cn2);
CharChain * copy_CharChain(CharChain * cn);
Connection * copy_Connection(Connection * cn);
ConnectionChain * copy_ConnectionChain(ConnectionChain * cn);
ConnectionChain * noWifi(ConnectionChain * cn);
Connection * getPreferredConnection(Connection * first, Connection * second);
AssocChain * newAssocChain();
void freeAssocChain(AssocChain * cn, bool preserveValues);
AssocChain * assocChain_append(AssocChain * cn, char * zone, Connection * con);
AssocChain * getAssocChainWithZone(AssocChain * cn, char * zone);
AssocChain * getZoneConnectionMapping(ConnectionChain * connections);
ZoneConfig * newZoneConfig();
void freeZoneConfig(ZoneConfig * zn, bool preserveValues);
ZoneConfig * zoneConfig_append(ZoneConfig * what, ZoneConfig * where);
ZoneConfig * getUnboundZoneConfig();
LocalZoneConfig * newLocalZoneConfig();
void freeLocalZoneConfig(LocalZoneConfig * cfg, bool preserveValues);
LocalZoneConfig * localZoneConfig_append(LocalZoneConfig * what, LocalZoneConfig * where);
LocalZoneConfig * getUnboundLocalZoneConfig();
bool isEmptyCharChain(CharChain * cc);
void unbound_local_zones_add(char * zone, char * type);
void unbound_zones_remove(char * zone, char * flush_command);
void freeCharChainCell(CharChain * cc);
void stored_zones_remove_double(CharChain * * chain, char * zone);
void stored_zones_remove(char * zone);
char * servers_to_string(CharChain * servers);
void unbound_zones_add(char * zone, CharChain * servers, bool validate);
void unbound_local_zones_remove(char * zone);

#endif /* JSON_HELPER_H */

