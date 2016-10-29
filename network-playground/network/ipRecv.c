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


/**
 * Handle IPv4 Packets
 * @param pkt received IPv4 packet
 * @return OK for success, SYSERR for syntax error
 */
syscall ipRecv(struct ipgram *pkt)
{
    int i, eqFlag;
    ushort origChksum, calChksum;
     
    if (pkt == NULL)
        return SYSERR;
    
    // Screen out packets with bad IPv4 headers
    if ( pkt->ver_ihl != IPv4_VERSION ||
         ntohs(pkt->len) < IPv4_HDR_LEN )
        return SYSERR;
    
    // Screen out packets with a bad checksum
    origChksum = ntohs(pkt->chksum);
    pkt->chksum = 0;
    calChksum = checksum((void *) pkt, IPv4_HDR_LEN);
    
    printf("IP Recv Orig checksum: %04x calc'd: %04x", origChksum, calChksum);
    
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
        
    if (eqFlag == SYSERR)
        return OK;
    
    /*
    if (pkt->proto == IPv4_PROTO_ICMP)
    {
        icmpEchoReply((struct icmpPkt *) &pkt->opts);
    }
    */
    return OK;
}
