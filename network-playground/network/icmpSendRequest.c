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
 /*
syscall icmpSendRequest(uchar *ipAddr, 
                        uchar *hwAddr, 
                        ushort id,
                        ushort seqNum)
*/
uchar               buf[0xFFEB];

syscall icmpSendRequest(uchar *ipAddr, 
                        ushort id,
                        ushort seqNum)
{
    int i;
    struct icmpPkt       *icmpP = NULL;
    //uchar               buf[0xFFEB];// ICMP_HEADER_LEN + 50000
    message             msg;
    
    if (ipAddr == NULL ) // || hwAddr == NULL
        return SYSERR;
    
    printf(" Maxsize 1");
    /* Set up ICMP header */
    bzero(buf, 0xFFEB);
    
    printf(" Maxsize 2");
    icmpP = (struct icmpPkt *) buf;
    
    icmpP->type = ICMP_ECHO_RQST_T;
    icmpP->code = ICMP_ECHO_RQST_C;
    icmpP->chksum = 0x0000;
    icmpP->id = htons(id);
    icmpP->seqNum = htons(seqNum);
    
    // Put the current time in seconds in the icmp packet's datafield
    ulongToUchar4(icmpP->data, clocktime, BIG_ENDIAN);
    
    printf(" Maxsize 3");
    for( i = ICMP_HEADER_LEN + 4; i < 0xFFEB; i++)
    {
        buf[i] = (uchar) (i - ICMP_HEADER_LEN);
    }
    printf(" Maxsize 4");
    // Calculate the checksum
    icmpP->chksum = checksum((void *) icmpP, ICMP_HEADER_LEN);
    
    // Grab semaphore
    wait(icmpTbl[id].sema);
    
    printf(" Maxsize 5");
    
    ipWrite((void *) buf, 0xFFEB, IPv4_PROTO_ICMP, ipAddr);
    
    printf(" Maxsize 6");
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
