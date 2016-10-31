/**
 * @file icmp.c
 * @provides icmpRecv, icmpHandleRequest, and icmpHandleReply
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

/* Global ICMP table definition */
struct icmpTblEntry icmpTbl[ICMP_TBL_LEN];


/**
 * Initialize the ICMP table
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpInit(void)
{
    int i, j;
    
    /* Initialize the ICMP table entries */
    for (i = 0; i < ICMP_TBL_LEN; i++)
    {
        icmpTbl[i].pid = ICMP_TBL_INIT_PID;
        icmpTbl[i].sema = semcreate(1);
        icmpTbl[i].flag = ICMP_ENTRY_INVALID;
        icmpTbl[i].id = 0;
        icmpTbl[i].seqNum = 0;
        
        for (j = 0; j < IPv4_ADDR_LEN; j++)
            icmpTbl[i].ipAddr[j] = 0;
    }
    
    return OK;
}


/**
 * Receive and filter ICMP Packets
 * @param ipPkt   received IPv4 packet
 * @param srcAddr Sender MAC address
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpRecv(struct ipgram *ipPkt, uchar *srcAddr)
{
    int i, eqFlag;
    struct icmpPkt *pkt;
    ushort origChksum, calChksum;
     
    if (ipPkt == NULL || srcAddr == NULL)
        return SYSERR;
    
    // TODO start after the ip packets options, at its data
    pkt = (struct icmpPkt *) ipPkt->opts;
    
    // Screen out packets with bad ICMP headers
    if ( pkt->type != ICMP_ECHO_RQST_T ||
         pkt->type != ICMP_ECHO_RPLY_T ||
         pkt->code != ICMP_ECHO_RQST_C )
        return SYSERR;
    
    // Screen out packets with a bad ICMP checksums
    origChksum = pkt->chksum;
    pkt->chksum = 0;
    calChksum = checksum((void *) pkt, ICMP_HEADER_LEN);
    
    printf("ICMP Recv Orig checksum: %04x calc'd: %04x", origChksum, calChksum);
    
    if (calChksum != origChksum)
        return SYSERR;
    
    // Handle the ICMP packet
    if ( pkt->type == ICMP_ECHO_RQST_T)
    {
        return icmpHandleRequest(ipPkt, srcAddr);
    }
    else if ( pkt->type == ICMP_ECHO_RPLY_T )
    {
        return icmpHandleReply(ipPkt);
    }
    
    return OK;
}


/**
 * Handle ICMP Echo request Packets
 * @param ipPkt   received IPv4 packet
 * @param srcAddr Sender MAC address
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpHandleRequest(struct ipgram *ipPkt, uchar *srcAddr)
{
    int i;
    struct ethergram    *egram = NULL;
    struct ipgram       *ipP = NULL;
    struct icmpPkt      *icmpPRecvd = NULL;
    struct icmpPkt      *icmpP = NULL;
    char                buf[ICMP_PKTSIZE];
    
    
    /* Set up Ethergram header */
    egram = (struct ethergram *) buf;
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->dst[i] = srcAddr[i];
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->src[i] = arp.hwAddr[i];
    
    egram->type = htons(ETYPE_IPv4);
    
    
    /* Set up IPv4 header */
    ipP = (struct ipgram *) &egram->data;
    
    // Version 5, IHL size 5 * (4 byte words) = 20
    ipP->ver_ihl = 0x45;
    ipP->tos = IPv4_TOS_ROUTINE;
    ipP->len = IPv4_HDR_LEN + ICMP_HEADER_LEN;
    
    // Get id from request packet
    ipP->id = ntohs(ipPkt->id);
    ipP->flags_froff = 0;
    ipP->ttl = IPv4_TTL;
    ipP->proto = IPv4_PROTO_ICMP;
    ipP->chksum = 0;
    
    // Source protocol addr (ours)
    for (i = 0; i < IPv4_ADDR_LEN; i++)
        ipP->src[i] = arp.ipAddr[i];
    
    // Dest protocol addr (requester's)
    for (i = 0; i < IPv4_ADDR_LEN; i++)
        ipP->dst[i] = ipPkt->src[i];
    
    // Calculate the IP header checksum
    ipP->chksum = htons(checksum((void *) ipP, IPv4_HDR_LEN));
    ipP->len = htons(IPv4_HDR_LEN + ICMP_HEADER_LEN);
    ipP->id = ipPkt->id;
    
    
    /* Set up ICMP header */
    icmpPRecvd = (struct icmpPkt *) &ipPkt->opts;
    icmpP = (struct icmpPkt *) &ipP->opts;
    
    icmpP->type = ICMP_ECHO_RPLY_T;
    icmpP->code = ICMP_ECHO_RPLY_C;
    icmpP->chksum = 0;
    icmpP->id = ntohs(icmpPRecvd->id);
    icmpP->seqNum = ntohs(icmpPRecvd->seqNum);
    
    // Calculate the ICMP header checksum
    icmpP->chksum = htons(checksum((void *) icmpP, ICMP_HEADER_LEN));
    icmpP->id = icmpPRecvd->id;
    icmpP->seqNum = icmpPRecvd->seqNum;
    
    
    /* Send packet */
    write(ETH0, (uchar *)buf, ICMP_PKTSIZE);
    
    return OK;
}


/**
 * Handle ICMP Echo reply Packets
 * @param ipPkt   received IPv4 packet
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpHandleReply(struct ipgram *ipPkt)
{
    int i, ipEqual = 0;
    ushort id;
    ushort seqNum;
    struct icmpPkt      *icmpPRecvd = NULL;
    message msg;
    
    icmpPRecvd = (struct icmpPkt *) &ipPkt->opts;
    id = ntohs(icmpPRecvd->id);
    seqNum = ntohs(icmpPRecvd->seqNum);
    
    // Make sure the id is within the table's range
    if (id < ICMP_TBL_LEN)
    {
        // Grab semaphore
        wait(icmpTbl[id].sema);
        
        // Is the ICMP table entry valid and does
        // it match the reply we got?
        if ( ICMP_RQST_SENT     == icmpTbl[id].flag &&
             ICMP_TBL_INIT_PID  != icmpTbl[id].pid &&
             seqNum             == icmpTbl[id].seqNum )
        {
            // Check if the IP address matches
            ipEqual = 1;
            for (i = 0; i < IPv4_ADDR_LEN; i++)
            {
                if (icmpTbl[id].ipAddr[i] != ipPkt->src[i])
                {
                    ipEqual = 0;
                    break;
                }
            }
            
            // Set the flag, since we got an ICMP reply
            if (ipEqual)
                icmpTbl[id].flag = ICMP_GOT_RPLY;
        }
        // Give back the semaphore
        signal(icmpTbl[id].sema);
    }
    
    
    // If this is true, we have received a good ICMP reply
    if (ipEqual)
    {
        // Send a message to the waiting process
        msg = (message) ntohs(ipPkt->len);
        send(icmpTbl[id].pid, msg);
    }
    return OK;
}

