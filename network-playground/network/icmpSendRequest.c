/**
 * @file icmpSendRequest.c
 * @provides icmpSendRequest
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/29/2016       */

#include <xinu.h>
#include <network.h>
#include <ether.h>
#include <icmp.h>
#include <arp.h>

/**
 * Send an ICMP request
 * @param ipAddr Destination IPv4 address
 * @param hwAddr Destination MAC address
 * @param id     ICMP identifier, used to index ICMP buffer
 * @param seqNum ICMP sequence number
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpSendRequest(uchar *ipAddr, 
                        uchar *hwAddr, 
                        ushort id,
                        ushort seqNum)
{
    // TODO
    
    return OK;
}

