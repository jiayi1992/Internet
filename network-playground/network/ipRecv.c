/**
 * @file ipRecv.c
 * @provides ipRecv
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
 * Handle IPv4 Packets
 * @param pkt received IPv4 packet
 * @return OK for success, SYSERR for syntax error
 */
syscall ipRecv(struct ipgram *pkt, uchar *srcAddr)
{
    int i, eqFlag;
    ushort origChksum, calChksum;
     
    if (pkt == NULL || srcAddr == NULL)
        return SYSERR;
    
    
    // Screen out packets with bad IPv4 headers
    if ( pkt->ver_ihl != 0x45 ||
         ntohs(pkt->len) < IPv4_HDR_LEN )
        return SYSERR;
        
    
    
    // Screen out packets not addressed to us
    eqFlag = OK;
    for (i = 0; i < IPv4_ADDR_LEN; i++)
    {
        if (pkt->dst[i] != arp.ipAddr[i])
        {
            eqFlag = SYSERR;
            break;
        }
    }
    
    //printf("IPv4 recvd 3\n");
    
    if (eqFlag == SYSERR)
        return OK;
    
    printf("IPv4 recvd 4\n");
    
    // Screen out packets with a bad checksum
    origChksum = ntohs(pkt->chksum);
    pkt->len = ntohs(pkt->len);
    pkt->id = ntohs(pkt->id);
    pkt->flags_froff = ntohs(pkt->flags_froff);
    pkt->chksum = 0;
    calChksum = checksum((void *) pkt, IPv4_HDR_LEN);
    
    // Put it back the way it was
    pkt->len = htons(pkt->len);
    pkt->id = htons(pkt->id);
    pkt->flags_froff = htons(pkt->flags_froff);
    
    printf("IP Recv Orig checksum: %04x calc'd: %04x", origChksum, calChksum);
    
    if (calChksum != origChksum)
        return SYSERR;
    
    printf("IPv4 recvd 1\n");
    
    // Handle the received packet based on its protocol
    if (pkt->proto == IPv4_PROTO_ICMP)
    {
        return icmpRecv(pkt, srcAddr);
    }
    
    printf("IPv4 recvd 2\n");
    
    return OK;
}
