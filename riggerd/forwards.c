/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "cfg.h"
#include "log.h"
#include "svr.h"
#include "probe.h"
#include "ubhook.h"
#include "forwards_data_types_tools.h"
#include "forwards_data_types.h"


ConnectionChain* copy_ConnectionChain(ConnectionChain *cn) {

    if (cn == NULL)
        return NULL;

    ConnectionChain *toRet = newConnectionChain();


    ConnectionChain *nxt = NULL;
    if (cn->next != NULL) {
        nxt = copy_ConnectionChain(cn->next);
    }
    toRet->next = nxt;

    if (cn->current != NULL) {
        toRet->current = copy_Connection(cn->current);
    }

    if (toRet->next != NULL) {
        toRet->next->prev = toRet;
    }
    return toRet;
}

static ConnectionChain* no_wifi(ConnectionChain *cn) {
    ConnectionChain *top = cn;
    ConnectionChain *i = cn;
    while (i != NULL) {
        if (i->current->type == WIFI) {
            if (i->prev != NULL)
                i->prev->next = i->next;
            if (i->next != NULL)
                i->next->prev = i->prev;
            if (i->next != NULL || i->prev != NULL) {// means that cell isn't the only one in chain
                if (i->prev == NULL)
                    top = i->next;
                ConnectionChain *tmp = i->next;
                freeConnectionChainCell(i);
                i = tmp;
                continue;
            } else {
                if (i->current != NULL) {
                    freeConnection(i->current);
                    i->current = NULL;
                }
            }
        }
        i = i->next;
    }

    return top;
}

static ConnectionChain* only_VPN(ConnectionChain *cn) {
    ConnectionChain *top = cn;
    ConnectionChain *i = cn;
    while (i != NULL) {
        if (i->current->type != VPN) {
            if (i->prev != NULL)
                i->prev->next = i->next;
            if (i->next != NULL)
                i->next->prev = i->prev;
            if (i->next != NULL || i->prev != NULL) {// means that cell isn't the only one in chain
                if (i->prev == NULL)
                    top = i->next;
                ConnectionChain *tmp = i->next;
                freeConnectionChainCell(i);
                i = tmp;
                continue;
            } else {
                if (i->current != NULL) {
                    freeConnection(i->current);
                    i->current = NULL;
                }
            }
        }
        i = i->next;
    }

    return top;
}

static ConnectionChain* only_default_connections(ConnectionChain *cn) {
    ConnectionChain *top = cn;
    ConnectionChain *i = cn;

    while (i != NULL) {
        if (!i->current->default_con) {
            if (i->prev != NULL)
                i->prev->next = i->next;
            if (i->next != NULL)
                i->next->prev = i->prev;
            if (i->next != NULL || i->prev != NULL) {// means that cell isn't the only one in chain
                if (i->prev == NULL)
                    top = i->next;
                ConnectionChain *tmp = i->next;
                freeConnectionChainCell(i);
                i = tmp;
                continue;
            } else {
                if (i->current != NULL) {
                    freeConnection(i->current);
                    i->current = NULL;
                }
            }
        }
        i = i->next;
    }

    return top;
}

AssocChain* get_AssocChain_with_zone(AssocChain *cn, char *zone) {
    for (AssocChain *i = cn; i != NULL; i = i->next) {
        if (i->zone == NULL)
            continue;
        if (strcmp(i->zone, zone) == 0) {
            return i;
        }
    }
    return NULL;
}

static Connection* get_preferred_Connection(Connection *first, Connection *second) //todo: add IPv6 functionality of default_con
{
    if (second->type == VPN && first->type != VPN) {
        return second;
    }

    if (first->type == VPN && second->type != VPN) {
        return first;
    }

    if (second->default_con && !first->default_con) {
        return second;
    }

    if (first->default_con && !second->default_con) {
        return first;
    }

    return first;
}

AssocChain* get_zone_Connection_mapping(ConnectionChain *connections) {
    AssocChain *result = newAssocChain();
    for (ConnectionChain *i = connections; i != NULL; i = i->next) {
        //TODO:null handle
        for (CharChain *l = i->current->zones; l != NULL; l = l->next) {
            AssocChain *tmp = get_AssocChain_with_zone(result, l->current);
            if (tmp == NULL) {
                assocChain_append(result, l->current, i->current);
            } else {
                tmp->connection = get_preferred_Connection(tmp->connection, i->current);
            }
        }
    }
    return result;
}

void stored_zones_append_double(CharChain **head, char *value) {
    if (*head == NULL) {
        CharChain *chain = newCharChain();
        chain->current = value;
        *head = chain;
        return;
    }

    CharChain *tmp = *head;
    if (NULL == tmp->current) {
        tmp->current = value;
        tmp->prev = NULL;
        tmp->next = NULL;
        return;
    }

    while (NULL != tmp->next) {
        tmp = tmp->next;
    }

    CharChain *vl = newCharChain();
    vl->current = value;
    vl->prev = tmp;
    tmp->next = vl;

}

static void stored_zones_remove_double(CharChain **chain, char* zone) { //removes only the first occurrence
    for (CharChain **i = chain; *i != NULL;) {
        CharChain *entry = *i;

        if (strcmp(entry->current, zone) == 0) {
            *i = entry->next;
            freeCharChainCell(entry);
            return;
        }

        i = &entry->next;
    }
}

void update_global_forwarders(struct svr *svr, ConnectionChain *input_connections) {
    ConnectionChain *connections = copy_ConnectionChain(input_connections);
    
    if (svr->cfg->use_vpn_global_forwarders) { // note only vpn must return NULL if there's none
        connections = only_VPN(connections);
    }
    if (isEmpty(connections)) {
        if (connections != NULL)
            freeConnectionChain(connections, false); // no preserve values because it should be empty
        connections = copy_ConnectionChain(input_connections);

        connections = only_default_connections(connections);
    }

    if (isEmpty(connections)) {
        if (connections != NULL)
            freeConnectionChain(connections, false);
        log_info("No global forwarders for processing.");
        return;
    }

    CharChain *ips = newCharChain();
    for (ConnectionChain *i = connections; NULL != i; i = i->next) { // every ip is in Chain only once after this cycle
        if (NULL == i->current)
            continue;

        for (CharChain *ip = i->current->servers; NULL != ip; ip = ip->next) {
            if (NULL == ip->current)
                continue;
            if (valueInCharChain(ips, ip->current))
                continue;

            char *cpy = calloc(strlen(ip->current) + 1, sizeof (char));
            if (cpy == NULL)
                fatal_exit("out of memory");
            strcpy(cpy, ip->current);
            charChain_append(ips, cpy);
        }
    }


    if (!charChainsEqual(ips, svr->global_forwarders)) { // compare with previous forwarders
        if (NULL != svr->global_forwarders)
            freeCharChain(svr->global_forwarders);
        svr->global_forwarders = ips;

        char *ips_str = CharChain_to_string(ips);

        probe_start(ips_str);
        free(ips_str);

    } else {
        freeCharChain(ips);
        log_info("Global forwarders are same as the last one. None is processed.");
    }

    freeConnectionChain(connections, false);

}

void update_connection_zones(struct svr* svr, ConnectionChain *input_connections) {
    char **rfc1918_reverse_zones = (char *[]){"c.f.ip6.arpa", "d.f.ip6.arpa", "168.192.in-addr.arpa", "16.172.in-addr.arpa", "17.172.in-addr.arpa", "18.172.in-addr.arpa", "19.172.in-addr.arpa", "20.172.in-addr.arpa", "21.172.in-addr.arpa", "22.172.in-addr.arpa", "23.172.in-addr.arpa", "24.172.in-addr.arpa", "25.172.in-addr.arpa", "26.172.in-addr.arpa", "27.172.in-addr.arpa", "28.172.in-addr.arpa", "29.172.in-addr.arpa", "30.172.in-addr.arpa", "31.172.in-addr.arpa", "10.in-addr.arpa"}; // hope every string ends with NULL
    struct cfg* glob_cfg = svr->cfg;

    // we copy inputConnections to keep them untouched for another usage somewhere else and for correct deallocation
    ConnectionChain *connections = copy_ConnectionChain(input_connections);

    if (!glob_cfg->add_wifi_provided_zone) { // what if there's only wifi on list?
        connections = no_wifi(connections);
    }

    AssocChain *mappedConnections = get_zone_Connection_mapping(connections);
    ZoneConfig *unbound_zones = get_unbound_ZoneConfig(glob_cfg);
    bool in = false;
    bool not_in = true;


    // Remove any zones managed by dnssec-trigger that are no longer valid.

    //for(CharChain *c = stored_zones; c != NULL; c = c->next) {
    CharChain *c = svr->stored_zones;
    while (c != NULL) {
        // leave zones that are provided by some connection
        for (AssocChain *cn = mappedConnections; cn != NULL; cn = cn->next) {
            if (cn->zone == NULL || c->current == NULL)
                continue;
            if (strcmp(cn->zone, c->current) == 0) {
                in = true;
                break;
            }
        }

        if (in) {
            in = false;
            c = c->next;
            continue;
        }
        // ---------------------

        for (int i = 0; i < 20; i++) { // 20 is number of records in rfc array //todo: do it dynamic use arrayDelimiter to determine the end of the array
            char *zone = (char *) *(rfc1918_reverse_zones + i);

            if (c->current == NULL || zone == NULL)
                continue;
            if (strcmp(zone, c->current) == 0) {
                // if zone is private address range reverse zone and we are configured to use them, leave it
                if (glob_cfg->use_private_address_range) {
                    in = true;
                    break;
                } else {
                    // otherwise add Unbound local zone of type 'static' like Unbound does and remove it later
                    hook_unbound_local_zones_add(glob_cfg, c->current, "static");
                    // how about putting here break too?
                }
            }
        }

        if (in) {
            in = false;
            c = c->next;
            continue;
        }
        // --------------------
        // Remove all zones that are not in connections except OR
        // are private address ranges reverse zones and we are NOT
        // configured to use them

        for (ZoneConfig *ubd_zn = unbound_zones; ubd_zn != NULL; ubd_zn = ubd_zn->next) {
            if (ubd_zn->name == NULL || c->current == NULL)
                continue;
            if (strcmp(ubd_zn->name, c->current) == 0) {
                hook_unbound_zones_remove(glob_cfg, c->current);
                // how about putting here break too?
            }
        }
        CharChain *tmp = c->next; // now there's just one danger, we've removed next cell, but this case shouldn't appear because we should have been iterated on the same position which will be deleted
        stored_zones_remove_double(&svr->stored_zones, c->current); // danger situations we've removed next, we've removed current
        c = tmp;
    }
    // ------------------------
    // Install all zones coming from connections except those installed
    // by other means than dnssec-trigger-script.
    for (AssocChain *cn = mappedConnections; cn != NULL; cn = cn->next) {
        // Reinstall a known zone or install a new zone.
        for (CharChain *stored = svr->stored_zones; stored != NULL; stored = stored->next) {
            if (cn->zone == NULL || stored->current == NULL)
                continue;
            if (strcmp(cn->zone, stored->current) == 0) {
                in = true;
                break;
            }
        }

        if (!in) {
            for (ZoneConfig *ubd_zn = unbound_zones; ubd_zn != NULL; ubd_zn = ubd_zn->next) {
                if (ubd_zn->name == NULL || cn->zone == NULL)
                    continue;
                if (strcmp(cn->zone, ubd_zn->name) == 0) {
                    not_in = false;
                    break;
                }
            }
        }

        if (in || not_in) {
            char *servers = CharChain_to_string(cn->connection->servers);
            hook_unbound_zones_add(glob_cfg, cn->zone, servers, glob_cfg->validate_connection_provided_zones); // maybe should ensure every server is there only once in chain
            free(servers);
            char *zn = calloc(strlen(cn->zone) + 1, sizeof (char));
            if (zn == NULL)
                fatal_exit("out of memory");
            strcpy(zn, cn->zone);
            stored_zones_append_double(&svr->stored_zones, zn);
        }
        in = false;
        not_in = true;
    }
    // ---------------------------

    in = false;
    not_in = true;

    // Configure forward zones for reverse name resolution of private addresses.
    // RFC1918 zones will be installed, except those already provided by connections
    // and those installed by other means than by dnssec-trigger-script.
    // RFC19118 zones will be removed if there are no global forwarders.
    if (glob_cfg->use_private_address_range) {
        for (int i = 0; i < 20; i++) { //todo: use arrayDelimiter to determine the end of the array
            char *zone = (char *) *(rfc1918_reverse_zones + i);
            // Ignore a connection provided zone as it's been already processed.
            for (AssocChain *con = mappedConnections; con != NULL; con = con->next) {
                if (zone == NULL || con->zone == NULL)
                    continue;
                if (strcmp(zone, con->zone) == 0) {
                    continue;
                }
            }

            if (!isEmptyCharChain(svr->global_forwarders)) {
                // Reinstall a known zone or install a new zone.
                for (CharChain *stored = svr->stored_zones; stored != NULL; stored = stored->next) {
                    if (zone == NULL || stored->current == NULL)
                        continue;
                    if (strcmp(zone, stored->current) == 0) {
                        in = true;
                        break;
                    }
                }

                not_in = true;
                if (!in) {
                    for (ZoneConfig *ubd_zn = unbound_zones; ubd_zn != NULL; ubd_zn = ubd_zn->next) {
                        if (zone == NULL || ubd_zn->name == NULL)
                            continue;
                        if (strcmp(zone, ubd_zn->name) == 0) {
                            not_in = false;
                            break;
                        }
                    }
                }

                if (in || not_in) {
                    char *servers = CharChain_to_string(svr->global_forwarders);
                    hook_unbound_zones_add(glob_cfg, zone, servers, false);
                    free(servers);
                    char *zn = calloc(strlen(zone) + 1, sizeof (char));
                    if (zn == NULL)
                        fatal_exit("out of memory");
                    strcpy(zn, zone);
                    stored_zones_append_double(&svr->stored_zones, zn);
                    hook_unbound_local_zones_remove(glob_cfg, zone);
                }
                in = false;
                not_in = true;

            } else {
                // There are no global forwarders, therefore remove the zone
                //CharChain *stored = stored_zones;
                for (CharChain *stored = svr->stored_zones; stored != NULL; stored = stored->next) {
                    //while(stored != NULL) {
                    if (zone == NULL || stored->current == NULL) {
                        continue;
                    }

                    if (strcmp(zone, stored->current) == 0) {
                        //CharChain *tmp = stored_zones->next;
                        stored_zones_remove_double(&svr->stored_zones, zone);
                        //	stored = tmp;
                        //stored_zones_remove(zone);
                        break;
                    }
                }

                for (ZoneConfig *ubd_zn = unbound_zones; ubd_zn != NULL; ubd_zn = ubd_zn->next) {
                    if (zone == NULL || ubd_zn->name == NULL)
                        continue;
                    if (strcmp(zone, ubd_zn->name) == 0) {
                        hook_unbound_zones_remove(glob_cfg, zone);
                        break;
                    }
                }
                hook_unbound_local_zones_add(glob_cfg, zone, "static");

            }
        }
    }

    freeAssocChain(mappedConnections, true); // preserve values because for mapping we used values from other chain and didn't copy them
    if(unbound_zones != NULL) // unbound_zones may be null in case the unbound-control for getting them didn't work
        freeZoneConfig(unbound_zones, false);
    freeConnectionChain(connections, false);

}

