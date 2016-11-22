/**
 * @file ipWrite.c
 * @provides ipWrite
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   11/19/2016       */

#include <xinu.h>
#include <network.h>
#include <ether.h>
#include <arp.h>


/**
 * Send an IPv4 packet
 * @param data     pointer to the raw payload
 * @param id       id of the packet, set by the upper layers
 * @param dataLen  Length of the payload in bytes
 * @param proto    Protocol of IPv4 service
 * @return OK for success, SYSERR for syntax error
 */
syscall ipWrite(void *data, ushort id, ushort dataLen, uchar proto, uchar *ipAddr)
{
    int i;
    struct ipgram       *ipP = NULL;
    uchar               pktBuf[ETH_MTU];
    uchar               dstHwAddr[ETH_ADDR_LEN];
    uchar               *dataBytes;
    ushort              pktSize;
    ushort              dataSize;
    ushort              froff;
    int                 dataLeft;
    
    if (data == NULL || ipAddr == NULL || dataLen > (0xFFFF - IPv4_HDR_LEN))
        return SYSERR;
    
    /*
    The ipWrite function should take as parameters a pointer to the payload, 
    a payload length, an IPv4 type field, and a destination IP address. 
    This function builds the IPv4 header, and passes the packet down to the 
    netWrite function with a destination MAC address.
    */
    
    if (SYSERR == arpResolve(ipAddr, dstHwAddr))
        return SYSERR;
    
    // Zero out the packet buffer
    bzero(pktBuf, ETH_MTU);
    
    ipP = (struct ipgram *) pktBuf;
    dataBytes = (uchar *) data;
    
    // Version 5, IHL size 5 * (4 byte words) = 20
    ipP->ver_ihl = 0x45;
    ipP->tos = IPv4_TOS_ROUTINE;
    
    // Set the packet size
    pktSize = IPv4_HDR_LEN + dataLen;
    ipP->len = htons(pktSize);
    
    ipP->id = htons(id);
    ipP->flags_froff = 0;
    ipP->ttl = IPv4_TTL;
    ipP->proto = proto;
    ipP->chksum = 0x0000;
    
     // Source protocol addr (ours)
    for (i = 0; i < IP_ADDR_LEN; i++)
        ipP->src[i] = net.ipAddr[i];
    
    // Dest protocol addr (requester's)
    for (i = 0; i < IP_ADDR_LEN; i++)
        ipP->dst[i] = ipAddr[i];
    
    ipP->chksum = checksum((void *) ipP, IPv4_HDR_LEN);
    
    // The total packet size can fit in a single Ethernet packet
    if (pktSize <= ETH_MTU)
    {
        // Add in the payload to the packet
        memcpy((void *) ipP->opts, (void *) dataBytes, dataLen);
        
        // Make sure the packet size is at least equal to the ETHER_MINPAYLOAD
        if (pktSize < ETHER_MINPAYLOAD)
            pktSize = ETHER_MINPAYLOAD;
        
        // Send the packet
        return netWrite((void *) pktBuf, pktSize, ETYPE_IPv4, (uchar *) &dstHwAddr);
    }
    
    // Otherwise, fragment the packet
    // Initialize the header of the first fragment
    dataSize = ETH_MTU - IPv4_HDR_LEN;
    ipP->len = htons(IPv4_HDR_LEN + dataSize);
    ipP->flags_froff = htons(IPv4_FLAG_MF);
    ipP->chksum = 0x0000;
    
    // Calculate the Checksum
    ipP->chksum = checksum((void *) ipP, IPv4_HDR_LEN);
    
    // Add in the payload to the packet
    memcpy((void *) ipP->opts, (void *) dataBytes, dataSize);
    
    // Move the payload pointer up
    dataBytes += dataSize;
    
    // Send the first fragment
    pktSize = IPv4_HDR_LEN + dataSize;
    netWrite((void *) pktBuf, pktSize, ETYPE_IPv4, (uchar *) &dstHwAddr);
    
    // Prepare for the next fragment
    dataLeft = dataLen;
    dataLeft -= dataSize;
    froff = dataSize/8;
    
    while (dataLeft > 0)
    {
        if ( dataLeft > (ETH_MTU - IPv4_HDR_LEN) )
        {
            dataSize = ETH_MTU - IPv4_HDR_LEN;
            ipP->flags_froff = htons(froff | IPv4_FLAG_MF);
        }
        else 
        {
            dataSize = dataLeft;
            ipP->flags_froff = htons(froff);
        }
        // Initialize the header of the fragment
        ipP->len = htons(IPv4_HDR_LEN + dataSize);
        ipP->chksum = 0x0000;
        
        // Calculate the Checksum
        ipP->chksum = checksum((void *) ipP, IPv4_HDR_LEN);
        
        // Add in the payload to the packet
        memcpy((void *) ipP->opts, (void *) dataBytes, dataSize);
        
        pktSize = IPv4_HDR_LEN + dataSize;
        
        // Make sure the packet size is at least equal to the ETHER_MINPAYLOAD
        if (pktSize < ETHER_MINPAYLOAD)
            pktSize = ETHER_MINPAYLOAD;
        
        // Send the fragment
        netWrite((void *) pktBuf, pktSize, ETYPE_IPv4, (uchar *) &dstHwAddr);
        
        // Prepare for the next fragment
        dataLeft -= dataSize;
        froff += dataSize/8;
        
        // Move the payload pointer up
        dataBytes += dataSize;
    }
    
    return OK;
}
