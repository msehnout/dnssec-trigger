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

/**
 * Initialize a new list of strings
 * @param list: New list
 */
void string_list_init(struct string_list* list);

/**
 * Clear the list and free all contained buffers
 * @param list: List to be cleared. To free the structure itself is caller responsibility.
 */
void string_list_clear(struct string_list* list);

/**
 * Push new string at the end of the list. The string
 * is copied into the node.
 * @param list: List to append to
 * @param new_value: String to be appended
 * @param buffer_size: Size of the buffer from which the string is copied
 */
void string_list_push_back(struct string_list* list, const char* new_value, const size_t buffer_size);

/**
 * Find out whether the list contains the given value
 * @param list: List to check
 * @param new_value: String to be found
 * @param buffer_size: Size of the string buffer
 */
bool string_list_contains(const struct string_list* list, const char* value, const size_t buffer_size);

/**
 * Find out the size of given list
 * @param list: List to check
 */
size_t string_list_length(const struct string_list* list);

/**
 * Compare content of two lists. Every value must be unique.
 * @param l1: First list
 * @param l2: Second list
 */
bool string_list_is_equal(const struct string_list* l1, const struct string_list* l2);

#endif /* FORWARDS_DATA_TYPES_TOOLS_H */

