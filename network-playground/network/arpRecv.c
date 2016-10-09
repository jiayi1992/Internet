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
    int j;
    
    if (pkt == NULL)
        return SYSERR;
 
    switch(ntohs(pkt->op))
    {
    /*************************/
    /** Handle ARP Request **/
    /*************************/
    case ARP_OP_RQST:
        //wait(arp.sema);
        printf("Got ARP Request\n");
        
        
        /*
        Pseudo code:
        
        We could also check if the address lengths are correct for sanity too
        (maybe before switch case?)
        
        Put the requester's info into the arp table?
        
        If requester is looking for our mac address
            send the requester a reply with our mac address and ip addr
            
        else sender is looking for someone else's mac address
            if we can find a valid arp entry with that someone's ip, send a packet with its mac
            else, don't send anything
            
        Send reply to requester:
            Eth Dest mac = requester's
            Eth Src mac = ours
            
            *Arp src mac = what the requester is requesting (could be us or someone elses?)
            *Arp src ip addr = what the requester is requesting (could be us or someone elses?)
            Arp dest mac = requester's
            Arp dest ip = requester's
            
            * these fields are variable depending on if the requestor is looking 
              for our mac or someone else's
        */
        
        /* Print dest mac addr (from ethergram)
        for (j = 0; j < ETH_ADDR_LEN-1; j++)
            printf("%x:",egram->dst[j]);
        printf("%x\n",egram->dst[ETH_ADDR_LEN - 1]);
        */
        
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
        //signal(arp.sema);
        break;
        
    /*************************/
    /** Handle ARP Reply **/
    /*************************/
    case ARP_OP_REPLY:
        //wait(arp.sema);
        printf("ARP reply\n");
        
        /*
        Pseudo code:
        
        We could also check if the address lengths are correct for sanity too 
        (maybe before switch case?)
        
        If our mac and ip addr match the arp dest addresses:
            Add the src mac and ip addr to the arp table
            
        Reply sent to us:
            Eth Dest mac = ours
            Eth Src mac = replier
            
            *Arp src mac = replier's or the requested mac addr
            *Arp src ip addr = replier's or the requested ip addr
            Arp dest mac = ours
            Arp dest ip = our (Do we allow this to be incorrect?)
            
            * these fields are the ones that are important
        */
        
        /* Print dest mac addr (from ethergram)
        for (j = 0; j < ETH_ADDR_LEN-1; j++)
            printf("%x:",egram->dst[j]);
        printf("%x\n",egram->dst[ETH_ADDR_LEN - 1]);
        */
        
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
        
        //signal(arp.sema);
        break;
        
    default:
        break;
    }
    
    return OK;
}
