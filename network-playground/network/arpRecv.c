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


/**
 * Handle arp requests and replies
 * @param pkt received ARP packet
 * @return OK for success, SYSERR for syntax error
 */
syscall arpRecv(struct arpPkt *pkt)
{
    int i, eqFlag;
     
    if (pkt == NULL)
        return SYSERR;
    
    // Screen out packets with bad ARP headers
    if ( pkt->hwAddrLen != ETH_ADDR_LEN ||
         pkt->prAddrLen != IP_ADDR_LEN ||
         ntohs(pkt->hwType) != ARP_HWTYPE_ETHERNET ||
         ntohs(pkt->prType) != ARP_PRTYPE_IPv4 ||
         ((ntohs(pkt->op) != ARP_OP_REPLY) && 
         (ntohs(pkt->op) != ARP_OP_RQST))
         )
        return SYSERR;
    
    // Screen out packets not addressed to us
    eqFlag = OK;
    for (i = 0; i < IP_ADDR_LEN; i++)
    {
        if (pkt->addrs[i + ARP_DPA_OFFSET] != arp.ipAddr[i])
        {
            eqFlag = SYSERR;
            break;
        }
    }
        
    if (eqFlag == SYSERR)
        return OK;
    
    // Add the sender to our arp table
    arpAddEntry(&pkt->addrs[ARP_SPA_OFFSET], &pkt->addrs[ARP_SHA_OFFSET]);  
    
    if (ntohs(pkt->op) == ARP_OP_RQST)
    {
        arpSendReply(pkt);
    }
    return OK;
}
