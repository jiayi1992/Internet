/**
 * @file icmpSendRequest.c
 * @provides icmpSendRequest
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/29/2016       */

#include <xinu.h>
#include <network.h>
#include <ether.h>
#include <icmp.h>
#include <arp.h>


/**
 * Send an ICMP request
 * @param ipAddr Destination IPv4 address
 * @param hwAddr Destination MAC address
 * @param id     ICMP identifier, used to index ICMP buffer
 * @param seqNum ICMP sequence number
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpSendRequest(uchar *ipAddr, 
                        uchar *hwAddr, 
                        ushort id,
                        ushort seqNum)
{
    int i, helperID;
    struct ethergram    *egram = NULL;
    struct icmpPkt       *icmpP = NULL;
    struct ipgram       *ipP = NULL;
    //ulong               *time;
    char                buf[ICMP_PKTSIZE];
    message             msg;
    
    if (ipAddr == NULL)
    {
        return SYSERR;
    }
    
    /* Set up Ethergram header */
    egram = (struct ethergram *) buf;
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->dst[i] = hwAddr[i];
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->src[i] = arp.hwAddr[i];
    
    egram->type = htons(ETYPE_IPv4);
    
    /* Set up IPv4 header */
    ipP = (struct ipgram *) &egram->data;
    
    // Version 5, IHL size 5 * (4 byte words) = 20
    ipP->ver_ihl = 0x45;      
    ipP->tos = IPv4_TOS_ROUTINE;
    // Add 4 bytes for the ICMP current time data
    ipP->len = htons(IPv4_HDR_LEN + ICMP_HEADER_LEN + 4);
    ipP->id = htons(id);
    ipP->flags_froff = 0;
    ipP->ttl = IPv4_TTL;
    ipP->proto = IPv4_PROTO_ICMP;
    ipP->chksum = 0x0000;
    
     // Source protocol addr (ours)
    for (i = 0; i < IP_ADDR_LEN; i++)
        ipP->src[i] = arp.ipAddr[i];
    
    // Dest protocol addr (requester's)
    for (i = 0; i < IP_ADDR_LEN; i++)
        ipP->dst[i] = ipAddr[i];
    
    ipP->chksum = checksum((void *) ipP, IPv4_HDR_LEN);
    
    /* Set up ICMP header */
    icmpP = (struct icmpPkt *) &ipP->opts;

    icmpP->type = ICMP_ECHO_RQST_T;
    icmpP->code = ICMP_ECHO_RQST_C;
    icmpP->chksum = 0x0000;
    icmpP->id = htons(id);
    icmpP->seqNum = htons(seqNum);
    
    //printf("icmpSendRequest 1\n");
    
    // Store the current time as the ICMP packet data
    //time = (ulong *) &(icmpP->data);
    //*time = htons(clocktime);
    
    //printf("icmpSendRequest 2\n");
    
    // Calculate the checksum
    icmpP->chksum = checksum((void *) icmpP, ICMP_HEADER_LEN);
    
    // Grab semaphore
    wait(icmpTbl[id].sema);
    
    write(ETH0, (uchar *)buf, ICMP_PKTSIZE);
    
    // Update icmpTbl entry
    icmpTbl[id].pid = getpid();
    icmpTbl[id].flag = ICMP_RQST_SENT;
    icmpTbl[id].seqNum = seqNum;
    
    for (i = 0; i < IP_ADDR_LEN; i++)
        icmpTbl[id].ipAddr[i] = ipAddr[i];
    
    // Give back the arp semaphore
    signal(icmpTbl[id].sema);
    
    msg = recvtime(1000);
    
    if (msg == TIMEOUT || (int) msg == 0)
    {
        return SYSERR;
    }
    
    return (syscall) msg;
}
