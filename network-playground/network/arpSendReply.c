/**
 * @file arpSendReply.c
 * @provides TODO 
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <arp.h>

/**
 * Send an ARP reply using the information contained in the packet
 * @param pkt
 * @return OK for success, SYSERR for syntax error
 */
syscall arpSendReply(struct arpPkt *recvdPkt)
{
    int i;
    struct ethergram    *egram = NULL;
    struct arpPkt       *arpP = NULL;
    char                buf[ARP_PKTSIZE];
    /* int                 bytes; */
    
    if (recvdPkt == NULL)
    {
        return SYSERR;
    }
    
    /* Set up Ethergram header */
    egram = (struct ethergram *) buf;
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->dst[i] = recvdPkt->addrs[i + ARP_SHA_OFFSET];
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->src[i] = arp.hwAddr[i];
    
    egram->type = htons(ETYPE_ARP);
    
    
    /* Set up Arp header */
    arpP = (struct arpPkt *) &egram->data;
    
    arpP->hwType = htons(ARP_HWTYPE_ETHERNET);
    arpP->prType = htons(ARP_PRTYPE_IPv4);
    arpP->hwAddrLen = ETH_ADDR_LEN;
    arpP->prAddrLen = IP_ADDR_LEN;
    arpP->op = htons(ARP_OP_RQST);
    
    /* Set up Arp addresses */
    
    // Source hw addr (ours)
    for (i = 0; i < ETH_ADDR_LEN; i++)
        arpP->addrs[i + ARP_SHA_OFFSET] = arp.hwAddr[i];
    
    // Source protocol addr (ours)
    for (i = 0; i < IP_ADDR_LEN; i++)
        arpP->addrs[i + ARP_SPA_OFFSET] = arp.ipAddr[i];
    
    // Dest hw addr (requester's)
    for (i = 0; i < ETH_ADDR_LEN; i++)
        arpP->addrs[i + ARP_DHA_OFFSET] = recvdPkt->addrs[i + ARP_SHA_OFFSET];
    
    // Dest protocol addr (requester's)
    for (i = 0; i < IP_ADDR_LEN; i++)
        arpP->addrs[i + ARP_DPA_OFFSET] = recvdPkt->addrs[i + ARP_SPA_OFFSET];
    
    /* Send packet */
    /* bytes = */ write(ETH0, (uchar *)buf, PKTSZ);

return OK;
}
