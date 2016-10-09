/**
 * @file     xsh_arp.c
 * @provides xsh_arp
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

/** STUFF TODO
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
    uchar tmp_ipAddr[IP_ADDR_LEN];
    int i, j;
    
    // If the user gave no arguments display arp table contents
    if (nargs < 2)
        return arpTablePrint();
    
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
            wait(arp.sema);
            for (i = 0; i < ARP_TABLE_LEN; i++)
            {
                // Skip comparision if known invalid
                if (arp.tbl[i].osFlags == ARP_ENT_INVALID)
                    continue;
                
                // Compare addresses
                for(j = 0; j < IP_ADDR_LEN; j++)
                {
                    // Stop checking if there is a difference
                    if (tmp_ipAddr[j] != arp.tbl[i].ipAddr[j])
                        break;
                }
                
                // The address is the same, then invalidate the entry
                if (j == IP_ADDR_LEN)
                {
                    arp.tbl[i].osFlags = ARP_ENT_INVALID;
                    break;
                }
            }
            signal(arp.sema);
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
    /*********************************************************/
    /** ARP command testing area for send a request**/
    /*********************************************************/
    else if (strcmp("-rq",args[1]) == 0)
    {
        if (OK == dot2ip(args[2],tmp_ipAddr))
        {
            return arpSendRequest(tmp_ipAddr);
        }
        else
        {
            printf("arp: invalid IP address format, example: 129.123.233.123\n");
            return SYSERR;
        }
    }
    /******************************/
    /** Error invalid arp option **/
    /******************************/
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
    wait(arp.sema);
    /******************************/
    /** Print ARP table contents **/
    /******************************/
    printf("Address\t\t\tHWaddress\n");
    for (i =0; i < ARP_TABLE_LEN; i++)
    {
        // Skip invalid entries
        if (arp.tbl[i].osFlags == ARP_ENT_INVALID)
            continue;
        
        // Print the IP addr
        for (j = 0; j < IP_ADDR_LEN-1; j++)
            printf("%d.",arp.tbl[i].ipAddr[j]);
        printf("%d",arp.tbl[i].ipAddr[IP_ADDR_LEN-1]);
        
        // Print tab spacing
        printf("\t\t\t");
        
        // Print if the mac is invalid, print asteristics
        if (arp.tbl[i].osFlags & ARP_ENT_IP_ONLY)
        {
            printf("**:**:**:**:**:**");
        }
        // Otherwise print the mac address
        else
        {
            for (j = 0; j < ETH_ADDR_LEN-1; j++)
                printf("%2x:",arp.tbl[i].hwAddr[j]);
            printf("%2x",arp.tbl[i].hwAddr[ETH_ADDR_LEN-1]);
        }
        
        // Print new line
        printf("\n");
    }
    signal(arp.sema);
    return OK;
}