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

void helper(uchar *, long, uchar *);
syscall getpid(void);

/**
 * Resolve an mac address from a given ip address
 * @param ipAddr IPv4 address to resolve
 * @param hwAddr mac address return value
 * @return OK for success, SYSERR for syntax error
 */
syscall arpResolve(uchar *ipAddr, uchar *hwAddr)
{
    int i, j, entID;
    long currpid;
    message msg;
    
    if (ipAddr == NULL || hwAddr == NULL)
    {
        return SYSERR;
    }

    entID = arpFindEntry(ipAddr);
    
    currpid = getpid();

    // If find ipAddr in table and it is valid, print it
    if (entID != ARP_ENT_NOT_FOUND && arp.tbl[entID].osFlags == ARP_ENT_VALID)
    {
        printf("HWaddress\n");
        for(i = 0; i < ETH_ADDR_LEN-1; i++)
        {
            hwAddr[i] = arp.tbl[entID].hwAddr[i];
            printf("%02x:",arp.tbl[entID].hwAddr[i]);
        }
        hwAddr[ETH_ADDR_LEN-1] = arp.tbl[entID].hwAddr[ETH_ADDR_LEN-1];
        printf("%02x\n",arp.tbl[entID].hwAddr[ETH_ADDR_LEN-1]);
    }
    // Entry dosen't have the ip address, or no mac address for the ipAddr
    else
    {
        // block and create a helper process
        j = create((void *)helper, INITSTK, 3, "ARP_HELPER", 3, ipAddr, currpid, hwAddr);
        ready(j, 1);

        //wait for the message from the helper process
        msg = recvtime(10000);

        //Not find or timeout
        if(msg == TIMEOUT || (int)msg == 0)
        {
            printf("Not find\n");
            return SYSERR;
        }
    }
    return OK;    
}

void helper(uchar *ipAddr, long sourpid, uchar *hwAddr)
{
    int i, entID;
    message msg;

    //Three attempts to ARP resolve
    for(i = 0; i < 3; i++)
    {
        arpSendRequest(ipAddr);

        entID = arpFindEntry(ipAddr);

        if (entID != ARP_ENT_NOT_FOUND && arp.tbl[entID].osFlags == ARP_ENT_VALID)
        {
            printf("HWaddress\n");
            for(i = 0; i < ETH_ADDR_LEN-1; i++)
            {
                hwAddr[i] = arp.tbl[entID].hwAddr[i];
                printf("%02x:",arp.tbl[entID].hwAddr[i]);
            }
            hwAddr[ETH_ADDR_LEN-1] = arp.tbl[entID].hwAddr[ETH_ADDR_LEN-1];
            printf("%02x\n",arp.tbl[entID].hwAddr[ETH_ADDR_LEN-1]);

            msg = (message)1;
            break;
        }
        else{
            sleep(1000);
        }
    }

    //Not find the Mac Address
    if(i == 3) 
    {
        msg = (message)0;
    }

    send(sourpid, msg);
    return;
}

syscall getpid(void)
{
    return (currpid);
}