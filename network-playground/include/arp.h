/**
 * @file arp.h
 *
 * $Id:$
 */
/* Author: Drew Vanderwiel  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#ifndef _ARP_H_
#define _ARP_H_

#include <network.h>
#include <ether.h>

/* Arp table defines */
#define ARP_TABLE_LEN 32
#define ARP_ENT_INVALID 0   /** Entry is empty/invalid */
#define ARP_ENT_VALID   1   /** Entry has an IP addr and mac */
#define ARP_ENT_IP_ONLY 2   /** Entry has an IP addr but no mac */

/** ARP table entry contents */
struct arpEntry
{
    uchar   ipAddr[IP_ADDR_LEN];
    uchar   hwAddr[ETH_ADDR_LEN];
    ushort  osFlags;
};

/** ARP header entry contents */
struct arpPkt
{
    ushort hwType;              
    ushort prType;              
    uchar hwAddrLen;               
    uchar prAddrLen;               
    ushort op;                  
    uchar addrs[1]; 
};

extern struct arpEntry arp_t[ARP_TABLE_LEN];    /** ARP table */
extern semaphore arp_tsema;                     /** ARP table semaphore */
extern int       arpDaemonId;                   /** ARP daemon id */

/** ARP initialization */
syscall arpInit(void);

/** ARP daemon process */
void arpDaemon(void);

//syscall arpRecv(struct packet *);
//syscall arpSendRqst(struct arpEntry *);
//syscall arpResolve(uchar *);

//syscall arpSendReply(struct packet *);
//syscall arpLookup(struct netif *, struct netaddr *, struct netaddr *);
//syscall arpNotify(struct arpEntry *, message);
//struct arpEntry *arpAlloc(void);
//syscall arpFree(struct arpEntry *);
//struct arpEntry *arpGetEntry(struct ipaddr *);

#endif                          /* _ARP_H_ *//**< procedure*/
