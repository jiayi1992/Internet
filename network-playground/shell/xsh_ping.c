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
	
	if (nargs < 2)
    {
        // Print helper info about this shell command
        printf("arp [IP address]\n");
        return OK;
    }

    if (OK == dot2ip(args[1],tmp_ipAddr))
    {
		/*
        if(OK == icmpResolve(tmp_ipAddr, hwAddr))
        {
            printf("arp: Resolved MAC address: ");
            for(i = 0; i < ETH_ADDR_LEN-1; i++)
            {
                printf("%02x:", hwAddr[i]);
            }
            printf("%02x\n", hwAddr[ETH_ADDR_LEN-1]);
        }
        else
        {
            printf("arp: Error, unable to resolve IP address after trying %d times\n", ARP_RESOLVE_ATTEMPTS);
            return SYSERR;
        }
		*/
    }
    else
    {
        printf("ping: invalid IP address format, example: 192.168.1.1\n");
        return SYSERR;
    }

	
    return OK;
}
