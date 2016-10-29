/**
 * @file icmp.c
 * @provides icmpRecv, icmpHandleRequest, and icmpHandleReply
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
 * Receive and filter ICMP Packets
 * @param ipPkt   received IPv4 packet
 * @param srcAddr Sender MAC address
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpRecv(struct ipgram *ipPkt, uchar *srcAddr)
{
    int i, eqFlag;
    struct icmpPkt *pkt;
    ushort origChksum, calChksum;
     
    if (ipPkt == NULL)
        return SYSERR;
    
    // TODO start after the ip packets options, at its data
    pkt = (struct icmpPkt *) ipPkt->opts;
    
    // Screen out packets with bad ICMP headers
    if ( pkt->type != ICMP_ECHO_RQST_T ||
         pkt->type != ICMP_ECHO_RPLY_T ||
         pkt->code != ICMP_ECHO_RQST_C )
        return SYSERR;
    
    // Screen out packets with a bad ICMP checksums
    origChksum = pkt->chksum;
    pkt->chksum = 0;
    calChksum = checksum((void *) pkt, ICMP_HEADER_LEN);
    
    printf("ICMP Recv Orig checksum: %04x calc'd: %04x", origChksum, calChksum);
    
    if (calChksum != origChksum)
        return SYSERR;
    
    // Handle the ICMP packet
    if ( pkt->type == ICMP_ECHO_RQST_T)
    {
        return icmpHandleRequest(ipPkt, srcAddr);
    }
    else if ( pkt->type == ICMP_ECHO_RPLY_T )
    {
        return icmpHandleReply(ipPkt, srcAddr);
    }
    
    return OK;
}


/**
 * Handle ICMP Echo request Packets
 * @param ipPkt   received IPv4 packet
 * @param srcAddr Sender MAC address
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpHandleRequest(struct ipgram *ipPkt, uchar *srcAddr)
{
    // TODO
    return OK;
}


/**
 * Handle ICMP Echo reply Packets
 * @param ipPkt   received IPv4 packet
 * @param srcAddr Sender MAC address
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpHandleReply(struct ipgram *ipPkt, uchar *srcAddr)
{
    // TODO
    return OK;
}

