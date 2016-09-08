/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   forwards.h
 * Author: fcap
 *
 * Created on 6. září 2016, 12:36
 */

#ifndef FORWARDS_H
#define FORWARDS_H

#include "forwards_data_types.h"

void update_global_forwarders(struct svr* svr, ConnectionChain *connections);

void update_connection_zones(struct svr* svr, ConnectionChain *connections);

#endif /* FORWARDS_H */

