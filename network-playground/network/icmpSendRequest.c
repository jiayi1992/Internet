/**
 * @file icmpSendRequest.c
 * @provides icmpSendRequest
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
 * Send an ICMP request
 * @param ipAddr Destination IPv4 address
 * @param hwAddr Destination MAC address
 * @param id     ICMP identifier, used to index ICMP buffer
 * @param seqNum ICMP sequence number
 * @return OK for success, SYSERR for syntax error
 */
syscall icmpSendRequest(uchar *ipAddr,
                        ushort id,
                        ushort seqNum)
{
    int i;
    struct icmpPkt       *icmpP = NULL;
    uchar               buf[ICMP_HEADER_LEN + 4];
    message             msg;
    
    if (ipAddr == NULL )
        return SYSERR;
    
    /* Set up ICMP header */
    bzero(buf, ICMP_HEADER_LEN + 4);
    
    icmpP = (struct icmpPkt *) buf;
    
    icmpP->type = ICMP_ECHO_RQST_T;
    icmpP->code = ICMP_ECHO_RQST_C;
    icmpP->chksum = 0x0000;
    icmpP->id = htons(id);
    icmpP->seqNum = htons(seqNum);
    
    // Put the current time in seconds in the icmp packet's datafield
    ulongToUchar4(icmpP->data, clocktime, BIG_ENDIAN);
    
    // Calculate the checksum
    icmpP->chksum = checksum((void *) icmpP, ICMP_HEADER_LEN);
    
    // Grab semaphore
    wait(icmpTbl[id].sema);
    
    ipWrite((void *) buf, ICMP_HEADER_LEN + 4, IPv4_PROTO_ICMP, ipAddr);
    
    // Update icmpTbl entry
    icmpTbl[id].pid = getpid();
    icmpTbl[id].flag = ICMP_RQST_SENT;
    icmpTbl[id].seqNum = seqNum;
    
    for (i = 0; i < IP_ADDR_LEN; i++)
        icmpTbl[id].ipAddr[i] = ipAddr[i];
    
    // Give back the entry's semaphore
    signal(icmpTbl[id].sema);
    
    msg = recvtime(1000);
    
    if (msg == TIMEOUT || (int) msg == 0)
        return SYSERR;
    
    return (syscall) msg;
}
