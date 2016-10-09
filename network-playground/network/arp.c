/**
 * @file arp.c
 * @provides arp initialization, arp table manipulation, and arp daemon
 *
 */
/* Authors: Drew Vanderwiel, Jiayi Xin */
/* Class:   COSC4300         */
/* Date:    10/7/2016        */

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
    i = etherControl(&devtab[ETH0], ETH_CTRL_GET_MAC, (long) &arp.hwAddr, 0);
    printf("DEBUG: etherControl result %d\n", i);
    
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
    
    printf("DEBUG: ARP daemon initalized\n");
    
    while(1)
    {
        printf("DEBUG: ARP daemon 0\n");
        read(ETH0, (void *) &packet, PKTSZ);
        
        printf("DEBUG: ARP daemon 1\n");
        egram = (struct ethergram *) packet;
        
        printf("DEBUG: ARP daemon 2\n");
        if(ntohs(egram->type) != ETYPE_ARP)
            continue;
        
        printf("DEBUG: ARP daemon 3\n");
        arpRecvLOL((struct arpPkt *) &egram->data);
    }
    
    return;
}

/**
 * Add an entry to the ARP table
 * @param ipAddr IPv4 address of entry we want to add
 * @param hwAddr mac address of entry we want to add
 * @return OK for success, SYSERR for syntax error
 */
syscall arpAddEntry(uchar * ipAddr, uchar *hwAddr)
{
    if (ipAddr == NULL || hwAddr == NULL)
        return SYSERR;
    
    
    return OK;
}


/**
 * Find the arpEntryID of the entry we are looking for
 * @param ipAddr IPv4 address of entry we are looking for
 * @return OK for success, SYSERR for syntax error
 */
arpEntryID arpFindEntry(uchar *ipAddr)
{
    if (ipAddr == NULL)
        return ARP_ENT_NOT_FOUND;
    
    
    return 0;
}
