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

/* IPv4 Packet Fragmentation Storage Struct */
struct ipFragEntry ipFrags[IPv4_FRAG_ENTS];


/**
 * Handle IPv4 Packets
 * @param pkt received IPv4 packet
 * @return OK for success, SYSERR for syntax error
 */
syscall ipRecv(struct ipgram *pkt, uchar *srcAddr)
{
    int i
    ushort eqFlag, demuxFlag;
    ushort ipfroff, ipflags, ipid, ipLen, ipHdrLen, ipDataLen;
    ushort origChksum, calChksum;
    struct ipgram *demuxIpPkt = NULL;
    
    /*
    Hosts on the receiving end of fragmented IPv4 datagrams perform 
    fragment reassembly. When the first fragment of a datagram arrives,
    a conceptual timer is started; if the rest of the fragments do not
    arrive before the timer expires, all of the fragments are discarded. 
    A fragmented datagram is not demultiplexed to higher layers unless all 
    of its fragments arrive.
    
    
    In the interest of simplicity for our embedded network stack implementation,
    you may assume that your stack is capable of assembling only one fragmented
    datagram at a time. Instead of a conceptual time-out, your implementation may
    give up on a fragmented datagram as soon as a fragment from some other datagram
    arrives.
    */
    
    if (pkt == NULL || srcAddr == NULL)
        return SYSERR;
    
    
    // Screen out packets with bad IPv4 headers
    if ( !(pkt->ver_ihl & 0x40) ||
         ((pkt->ver_ihl & IPv4_IHL) < 5) ||
          (ntohs(pkt->len) < IPv4_HDR_LEN) )
        return SYSERR;
    
    
    // Screen out packets not addressed to us/are not broadcast messages
    eqFlag = OK;
    if (pkt->dst[0] != 0xFF) // It couldn't be a broadcast msg
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
    
    demuxFlag = 0;
    ipfroff = ntohs(pkt->flags_froff) & IPv4_FROFF;
    ipflags = ntohs(pkt->flags_froff) & IPv4_FLAGS;
    ipid =  ntohs(pkt->id);
    ipLen = ntohs(pkt->len);
    ipHdrLen = (pkt->ver_ihl & IPv4_IHL) << 2;
    ipDataLen = ipLen - ipHdrLen;
    
    // If this packet is an IPv4 fragment packet, handle it
    if (ipfroff > 0 || ipflags == IPv4_FLAG_MF)
    {
        // This packet has "timed out" if this boolean is true
        // A "time out" means we clear the ipFrag struct and start from scratch
        timeoutFlag = (ipid != ipFrags[0].id && ipFrags[0].flag == IPv4_FRAG_INCOMPLETE);
         
        if ( ipFrags[0].flag == IPv4_FRAG_INVALID || timeoutFlag )
        {
            // Start a new fragment
            ipFrags[0].flag = IPv4_FRAG_INCOMPLETE;
            ipFrags[0].id = ipid;
            ipFrags[0].recvdBytes = ipDataLen;
            ipFrags[0].pktDataLen = 0;
            
            // Copy the IP header
            memcpy((void *) ipFrags[0].pkt,(void *) pkt, ipHdrLen);
            
            // Set the start of the IP packet's data
            dataStart = (uchar *) &ipFrags[0].pkt[ipHdrLen];
            
            // Copy the data from the ip packet
            memcpy((void *) &dataStart[ipfroff], 
                   (void *) &pkt->opts[ipHdrLen - IPv4_HDR_LEN],
                   ipDataLen);
            
            // If this packet is the last fragment, then set the ipFrag struct's pktDataLen
            // variable
            if (ipflags == 0)
            {
                // Total pkt data len = IP pkt len - IP hdr len + the last fragment's offset
                ipFrags[0].pktDataLen = ipDataLen + ipfroff;
                
                demuxIpPkt = (struct ipgram *) ipFrags[0].pkt;
                demuxIpPkt->len = htons(ipFrags[0].pktDataLen + ipHdrLen);
            }
        }
        else if (ipFrags[0].flag == IPv4_FRAG_INCOMPLETE)
        {
            // Add this fragment to the ipFragment struct
            ipFrags[0].recvdBytes += ipDataLen;
            
            demuxIpPkt = (struct ipgram *) ipFrags[0].pkt;
            
            // Copy the data from the ip packet
            memcpy((void *) &dataStart[ipfroff], 
                   (void *) &pkt->opts[ipHdrLen - IPv4_HDR_LEN],
                   ipDataLen);
            
            // If this packet is the last fragment, then set the ipFrag struct's pktDataLen
            // variable
            if (ipflags == 0)
            {
                // Total pkt data len = IP pkt len - IP hdr len + the last fragment's offset
                ipFrags[0].pktDataLen = ipDataLen + ipfroff;
                
                demuxIpPkt->len = htons(ipFrags[0].pktDataLen + ipHdrLen);
            }
            
            // If all the fragments have been collected, then set the flag to invalid
            // and demux the assembled packet
            if (ipFrags[0].pktDataLen == ipFrags[0].recvdBytes)
            {
                ipFrags[0].flag = IPv4_FRAG_INVALID;
                
                // Clean up the complete packet header for the higher layers
                demuxIpPkt->flags_froff = 0;
                demuxIpPkt->chksum = 0x0000;
                demuxIpPkt->chksum = checksum((void *) demuxIpPkt, IPv4_HDR_LEN);
                
                // Ready for demuxing
                demuxFlag = 1;
            }
        }
        
    }
    // This packet is not an IPv4 fragment, handle it
    else
    {
        demuxFlag = 1;
        demuxIpPkt = pkt;
    }
    
    
    // If this packet is complete (has all its fragments), then demux it
    if (demuxFlag)
    {
        // Handle the received packet based on its protocol
        if (demuxIpPkt->proto == IPv4_PROTO_ICMP)
        {
            return icmpRecv(demuxIpPkt, srcAddr);
        }
    }
    return OK;
}
