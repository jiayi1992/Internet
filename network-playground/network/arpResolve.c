/**
 * @file arpResolve.c
 * @provides TODO 
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <arp.h>

/* Private/helper functions */
void arpResolveHelper(uchar *, long, uchar *);
syscall getpid(void);


/**
 * Resolve an mac address from a given ip address
 * @param ipAddr IPv4 address to resolve
 * @param hwAddr mac address return value
 * @return OK for success, SYSERR for syntax error
 */
syscall arpResolve(uchar *ipAddr, uchar *hwAddr)
{
    int i, helperID, entID;
    long currpid;
    message msg;
    
    if (ipAddr == NULL || hwAddr == NULL)
    {
        return SYSERR;
    }

    // Search the arp table, checking if it has already been mapped
    entID = arpFindEntry(ipAddr);
    
    // Get this process's ID
    currpid = getpid();

    // If we found the ipAddr in the arp table and it is valid, then return it
    if (entID != ARP_ENT_NOT_FOUND && arp.tbl[entID].osFlags == ARP_ENT_VALID)
    {
        for(i = 0; i < ETH_ADDR_LEN; i++)
            hwAddr[i] = arp.tbl[entID].hwAddr[i];
    }
    // The entry doesn't have a mac address mapped, or doesn't exist
    else
    {
        // Block and create a helper process
        helperID = create((void *)arpResolveHelper, INITSTK, 3, "ARP_HELPER", 3, ipAddr, currpid, hwAddr);
        ready(helperID, 1);

        // Wait for the message from the helper process
        msg = recvtime(10000);
        
        // mac addresss was not found or timeout
        if(msg == TIMEOUT || (int) msg == 0)
        {
            return SYSERR;
        }
    }
    return OK;
}


/**
 * Helper process to arp request multiple times
 * @param ipAddr IPv4 address to resolve
 * @param sourpid the PID of the process who created this helper process
 * @param hwAddr mac address return value
 * @return OK for success, SYSERR for syntax error
 */
void arpResolveHelper(uchar *ipAddr, long sourpid, uchar *hwAddr)
{
    int attempts, i, entID;
    message msg;

    // Attempt to resolve the mac address
    for(attempts = 0; i < ARP_RESOLVE_ATTEMPTS; attempts++)
    {
        arpSendRequest(ipAddr);

        entID = arpFindEntry(ipAddr);

        // The IP address was successfully resolved to a mac address
        if (entID != ARP_ENT_NOT_FOUND && arp.tbl[entID].osFlags == ARP_ENT_VALID)
        {
            for(i = 0; i < ETH_ADDR_LEN; i++)
                hwAddr[i] = arp.tbl[entID].hwAddr[i];

            msg = (message)1;
            break;
        }
        // Not resolved, wait 1 second before retrying
        else
        {
            sleep(1000);
        }
    }

    // The mac address was not found
    if(attempts >= ARP_RESOLVE_ATTEMPTS) 
    {
        msg = (message)0;
    }

    send(sourpid, msg);
    return;
}


/**
 * @return the process id of this process
 */
syscall getpid(void)
{
    return (currpid);
}