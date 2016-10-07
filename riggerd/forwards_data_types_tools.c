/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "forwards_data_types.h"
#include "vendor/ccan/json/json.h"
#include "config.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

CharChain* newCharChain()
{
    CharChain *cn = calloc(1, sizeof (CharChain));
    if (NULL == cn) {
        fatal_exit("out of memory");
    }
    return cn;
}

void freeCharChain(CharChain *cn)
{
    if (NULL != cn->next) {
        freeCharChain(cn->next);
        //free(cn->next);
    }
    free(cn->current);

    //if(NULL == cn->prev)
    free(cn);
}

CharChain* charChain_append(CharChain *chain, char *value)
{
    if (chain == NULL) {
        chain = newCharChain();
        chain->current = value;
        return chain;
    }

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

bool valueInCharChain(CharChain *cn, char* value)
{
    if (NULL == value) {
        return false;
    }
    for (CharChain *i = cn; NULL != i; i = i->next) {
        if (NULL == i->current)
            return false;
        if (strcmp(i->current, value) == 0)
            return true;
    }

    return false;
}

int charChainLength(CharChain *cn)
{
    int len = 0;

    CharChain *tmp = cn;
    while (NULL != tmp) {
        tmp = tmp->next;
        len++;
    }

    return len;
}

bool charChainsEqual(CharChain *cn1, CharChain *cn2) // not the order of values, just "is it also there?"
{ 
    if (NULL == cn1 && NULL == cn2)
        return true;

    if ((NULL == cn1 && NULL != cn2) || (NULL == cn2 && NULL != cn1))
        return false;

    if (charChainLength(cn1) != charChainLength(cn2)) { // we pretend every value is unique
        return false;
    }

    for (CharChain *i = cn1; NULL != i->next; i = i->next) {
        if (!valueInCharChain(cn2, i->current))
            return false;
    }

    return true;

}

CharChain* copy_CharChain(CharChain *cn)
{
    if (cn == NULL)
        return NULL;

    CharChain *toRet = newCharChain();


    CharChain *nxt = NULL;
    if (cn->next != NULL) {
        nxt = copy_CharChain(cn->next);
    }
    toRet->next = nxt;

    if (cn->current != NULL) {
        char *chr = calloc(strlen(cn->current) + 1, sizeof (char));
        if (chr == NULL) {
            fatal_exit("out of memory");
        }

        strcpy(chr, cn->current);
        toRet->current = chr;
    }

    if (toRet->next != NULL) {
        toRet->next->prev = toRet;
    }
    return toRet;
}

bool isEmptyCharChain(CharChain *cc)
{
    if (NULL == cc || (NULL == cc->current && NULL == cc->next))
        return true;
    return false;
}

char* CharChain_to_string(CharChain *input) {
    if (!input) {
        char *str = calloc(1, sizeof(char));
        if(!str)
            fatal_exit("out of memory");
        return str;
    }

    int len = 0;
    for (CharChain *c = input; c != NULL; c = c->next) {
        if (!c->current)
            continue;

        len += strlen(c->current);
        len++;
    }

    char *str = calloc(len, sizeof (char));
    if (!str) {
        fatal_exit("out of memory");
    }
    strcpy(str, input->current);
    strcat(str, " ");

    for (CharChain *c = input->next; c != NULL; c = c->next) {
        if (!c->current)
            continue;

        strcat(str, c->current);
        strcat(str, " ");
    }
    str[len - 1] = '\0'; // string delimiter instead of the last space

    return str;
}

void freeCharChainCell(CharChain *cc)
{
    free(cc->current);
    free(cc);
}

Connection* newConnection()
{
    Connection *con = calloc(1, sizeof (Connection));
    if (NULL == con) {
        fatal_exit("out of memory");
    }
    return con;
}

void freeConnection(Connection *con)
{
    if (con != NULL) {
        // default_con & type doesn't have to be freed/ mustn't be freed they weren't allocated by malloc
        freeCharChain(con->servers);
        freeCharChain(con->zones);
        free(con);
    }
}

Connection* copy_Connection(Connection* cn)
{

    if (cn == NULL) {
        return NULL;
    }
    Connection *toRet = newConnection();


    toRet->default_con = cn->default_con;
    toRet->type = cn->type;
    toRet->servers = copy_CharChain(cn->servers);
    toRet->zones = copy_CharChain(cn->zones);

    return toRet;
}

ConnectionChain* newConnectionChain()
{
    ConnectionChain *cn = calloc(1, sizeof (ConnectionChain));
    if (NULL == cn) {
        fatal_exit("out of memory");
    }
    return cn;
}

void freeConnectionChainCell(ConnectionChain *cn)
{
    freeConnection(cn->current);
    free(cn);
}

void freeConnectionChain(ConnectionChain *cn, bool preserveValues) // we're freeing in just one direction
{ 
    if (NULL != cn->next) {
        freeConnectionChain(cn->next, preserveValues);
        free(cn->next);
    }

    if (!preserveValues && NULL != cn->current)
        freeConnection(cn->current);

    if (NULL == cn->prev)
        free(cn);
}

ConnectionChain* connectionChain_append(ConnectionChain *chain, Connection *value)
{
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

bool isEmpty(ConnectionChain *cn)
{ // only one direction checking
    if (NULL == cn || (NULL == cn->current && NULL == cn->next))
        return true;
    return false;
}

AssocChain* newAssocChain()
{
    AssocChain *cn = calloc(1, sizeof (AssocChain));

    if (cn == NULL) {
        fatal_exit("out of memory");
    }

    return cn;
}

void freeAssocChain(AssocChain *cn, bool preserveValues) // we're freeing in just one direction
{ 

    if (NULL != cn->next) {
        freeAssocChain(cn->next, preserveValues);
        free(cn->next);
    }

    if (!preserveValues && NULL != cn->zone)
        free(cn->zone);

    if (!preserveValues && NULL != cn->connection)
        freeConnection(cn->connection);

    if (NULL == cn->prev)
        free(cn);

}

AssocChain* assocChain_append(AssocChain *cn, char *zone, Connection *con)
{
    AssocChain *tmp = cn;
    if (NULL == tmp->zone && NULL == tmp->connection) {
        tmp->zone = zone;
        tmp->connection = con;
        tmp->prev = NULL;
        tmp->next = NULL;
        return cn;
    }

    while (NULL != tmp->next) {
        tmp = tmp->next;
    }

    AssocChain *vl = newAssocChain();
    vl->zone = zone;
    vl->connection = con;
    vl->prev = tmp;
    vl->next = NULL;
    tmp->next = vl;

    return cn;
}

ZoneConfig* newZoneConfig()
{
    ZoneConfig *zc = calloc(1, sizeof (ZoneConfig));

    if (zc == NULL)
        fatal_exit("out of memory");

    return zc;
}

void freeZoneConfig(ZoneConfig *zn, bool preserveValues)
{
    if (NULL != zn->next) {
        freeZoneConfig(zn->next, preserveValues);
        free(zn->next);
    }

    if (!preserveValues && NULL != zn->ips)
        freeCharChain(zn->ips);

    if (!preserveValues && NULL != zn->name)
        free(zn->name);

    //		if(!preserveValues && NULL != zn->secure)
    //			free(zn->secure);

    if (NULL == zn->prev)
        free(zn);
}

ZoneConfig* zoneConfig_append(ZoneConfig *what, ZoneConfig *where)
{
    ZoneConfig *tmp = where;
    if (NULL == tmp->name && NULL == tmp->ips) {
        tmp->name = what->name;
        tmp->ips = what->ips;
        tmp->secure = what->secure;
        tmp->prev = NULL;
        tmp->next = NULL;

        freeZoneConfig(what, true);
        return where;
    }

    while (NULL != tmp->next) {
        tmp = tmp->next;
    }

    what->prev = tmp;
    tmp->next = what;

    return where;
}

LocalZoneConfig* newLocalZoneConfig()
{
    LocalZoneConfig *cn = calloc(1, sizeof (LocalZoneConfig));
    if (NULL == cn) {
        fatal_exit("out of memory");
    }
    return cn;
}

void freeLocalZoneConfig(LocalZoneConfig *cfg, bool preserveValues)
{
    if (NULL != cfg->next) {
        freeLocalZoneConfig(cfg->next, preserveValues);
        free(cfg->next);
    }

    if (!preserveValues && NULL != cfg->name)
        free(cfg->name);

    if (!preserveValues && NULL != cfg->type)
        free(cfg->type);

    if (NULL == cfg->prev)
        free(cfg);
}

LocalZoneConfig* localZoneConfig_append(LocalZoneConfig *what, LocalZoneConfig *where)
{
    LocalZoneConfig *tmp = where;
    if (NULL == tmp->name && NULL == tmp->type) {
        tmp->name = what->name;
        tmp->type = what->type;
        tmp->prev = NULL;
        tmp->next = NULL;

        freeLocalZoneConfig(what, true);
        return where;
    }

    while (NULL != tmp->next) {
        tmp = tmp->next;
    }

    what->prev = tmp;
    tmp->next = what;

    return where;
}

/**
 * Parses input json char* to ConnectionChain
 * @Example //TODO:
 *
 * @param input
 * @return ConnectionChain*
 */
ConnectionChain* parse_connections(char* input)
{
    if (!input || !json_validate(input)) {
        return NULL;

    }
    JsonNode *head = json_decode(input);
    //shouldn't be null because we checked if it is valid
    if (head->tag != JSON_OBJECT) {
        return NULL;

    }

    JsonNode *node = head->children.head; // now it should be the first dictionary value e.g. connections

    if (!node || strcmp(node->key, "connections") != 0) { // and also must be array
        return NULL;

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
                con->default_con = param->bool_; // hope it copies the value
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
                        fatal_exit("out of memory");
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
                        fatal_exit("out of memory");
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
    }

    json_delete(head);
    return cons;
}

void string_list_init(struct string_list* list)
{
	list->first = NULL;
}

void string_list_clear(struct string_list* list)
{
	struct string_entry* iter = list->first;
	while (NULL != iter) {
		struct string_entry* node = iter;
		iter = node->next;
		free(node->string);
		free(node);
	}
}

static void* calloc_or_die(size_t size) {
	void* mem = calloc(1, size);
	if (NULL == mem){
		fatal_exit("out of memory");
	} else {
		return mem;
	}
}

void string_list_push_back(struct string_list* list, char* new_value, size_t buffer_size)
{
	size_t len = strnlen(new_value, buffer_size);
	struct string_entry** node = &list->first;

	while (NULL != *node) {
		node = &(*node)->next;
	}

	*node = (struct string_entry*) calloc_or_die(sizeof(struct string_entry));
	(*node)->length = len;
	(*node)->string = (char*) calloc_or_die(len+1);
	strncpy((*node)->string, new_value, len);
}
