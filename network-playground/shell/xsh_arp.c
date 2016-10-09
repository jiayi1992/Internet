/**
 * @file     xsh_arp.c
 * @provides xsh_arp
 *
 */
/* Author: Drew Vanderwiel  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

/** STUFF TODO
 *  - Any reading/changes to arp table needs to use a semaphore lock (DONE)
 *  - Implement arpResolve function and call it to request a
 *    new ip to mac mapping 
 */



#include <xinu.h>
#include <string.h>
#include <arp.h>

int arpTablePrint(void);
int arpTest(uchar *ipAddr);
int arpSend(uchar *ipAddr);

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
    /*********************************************************/
    /** ARP command testing area **/
    /*********************************************************/
    else if (strcmp("-t",args[1]) == 0)
    {
        if (OK == dot2ip(args[2],tmp_ipAddr))
        {
            return arpTest(&tmp_ipAddr);
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
            return arpSend(tmp_ipAddr);
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

int arpSend(uchar *ipAddr)
{
	int i;
	struct ethergram *egram = NULL;
	struct arpPkt *arpP = NULL;
    char buf[PKTSZ];
	char *hostIp;
	int bob;
	uchar ethaddr[ETH_ADDR_LEN];
	
	// Get the host ip addr
	hostIp = nvramGet("lan_ipaddr\0");	
	
	egram = (struct ethergram *) buf;

	for (i = 0; i < ETH_ADDR_LEN; i++)
		egram->dst[i] = 0xFF;
	
	bob = etherControl(&devtab[ETH0], ETH_CTRL_GET_MAC, (long) &ethaddr, 0);
	
	for (i = 0; i < ETH_ADDR_LEN; i++)
		printf("%x:",ethaddr[i]);
	printf("\n");
	
/*	
	for (i = 0; i < ETH_ADDR_LEN; i++)
		egram->src[i] = htons(ethaddr[i]);
	
	egram->type = htons(ETYPE_ARP);
	
	arpP = (struct arpPkt *) &egram->data;
	
	arpP->hwType = htons(ARP_HWTYPE_ETHERNET);
	arpP->prType = htons(ARP_PRTYPE_IPv4);
	arpP->hwAddrLen = htons(ETH_ADDR_LEN);
	arpP->prAddrLen = htons(IP_ADDR_LEN);
	arpP->op = htons(ARP_OP_RQST);
	
	// Source hw addr
	for (i = 0; i < ETH_ADDR_LEN; i++)
		arpP->addrs[i] = htons(ethaddr[i]);
	
	// Source protocol addr
	for (i = ETH_ADDR_LEN; i < ETH_ADDR_LEN + IP_ADDR_LEN; i++)
		arpP->addrs[i] = htons(hostIp[i]);
	
	// Dest hw addr
	for (i = ETH_ADDR_LEN + IP_ADDR_LEN; i < ETH_ADDR_LEN*2 + IP_ADDR_LEN; i++)
		arpP->addrs[i] = 0xFF;
	
	// Dest protocol addr
	for (i = ETH_ADDR_LEN*2 + IP_ADDR_LEN; i < ETH_ADDR_LEN*2 + IP_ADDR_LEN*2; i++)
		arpP->addrs[i] = htons(ipAddr[i]);
	*/
	for (i = 0; i < ETH_ADDR_LEN; i++)
		egram->src[i] = ethaddr[i]
	
	egram->type = ETYPE_ARP;
	
	arpP = (struct arpPkt *) &egram->data;
	
	arpP->hwType = ARP_HWTYPE_ETHERNET;
	arpP->prType = ARP_PRTYPE_IPv4;
	arpP->hwAddrLen = ETH_ADDR_LEN;
	arpP->prAddrLen = IP_ADDR_LEN;
	arpP->op = ARP_OP_RQST;
	
	// Source hw addr
	for (i = 0; i < ETH_ADDR_LEN; i++)
		arpP->addrs[i] = ethaddr[i];
	
	// Source protocol addr
	for (i = ETH_ADDR_LEN; i < ETH_ADDR_LEN + IP_ADDR_LEN; i++)
		arpP->addrs[i] = hostIp[i];
	
	// Dest hw addr
	for (i = ETH_ADDR_LEN + IP_ADDR_LEN; i < ETH_ADDR_LEN*2 + IP_ADDR_LEN; i++)
		arpP->addrs[i] = 0xFF;
	
	// Dest protocol addr
	for (i = ETH_ADDR_LEN*2 + IP_ADDR_LEN; i < ETH_ADDR_LEN*2 + IP_ADDR_LEN*2; i++)
		arpP->addrs[i] = ipAddr[i];
	
    i = write(ETH0, (uchar *)buf, PKTSZ);

	if(i != SYSERR){
		printf("Bytes sent: %d\n",i);
	}else{
		printf("error");
	}
    
    return OK;
}

int arpTest(uchar *ipAddr)
{
    struct ethergram ethPkt;
    char buffer[ETH_RX_BUF_SIZE];
    char *ip;
    int i, j;
    
    //ethPkt = (struct ethergram *) malloc(sizeof(struct ethergram));
    printf("Before read\n");
    i = read(ETH0, &buffer, ETH_RX_BUF_SIZE);
    printf("After read: %d\n",i);
    
    // print this host's ip address
    ip = nvramGet("lan_ipaddr\0");
    
    // Print the IP addr
    for (j = 0; j < IP_ADDR_LEN-1; j++)
        printf("%d.",ip[j]);
    printf("%d",ip[IP_ADDR_LEN-1]);
    
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