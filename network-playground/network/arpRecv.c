/**
 * @file arp.c
 * @provides arp table 
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <arp.h>

void arpRecvDebug(struct arpPkt *pkt);

/**
 * Handle arp requests and replies
 * @param pkt received ARP packet
 * @return OK for success, SYSERR for syntax error
 */
syscall arpRecv(struct arpPkt *pkt)
{
    if (pkt == NULL)
        return SYSERR;

    // Screen out packets with bad ARP headers
    if ( pkt->hwAddrLen != ETH_ADDR_LEN ||
         pkt->prAddrLen != IP_ADDR_LEN ||
         ntohs(pkt->hwType) != ARP_HWTYPE_ETHERNET ||
         ntohs(pkt->prType) != ARP_PRTYPE_IPv4 )
        return SYSERR;
 
    switch(ntohs(pkt->op))
    {
    /*************************/
    /** Handle ARP Request **/
    /*************************/
    case ARP_OP_RQST:
        printf("Got ARP request\n");
        /* TODO: Put the requester's info into the arp table? */
        
        arpRecvDebug(pkt);
        
        // If the destination protocol address of the packet
        // is equal to our ip address, then send an arp reply
        if ( ipEq(&pkt->addrs[ARP_DPA_OFFSET],&arp.ipAddr) )
            arpSendReply(pkt);
        break;
        
    /*************************/
    /** Handle ARP Reply **/
    /*************************/
    case ARP_OP_REPLY:
        printf("Got ARP reply\n");
        
        arpRecvDebug(pkt);
        
        // If they are replying to our request, then add their info to our ARP table
        if ( ipEq(&pkt->addrs[ARP_DPA_OFFSET],&arp.ipAddr) )
            arpAddEntry(&pkt->addrs[ARP_SPA_OFFSET], &pkt->addrs[ARP_SHA_OFFSET]);
        break;
        
    default:
        break;
    }
    
    return OK;
}


void arpRecvDebug(struct arpPkt *pkt)
{
    int j;
    
    // Print source mac addr
    for (j = 0; j < ETH_ADDR_LEN-1; j++)
        printf("%x:",pkt->addrs[j + ARP_SHA_OFFSET]);
    printf("%x\n",pkt->addrs[ARP_SPA_OFFSET - 1]);

    // Print source protocol addr
    for (j = 0; j < IP_ADDR_LEN - 1; j++)
        printf("%d.",pkt->addrs[j + ARP_SPA_OFFSET]);
    printf("%d\n",pkt->addrs[ARP_DHA_OFFSET - 1]);

    // Print dest hw addr
    for (j = 0; j < ETH_ADDR_LEN - 1; j++)
        printf("%x:",pkt->addrs[j + ARP_DHA_OFFSET]);
    printf("%x\n",pkt->addrs[ARP_DPA_OFFSET - 1]);

    // Print dest protocol addr
    for (j = 0; j < IP_ADDR_LEN - 1; j++)
        printf("%d.",pkt->addrs[j + ARP_DPA_OFFSET]);
    printf("%d\n",pkt->addrs[ARP_ADDR_END_OFFSET - 1]);
    
    return;
}