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
    uchar *tmp_addr;
    
    // Get this machine's ip addr
    tmp_addr = (uchar *) nvramGet("lan_ipaddr\0");
    
    for (i = 0; i < IP_ADDR_LEN; i++)
    {
        arp.ipAddr[i] = tmp_addr[i];
        printf("%d.", arp.ipAddr[i]);
    }
    printf("\n");
    
    // Get this machine's mac addr
    i = etherControl(&devtab[ETH0], ETH_CTRL_GET_MAC, (long) &arp.hwAddr, 0);
    printf("DEBUG: etherControl result %d\n", i);
    
    /* Initialize arp semaphore*/ 
    arp.sema = semcreate(1);
    
    /* Initialize arp table free entry index*/ 
    arp.freeEnt = 0;
    
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
    
    /*
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
    }*/
    
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
        read(ETH0, (void *) &packet, PKTSZ);
        
        egram = (struct ethergram *) packet;
        
        if(ntohs(egram->type) != ETYPE_ARP)
            continue;
        
        arpRecv((struct arpPkt *) &egram->data);
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
    int i, entID;
    
    if (ipAddr == NULL || hwAddr == NULL)
        return SYSERR;
    
    wait(arp.sema);
    
    entID = arpFindEntry(ipAddr);
    
    if (entID == ARP_ENT_NOT_FOUND)
    {
        entID = arp.freeEnt;
        
        // Set IP Address of entry
        for (i = 0; i < IP_ADDR_LEN; i++)
            arp.tbl[entID].ipAddr[i] = ipAddr[i];
        
        // Set mac address of entry
        for (i = 0; i < ETH_ADDR_LEN; i++)
            arp.tbl[entID].hwAddr[i] = hwAddr[i];
        
        arp.tbl[entID].osFlags = ARP_ENT_VALID;
        
        for (i = 0; i < ARP_TABLE_LEN; i++)
        {
            // Find an invalid entry to be the next freeEnt
            if (arp.tbl[i].osFlags == ARP_ENT_INVALID)
            {
                arp.freeEnt = i;
                break;
            }
        }
        
        // TODO: Implement a timer on each entry in the arp table
        // Replace the first element next time
        if (i == ARP_TABLE_LEN)
        {
            arp.tbl[0].osFlags = ARP_ENT_INVALID;
            arp.freeEnt = 0;
        }
    }
    // Entry has an ip address, but no mac address
    else if (arp.tbl[entID].osFlags & ARP_ENT_IP_ONLY)
    {
        // Set mac address of entry
        for (i = 0; i < ETH_ADDR_LEN; i++)
            arp.tbl[entID].hwAddr[i] = hwAddr[i];
        
        arp.tbl[entID].osFlags = ARP_ENT_VALID;
    }
    signal(arp.sema);
    return OK;
}


/**
 * Find the index of the entry we are looking for
 * @param ipAddr IPv4 address of entry we are looking for
 * @return OK for success, SYSERR for syntax error
 */
int arpFindEntry(uchar *ipAddr)
{
    int i, j;
    
    if (ipAddr == NULL)
        return ARP_ENT_NOT_FOUND;
    
    for (i = 0; i < ARP_TABLE_LEN; i++)
    {
        // Skip invalid entries
        if (arp.tbl[i].osFlags == ARP_ENT_INVALID)
            continue;
        
        // Compare addresses
        for(j = 0; j < IP_ADDR_LEN; j++)
        {
            // Stop checking if there is a difference
            if (ipAddr[j] != arp.tbl[i].ipAddr[j])
                break;
        }
        
        // The address is the same, return the index
        if (j == IP_ADDR_LEN)
        {
            signal(arp.sema);
            return i;
        }
    }
    return ARP_ENT_NOT_FOUND;
}
