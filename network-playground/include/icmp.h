/**
 * @file icmp.h
 *
 * $Id:$
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/28/2016       */

#ifndef _ICMP_H_
#define _ICMP_H_

#include <network.h>
#include <ether.h>

/* ICMP message Types */
#define ICMP_ECHO_RQST_T 0x08
#define ICMP_ECHO_RPLY_T 0x00

/* ICMP message codes */
#define ICMP_ECHO_RQST_C  0
#define ICMP_ECHO_RPLY_C  0

/* ICMP header length */
#define ICMP_HEADER_LEN 8

/* ICMP packet size */
#define ICMP_PKTSIZE ETHER_MINPAYLOAD + ETH_HEADER_LEN

/* ICMP Table defines */
#define ICMP_TBL_LEN          1  // May be lengthed later
#define ICMP_TBL_INIT_PID    -1
#define ICMP_ENTRY_INVALID 0x00
#define ICMP_RQST_SENT     0x01
#define ICMP_GOT_RPLY      0x02

struct icmpTblEntry
{
    int pid;
    int sema;
    uchar flag;
    ushort seqNum;
    uchar ipAddr[IPv4_ADDR_LEN];
}

extern struct icmpTblEntry icmpTbl[ICMP_TBL_LEN];

/*
 * ICMP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Type (8 bits) | Code (8 bits)  |      Checksum (16 bits)      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |            Identifier          |      Sequence Number         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          Data Section                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
 
/** ICMP header entry contents */
struct icmpPkt
{
    uchar type;
    uchar code;
    ushort chksum;
    ushort id;
    ushort seqNum;
    uchar data[1]; 
};

/** ICMP table initializations */
syscall icmpInit(void);

/** Receive ICMP echo requests and replies (used by netDaemon) **/
syscall icmpRecv(struct ipgram *, uchar *);

/** Handle an ICMP echo request (used by netDaemon) **/
syscall icmpHandleRequest(struct ipgram *, uchar *);

/** Handle an ICMP echo reply (used by netDaemon) **/
syscall icmpHandleReply(struct ipgram *);

/** Send an ICMP echo request (used by ping) */
syscall icmpSendRequest(uchar *ipAddr, 
                        uchar *hwAddr, 
                        ushort id,
                        ushort seqNum);

#endif                          /* _ICMP_H_ */
