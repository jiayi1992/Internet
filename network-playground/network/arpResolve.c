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

/**
 * Resolve an mac address from a given ip address
 * @param ipAddr IPv4 address to resolve
 * @param hwAddr mac address return value
 * @return OK for success, SYSERR for syntax error
 */
syscall arpResolve(uchar *ipAddr, uchar *hwAddr)
{
    int i, j, entID, currpid;
    message msg;
    
    if (ipAddr == NULL || hwAddr == NULL)
    {
        return SYSERR;
    }

    wait(arp.sema);

    entID = arpFindEntry(ipAddr);
    
    currpid = getpid();
    
    // if find ipAddr in table and it is valid, print it
    if (entID != ARP_ENT_NOT_FOUND && arp.tbl[entID].osFlags == ARP_ENT_VALID)
    {
        for(i = 0; i < ETH_ADDR_LEN; i++)
            printf("%d:",arp.tbl[entID].hwAddr[i]);
    }
    // // Entry dosen't have the ip address, or no mac address for the ipAddr
    else
    {
        // block and create a helper process
        j = create((void *)helper, INITSTK, 3, "ARP_HELPER", 2, ipAddr, currpid);
        ready(j, 1);
        msg = recvtive(10000);
        
        //Not find or timeout
        if(msg == TIMEOUT || (int)msg == 0)
        {
            return SYSERR;
        }
    }
    return OK;
}

void helper(uchar *ipAddr, int sourpid)
{
    int i;
    message msg;
    for(i = 0; i < 3; i++){
        arpSendRequest(ipAddr);
        
        if (entID != ARP_ENT_NOT_FOUND && arp.tbl[entID].osFlags == ARP_ENT_VALID)
        {
            for(i = 0; i < ETH_ADDR_LEN; i++)
                printf("%d:",arp.tbl[entID].hwAddr[i]);
            msg = (message)1;
            break;
        }
        else{
            sleep(1000);
        }
    }
    
    //Not find the Mac Address
    if(i == 3) 
        msg = (message)0;
    
    send(sourpid, msg);
    return;
}