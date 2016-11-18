/**
 * @file netWrite.c
 * @provides netWrite
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
 * TODO
 * @param TODO
 * @param TODO
 * @return OK for success, SYSERR for syntax error
 */
syscall netWrite(struct ipgram *ipPkt, uchar *mac)
{
    if (ipPkt == NULL || mac == NULL)
        return SYSERR;
    
    /*
    This function fills the various IPv4 header fields, 
    assigns a unique packet identifier, calculates the IP header checksum, 
    and looks up the destination MAC of the next hop.
    */
    return OK;
}
