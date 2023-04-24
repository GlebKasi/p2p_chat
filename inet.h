#ifndef  __inet_h
#define  __inet_h

#include	<sys/socket.h>	/* basic socket definitions */
#include	<errno.h>
#include	<netdb.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>      /* for fork */
#include    <arpa/inet.h>   /* translation to 32-bit IP addres*/
#include    <iostream>

#define	    SERV_TCP_PORT	6010
#define 	SERV_TCP_IP    "127.0.0.1"

#define MAXLINE 1024 /*max line text*/

#endif

