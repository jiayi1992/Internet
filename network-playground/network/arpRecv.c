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
  
    //arpRecvDebug(pkt);
    
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
    
    //printf("\nGot ARP msg type %d\n", ntohs(pkt->op));
    arpAddEntry(&pkt->addrs[ARP_SPA_OFFSET], &pkt->addrs[ARP_SHA_OFFSET]);  
    
    if (ntohs(pkt->op) == ARP_OP_RQST)
    {
        arpSendReply(pkt);
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