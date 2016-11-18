/**
 * @file arpSendRequest.c
 * @provides TODO 
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <arp.h>

/**
 * Send an ARP request to the provided IPv4 address
 * to get the target's coresponding mac address
 * @param ipAddr IPv4 address target
 * @return OK for success, SYSERR for syntax error
 */
syscall arpSendRequest(uchar *ipAddr)
{
    int i;
    struct ethergram    *egram = NULL;
    struct arpPkt       *arpP = NULL;
    char                buf[ARP_PKTSIZE];
    
    if (ipAddr == NULL)
    {
        return SYSERR;
    }    
    
    /* Set up Ethergram header */
    egram = (struct ethergram *) buf;
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->dst[i] = 0xFF;
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->src[i] = net.hwAddr[i];
    
    egram->type = htons(ETYPE_ARP);
    
    
    /* Set up Arp header */
    arpP = (struct arpPkt *) &egram->data;
    
    arpP->hwType = htons(ARP_HWTYPE_ETHERNET);
    arpP->prType = htons(ARP_PRTYPE_IPv4);
    arpP->hwAddrLen = ETH_ADDR_LEN;
    arpP->prAddrLen = IP_ADDR_LEN;
    arpP->op = htons(ARP_OP_RQST);
    
    /* Set up Arp addresses */
    
    // Source hw addr
    for (i = 0; i < ETH_ADDR_LEN; i++)
        arpP->addrs[i + ARP_SHA_OFFSET] = net.hwAddr[i];
    
    // Source protocol addr
    for (i = 0; i < IP_ADDR_LEN; i++)
        arpP->addrs[i + ARP_SPA_OFFSET] = net.ipAddr[i];
    
    // Dest hw addr
    for (i = 0; i < ETH_ADDR_LEN; i++)
        arpP->addrs[i + ARP_DHA_OFFSET] = 0x00;
    
    // Dest protocol addr
    for (i = 0; i < IP_ADDR_LEN; i++)
        arpP->addrs[i + ARP_DPA_OFFSET] = ipAddr[i];
    
    /* Send packet */
    write(ETH0, (uchar *)buf, PKTSZ);

    return OK;
}
