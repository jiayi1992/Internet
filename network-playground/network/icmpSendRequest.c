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
    // TODO
    int i;
    struct ethergram    *egram = NULL;
	struct icmpPk       *icmpP = NULL;
	struct ipgram       *ipP = NULL;
	char                buf[ICMP_PKTSIZE];
    
    if (ipAddr == NULL)
    {
        return SYSERR;
    }
	
	/* Set up Ethergram header */
	egram = (struct ethergram *) buf;
	
	for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->dst[i] = 0xFF;
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->src[i] = arp.hwAddr[i];
	
	egram->type = htons(ETYPE_IPv4);
	
	/* Set up IP header */
	ipP = (struct ipgram *) &egram->data;
	
	ipP->ver_ihl = 0x45;      
	ipP->tos = IPv4_TOS_ROUTINE;
	ipP->len = IPv4_HDR_LEN + ICMP_HEADER_LEN;
	ipP->id = id;                 //Not sure
	ipP->flags_froff = IPv4_FLAG_LF;    //Not sure
	ipP->ttl = IPv4_TTL;
	ipP->proto = IPv4_PROTO_ICMP;
	ipP->chksum = 0x0000;
	
	for (i = 0; i < IP_ADDR_LEN; i++)
        ipP->src[i] = arp.ipAddr[i];
	
	for (i = 0; i < IP_ADDR_LEN; i++)
        ipP->dst[i] = ipAddr[i];
	
	ipP->chksum = hecksum((void *)ipP, IPv4_HDR_LEN);
	
	/* Set up IP header */
	icmpP = (struct icmpPk *) &ipP->opts;
	
	icmpP->type = ICMP_ECHO_RQST_T;
	icmpP->code = ICMP_ECHO_RQST_C;
	icmpP->chksum = 0x0000;
	icmpP->id = id;
	icmpP->sequNum = seqNum;
	
	icmpP->chksum = checksum((void *)icmpP, ICMP_HEADER_LEN);
	
	write(ETH0, (uchar *)buf, ICMP_PKTSIZE);
	
    return OK;
}

