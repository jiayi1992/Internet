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
syscall ipRecv(struct ipgram *pkt, uchar *srcAddr)
{
    int i, eqFlag;
    ushort origChksum, calChksum;
    
    if (pkt == NULL || srcAddr == NULL)
        return SYSERR;
    
    
    // Screen out packets with bad IPv4 headers
    if ( !(pkt->ver_ihl & 0x40) ||
          (pkt->ver_ihl & 0x0F < 5) ||
          (ntohs(pkt->len) < IPv4_HDR_LEN) )
        return SYSERR;
    
    
    // Screen out packets not addressed to us/are not broadcast messages
    eqFlag = OK;
    if (pkt->dst[i] != 0xFF) // It couldn't be a broadcast msg
    {
        for (i = 0; i < IPv4_ADDR_LEN; i++)
        {
            if (pkt->dst[i] != net.ipAddr[i])
            {
                eqFlag = SYSERR;
                break;
            }
        }
        if (eqFlag == SYSERR)
            return OK;
    }
    else // The packet could be a broadcast msg; check it
    {
        for (i = 0; i < IPv4_ADDR_LEN; i++)
        {
            if (pkt->dst[i] != 0xFF)
            {
                eqFlag = SYSERR;
                break;
            }
        }
        if (eqFlag == SYSERR)
            return OK;
    }
    
    
    // Screen out packets with a bad checksum
    origChksum = pkt->chksum;
    pkt->chksum = 0;
    calChksum = checksum((void *) pkt, IPv4_HDR_LEN);
    
    
    if (calChksum != origChksum)
        return SYSERR;
    
    
    // Handle the received packet based on its protocol
    if (pkt->proto == IPv4_PROTO_ICMP)
    {
        return icmpRecv(pkt, srcAddr);
    }
    
    return OK;
}
