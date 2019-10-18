#include "header.h"

void proto_init( unsigned char key )
{
    /* Initialising the state to IDLE */
    state = IDLE;
    xkey = key;  
}

/* Run the state machine */
void proto_rx( unsigned char * buffer, int buffer_size )
{
    /* Drop the response packets */
    if ( ! ( buffer[0] & COMMAND ) )
        {
        trace( ">> Dropping the response packet " );
        return ;
        }
        
    /* CHECK - 1. Verify the length */
    if ( buffer[1] != buffer_size )
        {
          /* discarding the wrong length packet */
          reject_packet ( buffer, WRONG_LENGTH );
          return;
        } 

    /* CHECK - 2. check the channel id */
    channel_id = buffer[2] ^ xkey;
    if ( ( channel_id < 0x01 ) || ( channel_id > 0x10 ) )
        {
            /* discard the packet */
            reject_packet ( buffer, INVALID_CHANNEL_ID );
            return;
        } 
    int packet_type = buffer[0] >> 1;
    
    /* 1. CONNECT PACKETS */
    if ( packet_type == CONNECT )
        {
            trace( " >> Connect packet is received " );
            /* check the state */
            if ( state != IDLE )
                {
                    /* Discard unexpected packet */
                    reject_packet ( buffer, UNEXPECTED_PACKET );
                    return;
                 }
                 
            /* set the channel id for this connection */
            channel_id = buffer[2] ^ xkey;

            /* set the state to connected */
            state = CONNECTED;
            
            /***********************************************************/            
            /* connect response */
            unsigned char res_buffer[CONNECT_PACKET_LENGTH];
            
            /* pt = connect response */
            res_buffer[0] = CONNECT;
            res_buffer[0] <<= 1;
            res_buffer[0] &= RESPONSE;
            
            /* pkt length */
            res_buffer[1] = CONNECT_PACKET_LENGTH;
            
            /* channel - id */
            res_buffer[2] = buffer[2];
            
            while ( proto_tx( res_buffer, CONNECT_PACKET_LENGTH ) != 0 ){;}
        }/* 1. CONNECT PACKETS */
        
    /* 2. CONFIG PACKETS */
    if ( packet_type == CONFIG )
        {
            trace( " >> Config packet is recieved  " );
            /* check the state to be connected */
            if ( state != CONNECTED )
                {
                    /* discard the unexpected packet */
                    reject_packet ( buffer, UNEXPECTED_PACKET );
                    return;
                }
            
            /* set the channel id for this connection */
            channel_id = buffer[2] ^ xkey;
                 
            /* check the config parameters */
            int config_param;
            config_param = buffer[3] ^ xkey;
            config_param <<= 8;
            config_param &= buffer[4] ^ xkey;
            
            if ( ( config_param != CF1) && ( config_param != CF2 ) && ( config_param != CF3  ) && (config_param != CF4 ) )
                { 
                    /* config parameters are wrong */
                    reject_packet( buffer, KEY_MISMATCH );
                    return;
                }
            /******************************************************/
            /* config response */
            state = CONFIGURED;
            unsigned char res_buffer[CONFIG_PACKET_LENGTH];
            
            /* pt and cr */
            res_buffer[0] = buffer[0] & RESPONSE;
            
            /* packet length */
            res_buffer[1] = CONFIG_PACKET_LENGTH;
            
            /* channel -id */
            res_buffer[2] = buffer[2];
            
            /* config params same as input */
            res_buffer[3] = buffer[3];
            res_buffer[4] = buffer[4];
            
            while ( proto_tx( res_buffer, CONFIG_PACKET_LENGTH ) != 0 ){;}
        }/* 2. CONFIG PACKETS */
         
    /* 3. DISCONNECT PACKETS */   
    if ( packet_type = DISCONNECT )
        {
            trace( " >> Disconnect packet is recieved" );
            if ( ( state != CONFIGURED ) || ( state != CONNECTED ) )
                {
                    /* discard the unexpected packet */
                    reject_packet ( buffer, UNEXPECTED_PACKET );
                    return;
                }
            state = IDLE;
            channel_id = 0;
         }
         
     /* 4. REJECTED PACKETS */
     if ( packet_type == REJECT )
        {
            trace ( " >> Rejected packet is recieved in reply to my packet" ); 
            int reason;
            reason = buffer[4];
            int my_packet;
            my_packet = buffer[3];
            printf (" MY packet : %d is rejected because of : %d \n ", my_packet, reason );
         }
}

/* Sent a response to all rejected packets */
void reject_packet ( unsigned char * buffer, int reason )
{
            trace ( ">> Received Packet is rejected " );
            unsigned char res_buffer[5];
            
            /* pt = reject */
            res_buffer[0] = REJECT;
            res_buffer[0] <<= 1;
            /* cr = 0 */
            res_buffer[0] &= RESPONSE;
            
            /* pkt length */
            res_buffer[1] = REJECT_PACKET_LENGTH;
            
            /* channel - id */
            res_buffer[2] = buffer[2];
            
            /* Rx'd packet Type */
            res_buffer[3] = ( buffer[0] >> 1 ) ^ xkey;
            
            /* Reason */
            res_buffer[4] = reason ^ xkey;
            
            while ( proto_tx( res_buffer, REJECT_PACKET_LENGTH ) != 0 ){;}
}
