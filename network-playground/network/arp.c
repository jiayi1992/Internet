/**
 * @file arp.c
 * @provides arp initialization, arp table manipulation, and the arp table watcher
 *
 */
/* Authors: Drew Vanderwiel, Jiayi Xin */
/* Class:   COSC4300         */
/* Date:    10/29/2016       */

#include <xinu.h>
#include <arp.h>
#include <stdio.h>  

/* Global ARP table definition */
struct arpTable arp;


/**
 * Initializes the arp table and starts an ARP Daemon process
 * @return OK for success, SYSERR for syntax error
 */
syscall arpInit(void)
{
    int i;
    
    /* Initialize arp semaphore */ 
    arp.sema = semcreate(1);
    
    /* Initialize arp table free entry index */ 
    arp.freeEnt = 0;
    
    /* Initialize arp table victim entry (to be replaced if arp table full) */ 
    arp.victimEnt = 0;
    
    /* Initialize arp table contents to be invalid/empty */
    for (i = 0; i < ARP_TABLE_LEN; i++)
    {
        arp.tbl[i].osFlags = ARP_ENT_INVALID;
    }
    
    /* Create arp table watcher */
    arp.wId = create((void *)arpWatcher, INITSTK, 3, "ARP_WATCHER", 0);
    
    ready(arp.wId, 1);
    
    return OK;
}


/**
 * ARP Table Watcher process: updates arp entry timeouts
 */
void arpWatcher(void)
{
    int i;
     
    while(1)
    {
        // Sleep 1 second
        sleep(1000);
        wait(arp.sema);
        for (i = 0; i < ARP_TABLE_LEN; i++)
        {
            // Skip invalid entries
            if (arp.tbl[i].osFlags == ARP_ENT_INVALID)
                continue;
            
            // Invalidate entries that have timed out
            if (arp.tbl[i].timeout == 0)
            {
                arp.tbl[i].osFlags = ARP_ENT_INVALID;
                continue;
            }
            
            arp.tbl[i].timeout--;
        }
        signal(arp.sema);
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
        arp.tbl[entID].timeout = ARP_ENT_DEFAULT_TIMEOUT;
        
        for (i = 0; i < ARP_TABLE_LEN; i++)
        {
            // Find an invalid entry to be the next freeEnt
            if (arp.tbl[i].osFlags == ARP_ENT_INVALID)
            {
                arp.freeEnt = i;
                break;
            }
        }
        
        // Replace the first element next time
        if (i >= ARP_TABLE_LEN)
        {
            // Invalidate an entry
            arp.tbl[arp.victimEnt].osFlags = ARP_ENT_INVALID;
            arp.freeEnt = arp.victimEnt;
            
            // Move the victimEnt index up and wrap around if end of arp table is reached
            arp.victimEnt++;
            if (arp.victimEnt >= ARP_TABLE_LEN)
                arp.victimEnt = 0;
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
    // Entry exists, is valid (has mac and IP addr), so refresh it
    else
    {
        // Set mac address of entry
        for (i = 0; i < ETH_ADDR_LEN; i++)
            arp.tbl[entID].hwAddr[i] = hwAddr[i];
        
        // Reset it's timeout
        arp.tbl[entID].timeout = ARP_ENT_DEFAULT_TIMEOUT;
    }
    signal(arp.sema);
    return OK;
}


/**
 * Find the index of the entry we are looking for
 * Caution: This function doesn't access the arp table using
 *          the arp semaphore.
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
            return i;
    }
    return ARP_ENT_NOT_FOUND;
}
