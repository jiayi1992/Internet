/**
 * @file netWrite.c
 * @provides netWrite
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   11/19/2016       */

#include <xinu.h>
#include <network.h>
#include <ether.h>


/**
 * Send an Ethernet packet with the given payload
 * @param payload       Pointer to the ethernet payload
 * @param payloadLen    length in bytes of the payload
 * @param type          Ethernet packet type
 * @param mac           Destination HW MAC address
 * @return OK for success, SYSERR for syntax error
 */
syscall netWrite(void *payload, ushort payloadLen, ushort type, uchar *hwAddr)
{
    int i;
    struct ethergram    *egram = NULL;
    uchar               pktBuf[PKTSZ];
    
    if (payload == NULL || hwAddr == NULL || payloadLen > ETH_MTU)
        return SYSERR;
    
    // Zero out the packet buffer
    bzero(pktBuf, PKTSZ);
    
    /* Set up Ethergram header */
    egram = (struct ethergram *) pktBuf;
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->dst[i] = hwAddr[i];
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->src[i] = net.hwAddr[i];
    
    egram->type = htons(type);
    
    memcpy((void *) egram->data, payload, payloadLen);
    
    write(ETH0, (uchar *) pktBuf, ETH_HEADER_LEN + payloadLen);
    
    return OK;
}
