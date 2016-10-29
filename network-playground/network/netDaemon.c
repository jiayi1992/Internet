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
#include <arp.h>


/**
 * Network Daemon process: handles IPv4, ARP, and ICMP packets as they arrive
 */
void netDaemon(void)
{
    uchar               packet[PKTSZ];
    struct ethergram    *egram = NULL;

    // Zero out the packet buffer.
    bzero(packet, PKTSZ);
    
    while(1)
    {
        read(ETH0, (void *) &packet, PKTSZ);
        
        egram = (struct ethergram *) packet;
        
        if(ntohs(egram->type) == ETYPE_ARP)
            arpRecv((struct arpPkt *) &egram->data);;
    }
    
    return;
}

