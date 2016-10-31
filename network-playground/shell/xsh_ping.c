/**
 * @file     xsh_ping.c
 * @provides xsh_ping
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/28/2016       */


#include <xinu.h>
#include <string.h>
#include <icmp.h>

#define ICMP_PINGS 4

/**
 * Shell command for pinging IPv4 addresses
 * @param nargs count of arguments in args
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_ping(int nargs, char *args[])
{
    /* TODO */
	uchar tmp_ipAddr[IP_ADDR_LEN];
    uchar hwAddr[ETH_ADDR_LEN];
    ushort foundid, i, j;
    int bytesRecvd = 0;
    int counter = 0;
	
	if (nargs < 2)
    {
        // Print helper info about this shell command
        printf("ping [IP address]\n");
        return OK;
    }

    if (OK == dot2ip(args[1],tmp_ipAddr))
    {
        // Resolve the IP
        arpResolve(tmp_ipAddr, hwAddr);
        /*
        if (OK != arpResolve(tmp_ipAddr, hwAddr))
        {
            printf("ping: Error, unable to resolve IP address after trying %d times\n", ARP_RESOLVE_ATTEMPTS);
            return SYSERR;
        }
        */
		
        foundid = ICMP_TBL_LEN;
        
        // Find an invalid (free) ICMP table entry
        for (i = 0; i < ICMP_TBL_LEN; i++)
        {
            wait(icmpTal[i].sema);
            if(icmpTal[i].flag == ICMP_ENTRY_INVALID)
            {
                foundid = i;
                signal(icmpTal[i].sema);
                break;
            }
            signal(icmpTal[i].sema);
        }
        
        if (foundid == ICMP_TBL_LEN) 
        {
            printf("ping: internal error\n");
            return SYSERR;
        }
        
        // Print some starter text
        printf("\nPinging ");
        for (j = 0; j < IP_ADDR_LEN-1; j++)
            printf("%d.", tmp_ipAddr[j]);
        printf("%d", tmp_ipAddr[IP_ADDR_LEN-1]);
        printf(" with 28 bytes of data:\n"); // 32 bytes with a time stamp
        
        for (i = 0; i < ICMP_PINGS; i++)
        {
            // Get time before
            bytesRecvd = icmpSendRequest(tmp_ipAddr, hwAddr, foundid, i+1);
            
            if( SYSERR == bytesRecvd)
            {
                printf("PING: transmit failed. General failure.\n");
            }
            else
            {
                printf("Reply from ");
                for (j = 0; j < IP_ADDR_LEN-1; j++)
                    printf("%d.", tmp_ipAddr[j]);
                printf("%d", tmp_ipAddr[IP_ADDR_LEN-1]);
                printf(":  %d\n", bytesRecvd); //time=1ms TTL=61
                counter++;
            }
            // get time after
        }
        
        // Print some statistics
        /*
        printf("\nPing statistics for ");
        for (j = 0; j < IP_ADDR_LEN-1; j++)
            printf("%d.", tmp_ipAddr[j]);
        printf("%d", tmp_ipAddr[IP_ADDR_LEN-1]);
        printf(":\n");
        */
        
        // TODO...
    }
    else
    {
        printf("ping: invalid IP address format, example: 192.168.1.1\n");
        return SYSERR;
    }

	
    return OK;
}
