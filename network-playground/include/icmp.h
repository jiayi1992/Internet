/**
 * @file icmp.h
 *
 * $Id:$
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/28/2016       */

#ifndef _ICMP_H_
#define _ICMP_H_

#include <network.h>
#include <ether.h>

/* ICMP message Types */
#define ICMP_ECHO_RQST_T 0x08
#define ICMP_ECHO_RPLY_T 0x00

/* ICMP message codes */
#define ICMP_ECHO_RQST_C  0
#define ICMP_ECHO_RPLY_C  0

/* ICMP packet size */
#define ICMP_PKTSIZE ETHER_MINPAYLOAD + ETH_HEADER_LEN

/*
 * ICMP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Type (8 bits) | Code (8 bits)  |      Checksum (16 bits)      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |            Identifier          |      Sequence Number         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          Data Section                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
 
/** ICMP header entry contents */
struct icmpPkt
{
    uchar type;
    uchar code;
    ushort chksum;
    ushort id;
    ushort seqNum;
    uchar data[1]; 
};

/** IPv4 ping with ICMP Echo request **/
syscall ping(uchar *ipAddr);

/** ICMP Echo reply **/
syscall icmpEchoReply(uchar *ipAddr);

#endif                          /* _ICMP_H_ */
