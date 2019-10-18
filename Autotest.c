/* This is autotest file to check the
* functionalities of protocol state machine
*/

#include "header.h"

int main()
{
    /* Initialise the protocol state machine, to enable
    *  Tx and Rx
    */
    unsigned char key = 12;
    proto_init( key );
    int pkt_buffer_size = 0;
    int pkt_buffer_present = 0;
    
    /********************************************************************/
    /* Test - 1 */
    /* send a normal connect packet with valid args */
    unsigned char buffer[CONNECT_PACKET_LENGTH];
    
    /* creating a normal connect packet */
    trace( " Sending a valid connect packet" );
    buffer[0] = CONNECT;
    buffer[0] <<= 1;
    buffer[0] &= COMMAND;
    buffer[1] = CONNECT_PACKET_LENGTH;
    buffer[2] = 0x02 ^ key;
    
    /* send the packet to protocol */
    trace ( "send the packet to protocol" );
    proto_rx ( buffer, CONNECT_PACKET_LENGTH );
    
    int error = 0;
    if ( pkt_buffer_present )
        {
        /* check for response */
        trace( " AUTOTEST RECIEVED ONE PACKET " );
        if ( pkt_buffer[0] & COMMAND )
            {
                error = 1;
                trace( " ERROR: Autotest received an command packet " );
            }
        if ( ( pkt_buffer[0] >> 1 ) != CONNECT )
            {
                error = 1;
                trace( " ERROR: Autotest did not recieved connet reply " );
            }
        if (  pkt_buffer[1] != CONNECT_PACKET_LENGTH )
            {
                error = 1;
                trace ( " ERROR: Autotest got incorrect length parameter packet " );
            }
        if ( ( pkt_buffer[2] ^ key ) != 0x02 )
            {
                error = 1;
                trace ( " ERROR: Autotest did not see correct channel-id " );
            }
        if ( error )
            {
                trace (" *** PROTOCOL FAILED *** " );
            }
        else{
                trace (" *** PROTOCOL PASSED *** " );
            }
       }
}

int proto_tx( unsigned char * proto_buffer, int buffer_size )
{
    if( ! pkt_buffer_present )
        {
        /* store the packet in buffer */
        pkt_buffer_present = 1;
        int i = 0;
        for ( i = 0; i < buffer_size ; i++ )
            {
                pkt_buffer[i] = proto_buffer[i];
            }
        pkt_buffer_size = buffer_size;
        trace ( " PROTO_TX  :  Packet sent " );
        return 0;
        }
    trace ( " PROTO_TX   :   Acket not sent " );
    return -1;
}

void trace ( char * str )
{
  int debug = 1;
    if ( debug )
        {
            printf("!!  %s  !!\n", str );
        }
}
