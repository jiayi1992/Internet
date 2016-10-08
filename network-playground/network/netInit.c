/**
 * @file netInit.c
 * @provides netInit.
 *
 */
/* Author: Drew Vanderwiel  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <network.h>
#include <ether.h>
#include <arp.h>

/**
 * Initialize network interface.
 */
void netInit(void)
{
    // Open the Ethernet device
    open(ETH0);

    // Initialize ARP daemon and ARP table
    arpInit();

    return;
}
