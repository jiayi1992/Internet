/**
 * @file arp.c
 * @provides arp table 
 *
 */
/* Author: Drew Vanderwiel  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <arp.h>
#include <stdio.h>  

/* Global ARP table definition */
struct arpTable arp;

void arpInitDebug(void);

/**
 * Initializes the arp table and starts an ARP Daemon process
 * @return OK for success, SYSERR for syntax error
 */
syscall arpInit(void)
{
    int i;
    
    // Get this machine's ip addr
    arp.ipAddr = nvramGet("lan_ipaddr\0");
    
    // Get this machine's mac addr
    etherControl(&devtab[ETH0], ETH_CTRL_GET_MAC, (long) &arp.hwAddr, 0);
    
    /* Initialize arp semaphore*/ 
    arp.sema = semcreate(1);
    
    /* Initialize arp table contents to be invalid/empty */
    for (i = 0; i < ARP_TABLE_LEN; i++)
    {
        arp.tbl[i].osFlags = ARP_ENT_INVALID;
    }
    
    /* ARP Init debug: for testing purposes */
    arpInitDebug();
    
    /* Start arp daemon process */
    arp.dId = create((void *)arpDaemon, INITSTK, 3, "ARP_DAEMON", 0);

    ready(arp.dId, 1);
    
    return OK;
}


/**
 * ARP debug initialization
 * For debugging the arp table
 */
void arpInitDebug(void)
{
    int i,j;
    
    // This adds 3 fake ip to mac mappings in the ARP table    
    for (i = 0; i < 3; i++)
    {
        arp.tbl[i].osFlags = ARP_ENT_VALID;
        
        // Make up an ip
        for (j = 0; j < IP_ADDR_LEN; j++)
            arp.tbl[i].ipAddr[j] = i;
        
        // Make up a mac addr
        for (j = 0; j < ETH_ADDR_LEN; j++)
            arp.tbl[i].hwAddr[j] = i;
    }    
    
    printf("DEBUG: ARP table contents initialized\n");
}


/**
 * ARP Daemon process: manages arp requests and replies
 */
void arpDaemon(void)
{
    uchar               packet[PKTSZ];
    struct ethergram    *egram = NULL;

    // Zero out the packet buffer.
    bzero(packet, PKTSZ);
    
    while(1)
    {
        read(ETH0, (void *) &packet, PKTSZ);
        
        egram = (struct ethergram *) packet;
        
        if(ntohs(egram->type) != ETYPE_ARP)
            continue;
        
        arpRecv((struct arpPkt *) &egram->data);
    }
    
    return;
}


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
    /* int                 bytes; */
    
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
        arpP->addrs[i + ARP_SHA_OFFSET] = arp.hwAddr[i];
    
    // Source protocol addr
    for (i = 0; i < IP_ADDR_LEN; i++)
        arpP->addrs[i + ARP_SPA_OFFSET] = arp.ipAddr[i];
    
    // Dest hw addr
    for (i = 0; i < ETH_ADDR_LEN; i++)
        arpP->addrs[i + ARP_DHA_OFFSET] = 0x00;
    
    // Dest protocol addr
    for (i = 0; i < IP_ADDR_LEN; i++)
        arpP->addrs[i + ARP_DPA_OFFSET] = ipAddr[i];
    
    /* Send packet */
    /* bytes = */ write(ETH0, (uchar *)buf, PKTSZ);

    return OK;
}


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


/*
What will arpRecv need to do?

Is the arp packet a request destined for me? Is it a broadcast message?
You may need to send an arpReply (new function!)
Is it a reply to an ARP request previously sent by me?
You need to update the ARP table
How do you decide which of the two it will be?
*/

/*
How about arpResolve?

This is a function that upper layers will need to call.
Your code will only invoke it through the shell command.

void arpResolve(uchar *)
{
    
    
}
*/