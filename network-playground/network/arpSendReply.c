/**
 * @file arpSendReply.c
 * @provides TODO 
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <arp.h>

/**
 * Send an ARP reply to the provided destination IPv4 address
 * with the mac address found in our ARP table matching the
 * lookUpIpAddr
 * @param destIpAddr destination IPv4 address
 * @param destHwAddr destination hardware (mac) address
 * @param lookUpIpAddr
 * @return OK for success, SYSERR for syntax error
 */
syscall arpSendReply(uchar *destIpAddr, uchar *destHwAddr, uchar *lookUpIpAddr)
{
    if (destIpAddr == NULL ||
        destHwAddr == NULL ||
        lookUpIpAddr == NULL)
    {
        return SYSERR;
    }    

    return OK;
}
