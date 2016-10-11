/**
 * @file arp.h
 *
 * $Id:$
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#ifndef _ARP_H_
#define _ARP_H_

#include <network.h>
#include <ether.h>

/* ARP Hardware Types */
#define ARP_HWTYPE_ETHERNET     1
/* ARP Protocol Types */
#define ARP_PRTYPE_IPv4         ETYPE_IPv4  
/* ARP Operations */
#define ARP_OP_RQST             1
#define ARP_OP_REPLY            2
/* ARP Header */
#define ARP_CONST_HDR_LEN       8

/* ARP table defines */
#define ARP_TABLE_LEN 32
#define ARP_ENT_NOT_FOUND -1    /** Entry could not be found */
#define ARP_ENT_INVALID 0       /** Entry is empty/invalid */
#define ARP_ENT_VALID   1       /** Entry has an IP addr and mac */
#define ARP_ENT_IP_ONLY 2       /** Entry has an IP addr but no mac */
#define ARP_ENT_DEFAULT_TIMEOUT 300 /** Timeout in seconds **/

/* ARP address offsets */
#define ARP_SHA_OFFSET 0
#define ARP_SPA_OFFSET ETH_ADDR_LEN
#define ARP_DHA_OFFSET ETH_ADDR_LEN + IP_ADDR_LEN
#define ARP_DPA_OFFSET ETH_ADDR_LEN*2 + IP_ADDR_LEN
#define ARP_ADDR_END_OFFSET ETH_ADDR_LEN*2 + IP_ADDR_LEN*2

/* ARP packet size */
#define ARP_PKTSIZE ETHER_MINPAYLOAD + ETH_HEADER_LEN

/* maximum ARP resolve attempts */
#define ARP_RESOLVE_ATTEMPTS 3

/** ARP table entry contents */
struct arpEntry
{
    uchar   ipAddr[IP_ADDR_LEN];
    uchar   hwAddr[ETH_ADDR_LEN];
    ushort  osFlags;
    ushort  timeout;
};

/*
 * ARP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Link-Level Header                                             |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |        Hardware Type          |         Protocol Type         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Hardware Len | Protocol Len   |          Operation            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source Hardware Address (SHA)                                 |
 * | Source Protocol Address (SPA)                                 |
 * | Destination Hardware Address (DHA)                            |
 * | Destination Protocol Address (DPA)                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
 
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

struct arpTable
{
    struct arpEntry     tbl[ARP_TABLE_LEN];                 /** ARP table */
    semaphore           sema;                               /** ARP table semaphore */
    int                 freeEnt;                            /** Index to next free ARP entry */
    int                 victimEnt;                          /** Next victim if ARP table full */
    int                 dId;                                /** ARP daemon id */
    int                 wId;                                /** ARP table watcher id */
    uchar               ipAddr[IP_ADDR_LEN];                /** This host's IP address */
    uchar               hwAddr[ETH_ADDR_LEN];               /** This host's mac address */
};

extern struct arpTable arp;

/** ARP initialization */
syscall arpInit(void);

/** ARP daemon process */
void arpDaemon(void);

/** ARP table watcher */
void arpWatcher(void);

/** ARP request, reply, and receive **/
syscall arpSendRequest(uchar *);
syscall arpSendReply(struct arpPkt *);
syscall arpRecv(struct arpPkt *);

/** Resolving mac address from an IP **/
syscall arpResolve(uchar *ipAddr, uchar *hwAddr);

/** ARP Table manipulation **/
syscall arpAddEntry(uchar *ipAddr, uchar *hwAddr);
int arpFindEntry(uchar *ipAddr);

#endif                          /* _ARP_H_ */
