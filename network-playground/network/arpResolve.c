/**
 * @file arpResolve.c
 * @provides TODO 
 *
 */
/* Author: Drew Vanderwiel, Jiayi Xin  */
/* Class:  COSC4300         */
/* Date:   10/7/2016        */

#include <xinu.h>
#include <arp.h>

/**
 * Resolve an mac address from a given ip address
 * @param ipAddr IPv4 address to resolve
 * @param hwAddr mac address return value
 * @return OK for success, SYSERR for syntax error
 */
syscall arpResolve(uchar *ipAddr, uchar *hwAddr)
{
    if (ipAddr == NULL ||
        hwAddr == NULL)
    {
        return SYSERR;
    }

    return OK;
}
