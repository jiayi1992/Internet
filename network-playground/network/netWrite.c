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
 * Fragment an IPv4 packet and send it via Ethernet
 * @param ipPkg    Pointer to struct containing IP header, 
 *                 a pointer to the payload, and a variable to holding
 *                 the length of the data
 * @param mac      Destination HW MAC address
 * @return OK for success, SYSERR for syntax error
 */
syscall netWrite(struct ipPack *ipPkg, uchar *hwAddr)
{
    int i;
    struct ipgram       *ipP = NULL;
    struct ethergram    *egram = NULL;
    uchar               pktBuf[PKTSZ];
    ushort              pktSize;
    ushort              dataSize;
    ushort              froff;
    int                 dataLeft;
    
    /*
    This function fills the various IPv4 header fields, 
    assigns a unique packet identifier, calculates the IP header checksum, 
    and looks up the destination MAC of the next hop.
    */
    
    if (ipPkg == NULL || hwAddr == NULL)
        return SYSERR;
    
    /* Set up Ethergram header */
    egram = (struct ethergram *) pktBuf;
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->dst[i] = hwAddr[i];
    
    for (i = 0; i < ETH_ADDR_LEN; i++)
        egram->src[i] = net.hwAddr[i];
    
    egram->type = htons(ETYPE_IPv4);
    
    ipP = (struct ipgram *) &egram->data;
    
    // Copy the IP Header from the IP Package Struct
    memcpy((void *) ipP, (void *) &ipPkg->ipHdr, IPv4_HDR_LEN);
    
    pktSize = ntohs(ipPkg->ipHdr.len);
    
    // If the packet will be less than the ETH_MTU,
    // then just send it as it is now
    if (pktSize <= ETH_MTU)
    {
        // Add in the payload to the packet
        memcpy((void *) ipP->opts, (void *) ipPkg->payload, ipPkg->dataLen);
        
        // Send the packet
        write(ETH0, (uchar *)pktBuf, ETH_HEADER_LEN + pktSize);
        return OK;
    }
    
    printf("netWrite 1\n");
    
    // Otherwise, fragment the packet
    
    // Initialize the header of the first fragment
    dataSize = ETH_MTU - IPv4_HDR_LEN;
    ipP->len = htons(IPv4_HDR_LEN + dataSize);
    ipP->flags_froff = htons(IPv4_FLAG_MF);
    ipP->chksum = 0x0000;
    
    // Calculate the Checksum
    ipP->chksum = checksum((void *) &ipP, IPv4_HDR_LEN);
    
    // Add in the payload to the packet
    memcpy((void *) ipP->opts, (void *) ipPkg->payload, dataSize);
    
    // Move the payload pointer up
    ipPkg->payload += dataSize;
    
    // Send the first fragment
    pktSize = IPv4_HDR_LEN + dataSize;
    write(ETH0, (uchar *)pktBuf, ETH_HEADER_LEN + pktSize);
    
    // Prepare for the next fragment
    dataLeft = ipPkg->dataLen;
    dataLeft -= dataSize;
    froff = dataSize/8;
    
    printf("netWrite 2\n");
    
    while (dataLeft > 0)
    {
        printf("netWrite 3: dataLeft: %d\n", dataLeft);
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
        memcpy((void *) ipP->opts, (void *) ipPkg->payload, dataSize);
        
        // Send the fragment
        pktSize = IPv4_HDR_LEN + dataSize;
        write(ETH0, (uchar *)pktBuf, ETH_HEADER_LEN + pktSize);
        
        // Prepare for the next fragment
        dataLeft = ipPkg->dataLen;
        dataLeft -= dataSize;
        froff += dataSize/8;
        printf("netWrite 4: dataLeft: %d\n", dataLeft);
    }
    
    return OK;
}
