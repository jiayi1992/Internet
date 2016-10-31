/**
 * @file netInit.c
 * @provides netInit.
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <network.h>
#include <ether.h>
#include <icmp.h>
#include <arp.h>

/* Network daemon ID */
int netdId;

/**
 * Initialize network interface.
 */
void netInit(void)
{
    // Open the Ethernet device
    open(ETH0);

    // Initialize ARP table watcher and ARP table
    arpInit();
    
    // Initialize the ICMP table
    icmpInit();

    // Create net daemon process
    netdId = create((void *)netDaemon, INITSTK, 3, "NET_DAEMON", 0);

    // Point the arp daemon id to the network daemon id
    arp.dId = netdId;

    // Start the network daemon
    ready(netdId, 1);

    return;
}


/**
 * @return the process id of this process
 */
syscall getpid(void)
{
    return (currpid);
}