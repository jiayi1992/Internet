/**
 * @file netDaemon.c
 * @provides network daemon
 *
 */
/* Authors: Drew Vanderwiel, Jiayi Xin */
/* Class:   COSC4300         */
/* Date:    10/29/2016       */

#include <stdio.h>
#include <xinu.h>
#include <network.h>
#include <arp.h>


/**
 * Network Daemon process: handles IPv4, ARP, and ICMP packets as they arrive
 */
void netDaemon(void)
{
    uchar               packet[PKTSZ];
    ushort              type = 0x0;
    struct ethergram    *egram = NULL;

    // Zero out the packet buffer.
    bzero(packet, PKTSZ);
    
    while(1)
    {
        read(ETH0, (void *) &packet, PKTSZ);
        
        egram = (struct ethergram *) packet;
        
        type = ntohs(egram->type);
        
        if (ETYPE_IPv4 == type)
            ipRecv((struct ipgram *) &egram->data, (uchar *) &egram->src);
        else if(ETYPE_ARP == type)
            arpRecv((struct arpPkt *) &egram->data);
    }
    
    return;
}

