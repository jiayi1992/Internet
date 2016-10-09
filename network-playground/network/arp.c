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

struct arpEntry arp_t[ARP_TABLE_LEN];   /** Defined in arp.c */
semaphore arp_tsema;                    /** Semaphore for arp table */
int arpDaemonId;                        /** ARP daemon process ID */


/**
 * Initializes the arp table and starts an ARP Daemon process
 */
syscall arpInit(void)
{
    int i,j;

    /* Initialize arp semaphore*/ 
    arp_tsema = semcreate(1);
    
    /* Initialize arp table contents to be invalid/empty */
    for (i = 0; i < ARP_TABLE_LEN; i++)
    {
        arp_t[i].osFlags = ARP_ENT_INVALID;
    }
    
    
    /* !@!@!@! Test code !@!@!@!*/
    // This adds 3 fake ip to mac mappings in the ARP table
    
    for (i = 0; i < 3; i++)
    {
        arp_t[i].osFlags = ARP_ENT_VALID;
        
        // Make up an ip
        for (j = 0; j < IP_ADDR_LEN; j++)
            arp_t[i].ipAddr[j] = i;
        
        // Make up a mac addr
        for (j = 0; j < ETH_ADDR_LEN; j++)
            arp_t[i].hwAddr[j] = i;
    }
    
    
    printf("DEBUG: ARP table contents initialized\n");
    /* !@!@!@! End Test code !@!@!@!*/
    
    /* Start arp daemon process */
    arpDaemonId = create((void *)arpDaemon, INITSTK, 3, "ARP_DAEMON", 0);

    ready(arpDaemonId, 1);
    
    return OK;
}


/**
 * ARP Daemon process: manages arp requests and replies
 */
void arpDaemon(void)
{
    uchar packet[PKTSZ];
    struct ethergram *egram = NULL;
    struct arpPkt *arpP = NULL;
    int j, bytes;

// Read packet from the network
// Typecast so that you can access easily the Ethernet fields
// If it is an ARP packet, call a function to handle it. 
// Name it whatever you want, e.g. arpReceive
// For instance, you can check whether the type is equal to ETYPE_ARP
// (constant defined in include/network.h). Note, though, that due to the 
// network and host byte order issue that we have mentioned, you will need to 
// check the value ntohs(egram->type) instead of just egram->type

    while(1)
    {
        bytes = read(ETH0, (void *) &packet, PKTSZ);
        
        egram = (struct ethergram *) packet;
        
        if(ntohs(egram->type) != ETYPE_ARP)
        {
            continue;
        }
        arpP = (struct arpPkt *) &egram->data;
        
        // If the ARP packet is a request
        if (ntohs(arpP->op) == ARP_OP_RQST)
        {
            printf("Got ARP Request\n");
            
            // Print source mac addr
            for (j = 0; j < ETH_ADDR_LEN-1; j++)
                printf("%x:",arpP->addrs[j + ARP_SHA_OFFSET]);
            printf("%x\n",arpP->addrs[ARP_SPA_OFFSET - 1]);
            
            // Print source protocol addr
            for (j = 0; j < IP_ADDR_LEN - 1; j++)
                printf("%d.",arpP->addrs[j + ARP_SPA_OFFSET]);
            printf("%d\n",arpP->addrs[ARP_DHA_OFFSET - 1]);
            
            // Print dest hw addr
            for (j = 0; j < ETH_ADDR_LEN - 1; j++)
                printf("%x:",arpP->addrs[j + ARP_DHA_OFFSET]);
            printf("%x\n",arpP->addrs[ARP_DPA_OFFSET - 1]);
            
            // Print dest protocol addr
            for (j = 0; j < IP_ADDR_LEN - 1; j++)
                printf("%d.",arpP->addrs[j + ARP_DPA_OFFSET]);
            printf("%d\n",arpP->addrs[ARP_ADDR_END_OFFSET - 1]);
            
        }
        // If the ARP packet is a reply
        else if (ntohs(arpP->op) == ARP_OP_REPLY)
        {
            //wait(arp_tsema);
            printf("ARP reply\n");
            
            // Print source mac addr
            for (j = 0; j < ETH_ADDR_LEN-1; j++)
                printf("%x:",arpP->addrs[j + ARP_SHA_OFFSET]);
            printf("%x\n",arpP->addrs[ARP_SPA_OFFSET - 1]);
            
            // Print source protocol addr
            for (j = 0; j < IP_ADDR_LEN - 1; j++)
                printf("%d.",arpP->addrs[j + ARP_SPA_OFFSET]);
            printf("%d\n",arpP->addrs[ARP_DHA_OFFSET - 1]);
            
            // Print dest hw addr
            for (j = 0; j < ETH_ADDR_LEN - 1; j++)
                printf("%x:",arpP->addrs[j + ARP_DHA_OFFSET]);
            printf("%x\n",arpP->addrs[ARP_DPA_OFFSET - 1]);
            
            // Print dest protocol addr
            for (j = 0; j < IP_ADDR_LEN - 1; j++)
                printf("%d.",arpP->addrs[j + ARP_DPA_OFFSET]);
            printf("%d\n",arpP->addrs[ARP_ADDR_END_OFFSET - 1]);
            
            //signal(arp_tsema);
        }
        else
        {
            printf("Unknown ARP message\n");
        }
    }
    
    return;
}








/* Use the buffer */



/*
What will arpRecv need to do?

Is the arp packet a request destined for me? Is it a broadcast message?
You may need to send an arpReply (new function!)
Is it a reply to an ARP request previously sent by me?
You need to update the ARP table
How do you decide which of the two it will be?
void arpRecv(...)
{
    
    
}

*/



/*
How about arpResolve?

This is a function that upper layers will need to call.
Your code will only invoke it through the shell command.

void arpResolve(uchar *)
{
    
    
}
*/