/**
 * @file netWrite.c
 * @provides netWrite
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/29/2016       */

#include <xinu.h>
#include <network.h>
#include <ether.h>


/**
 * Fragment an IPv4 packet and send it via Ethernet
 * @param ipPkg    Pointer to struct containing IP header, 
 *                 a pointer to the payload, and a variable to holding
 *                 the length of the data
 * @param mac      Destination HW MAC address
 * @return OK for success, SYSERR for syntax error
 */
syscall netWrite(struct ipPack *ipPkg, uchar *mac)
{
    if (ipPkt == NULL || mac == NULL)
        return SYSERR;
    
    /*
    This function fills the various IPv4 header fields, 
    assigns a unique packet identifier, calculates the IP header checksum, 
    and looks up the destination MAC of the next hop.
    */
    
    
    
    return OK;
}
