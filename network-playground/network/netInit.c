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

/* Network Information Struct */
struct netInfo net;


/**
 * Initialize network interface.
 */
void netInit(void)
{
    // Open the Ethernet device
    open(ETH0);
    
    // Get this machine's ip addr
    dot2ip(nvramGet("lan_ipaddr\0"), (uchar *) &net.ipAddr);
    
    // Get this machine's mac addr
    etherControl(&devtab[ETH0], ETH_CTRL_GET_MAC, (long) &net.hwAddr, 0);
    
    // Initialize ARP table watcher and ARP table
    arpInit();
    
    // Initialize the ICMP table
    icmpInit();
    
    // Create net daemon process
    net.dId = create((void *)netDaemon, INITSTK, 3, "NET_DAEMON", 0);
    
    // Start the network daemon
    ready(net.dId, 1);

    return;
}


/**
 * @return the process id of this process
 */
syscall getpid(void)
{
    return (currpid);
}