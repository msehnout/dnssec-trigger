/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   forwards_data_types_tools.h
 * Author: fcap
 *
 * Created on 7. září 2016, 11:02
 */

#ifndef FORWARDS_DATA_TYPES_TOOLS_H
#define FORWARDS_DATA_TYPES_TOOLS_H

#include "forwards_data_types.h"
#include <stdbool.h>

CharChain* newCharChain();
void freeCharChain(CharChain *cn);
CharChain* charChain_append(CharChain *chain, char *value);
bool valueInCharChain(CharChain *cn, char* value);
int charChainLength(CharChain *cn);
bool charChainsEqual(CharChain *cn1, CharChain *cn2); // not the order of values, just "is it also there?"
CharChain* copy_CharChain(CharChain *cn);
bool isEmptyCharChain(CharChain *cc);
char* CharChain_to_string(CharChain *input);
void freeCharChainCell(CharChain *cc);
Connection* newConnection();
void freeConnection(Connection *con);
Connection* copy_Connection(Connection* cn);
ConnectionChain* newConnectionChain();
void freeConnectionChainCell(ConnectionChain *cn);
void freeConnectionChain(ConnectionChain *cn, bool preserveValues); // we're freeing in just one direction
ConnectionChain* connectionChain_append(ConnectionChain *chain, Connection *value);
bool isEmpty(ConnectionChain *cn); // only one direction checking
AssocChain* newAssocChain();
void freeAssocChain(AssocChain *cn, bool preserveValues); // we're freeing in just one direction
AssocChain* assocChain_append(AssocChain *cn, char *zone, Connection *con);
ZoneConfig* newZoneConfig();
void freeZoneConfig(ZoneConfig *zn, bool preserveValues);
ZoneConfig* zoneConfig_append(ZoneConfig *what, ZoneConfig *where);
LocalZoneConfig* newLocalZoneConfig();
void freeLocalZoneConfig(LocalZoneConfig *cfg, bool preserveValues);
LocalZoneConfig* localZoneConfig_append(LocalZoneConfig *what, LocalZoneConfig *where);

ConnectionChain* parse_connections(char* input);


#endif /* FORWARDS_DATA_TYPES_TOOLS_H */

