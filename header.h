#ifndef HEADER_H
#define HEADER_H
#include<stdio.h>
#include<stdlib.h>

/* Initializes the protocol */
extern void proto_init( unsigned char key );

/* Reception of packet from lower layer. */
extern void proto_rx( unsigned char * buffer, int buffer_size );

/* Transmits a protocol packet  */
extern int proto_tx( unsigned char * buffer, int buffer_size );

/* Reject an unexpected packet */
extern void reject_unexpected_packet( unsigned char * buffer );

/* Trace messages */
extern void trace( char * str );

/* Reject packets */
extern void reject_packet ( unsigned char * buffer, int reason );

/* GLOBAL VARIABLES */
extern int state;
extern unsigned char xkey;
extern int channel_id;
extern int debug;

extern unsigned char pkt_buffer[5];
extern int pkt_buffer_size;
extern int pkt_buffer_present;

/* CURRENT STATE OF PROTOCOL MACHINE */
# define IDLE       0
# define CONNECTED  1
# define CONFIGURED 2

/* PACKET TYPES */
# define CONNECT     0x01
# define CONFIG      0x02
# define DISCONNECT  0x04
# define REJECT      0x05

/* CR TYPES */
# define COMMAND  0x01
# define RESPONSE 0x00

/* CONFIG PARAMETERS */
# define CF1 0x1e5b
# define CF2 0xf3c2
# define CF3 0x392b
# define CF4 0xffff

/* PACKET REJECT REASON */
# define UNEXPECTED_PACKET   0x01
# define WRONG_LENGTH        0x02
# define INVALID_CHANNEL_ID  0x03
# define KEY_MISMATCH        0x04
# define UNKNOWN_ERROR       0x05

/* PACKET LENGTHS */
# define REJECT_PACKET_LENGTH  0x05
# define CONNECT_PACKET_LENGTH 0x03
# define CONFIG_PACKET_LENGTH  0x05
#endif
