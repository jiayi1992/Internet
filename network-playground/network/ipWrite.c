/**
 * @file ipWrite.c
 * @provides ipWrite
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/29/2016       */

#include <xinu.h>
#include <network.h>
#include <ether.h>
#include <icmp.h>


/**
 * Send an IPv4 packet
 * @param payload  pointer to the raw payload
 * @param len   Length of the payload in bytes
 * @param tos   Type of IPv4 service
 * @return OK for success, SYSERR for syntax error
 */
syscall ipWrite(void *payload, ushort len, uchar tos, uchar *ipAddr)
{
    if (payload == NULL || ipAddr == NULL)
        return SYSERR;
    
    /*
    The ipWrite function should take as parameters a pointer to the payload, 
    a payload length, an IPv4 type field, and a destination IP address. 
    This function builds the IPv4 header, and passes the packet down to the 
    netWrite function with a destination MAC address.
    */
    return OK;
}
