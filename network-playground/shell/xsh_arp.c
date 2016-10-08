/**
 * @file     xsh_arp.c
 * @provides xsh_arp
 *
 */
/* Author: Drew Vanderwiel  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

/** STUFF TODO
 *  - Any reading/changes to arp table needs to use a semaphore lock
 *  - Implement arpResolve function and call it to request a
 *    new ip to mac mapping
 */



#include <xinu.h>
#include <string.h>
#include <arp.h>

int arpTablePrint(void);

/**
 * Shell command to print/manpulate the ARP table
 * @param nargs count of arguments in args
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_arp(int nargs, char *args[])
{
    uchar packet[PKTSZ];
    uchar *ppkt = packet;
    uchar tmp_ipAddr[IP_ADDR_LEN];
    int i, j;
    
    // Zero out the packet buffer.
    bzero(packet, PKTSZ);
    
    // If the user gave no arguments display arp table contents
    if (nargs < 2)
    {
        return arpTablePrint();
    }
    
    if (nargs == 2)
    {
        // Print helper info about this shell command
        printf("arp [-a|-d] [IP address]\n");
        printf("    -d <IP ADDR>  delete entry from arp table with this IP addr\n");
        printf("    -a <IP ADDR>  resolve and add entry to arp table with this IP addr\n");
        printf("           NOTE: arp table is displayed if no arguments are given\n");
        return OK;
    }
    
    /********************************/
    /** Remove entry from ARP table**/
    /********************************/
    if (strcmp("-d",args[1]) == 0)
    {
        if (OK == dot2ip(args[2],tmp_ipAddr))
        {
            wait(arp_tsema);
            for (i = 0; i < ARP_TABLE_LEN; i++)
            {
                // Skip comparision if known invalid
                if (arp_t[i].osFlags == ARP_ENT_INVALID)
                    continue;
                
                // Compare addresses
                for(j = 0; j < IP_ADDR_LEN; j++)
                {
                    // Stop checking if there is a difference
                    if (tmp_ipAddr[j] != arp_t[i].ipAddr[j])
                        break;
                }
                
                // The address is the same, then invalidate the entry
                if (j == IP_ADDR_LEN)
                {
                    arp_t[i].osFlags = ARP_ENT_INVALID;
                    break;
                }
            }
            signal(arp_tsema);
        }
        else
        {
            printf("arp: invalid IP address format, example: 129.123.233.123\n");
            return SYSERR;
        }
    }
    /*********************************************************/
    /** Resolve an IP to a mac address and add to ARP table **/
    /*********************************************************/
    else if (strcmp("-a",args[1]) == 0)
    {
        if (OK == dot2ip(args[2],tmp_ipAddr))
        {
            // TODO resolve ip and add to arp table
            // return arpResolve(...);
        }
        else
        {
            printf("arp: invalid IP address format, example: 129.123.233.123\n");
            return SYSERR;
        }
    }
    /********************************************************/
    /** Resolve an IP to a mac address and add to ARP table**/
    /********************************************************/
    else
    {
        printf("arp: invalid option\n");
        return SYSERR;
    }

    return OK;
}

/**
 * Helper function to print the ARP table to the console
 * @return OK for success, SYSERR for syntax error
 */
int arpTablePrint(void)
{
    int i,j;
    wait(arp_tsema);
    /******************************/
    /** Print ARP table contents **/
    /******************************/
    printf("Address\t\t\tHWaddress\n");
    for (i =0; i < ARP_TABLE_LEN; i++)
    {
        // Skip invalid entries
        if (arp_t[i].osFlags == ARP_ENT_INVALID)
            continue;
        
        // Print the IP addr
        for (j = 0; j < IP_ADDR_LEN-1; j++)
            printf("%d.",arp_t[i].ipAddr[j]);
        printf("%d",arp_t[i].ipAddr[IP_ADDR_LEN-1]);
        
        // Print tab spacing
        printf("\t\t\t");
        
        // Print if the mac is invalid, print asteristics
        if (arp_t[i].osFlags & ARP_ENT_IP_ONLY)
        {
            printf("**:**:**:**:**:**");
        }
        // Otherwise print the mac address
        else
        {
            for (j = 0; j < ETH_ADDR_LEN-1; j++)
                printf("%2x:",arp_t[i].hwAddr[j]);
            printf("%2x",arp_t[i].hwAddr[ETH_ADDR_LEN-1]);
        }
        
        // Print new line
        printf("\n");
    }
    signal(arp_tsema);
    return OK;
}