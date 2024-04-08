#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#inlcude <stdio.h>
#include <stdlib.h>
#include <ctype.h>
/************************************************
*       FUNCTION DECLARATIONS AND CONSTANTS
*************************************************/
enum OP_code {RRQ = 1, WRQ, DATA, ACK, ERROR }: //Operation codes to define packets
FILE* resolve_packet( char packet[],unsigned short* blockno, unsigned short* potential_err, FILE* file, char filename[], char mode[], char folder[], char IP[], );
void terminate_prog();          // Terminates program upon an error
FILE* resolve_rrq( char filename[], char mode[], char packet[], char folder[] ); //Resolves read req and opens file/s
FILE* open_file( char filename[], char folder[] );   //Open filename and folder  
void parse_path( char filename[], char folder[], char full_path[] );  //Gives path given dcty and folder
int bundle_data ( unsigned short blockno, FILE* file, char packet[] ); //Formats data from file to pkt
void get_error_packet( unsigned short err_code, char err_msg[], char packet[] ); //Formats an err pkt
int blockno_acknowl( char packet[], unsigned short blockno ); //checks for echo of data packet blocknumber by ack packet
int validate_filename( char filename[] );  //Checking if client remains in directory specified by server  

/**************************************************
*              MAIN FUNCTION
***************************************************/
int main(int argc, char* argv[] ){
//2 cmd lines need to be sent
    if (argc==3) {
// Command line arguments valid: feedback
		printf( "Server listening on port: %s\n", argv[1]);

// Initialization of recieved pkt from server
// Is 4B + 512B = 516B (header and data sections)
// Block number to keep track of data pkts cons
		char packet[516] = "\0"; char prev_packet[516] = "\0";
		unsigned short blockno = 1;

// Program times out when value reaches 10 tries on same pkt
		unsigned short timeout_cnter = 0;
// Creating a socket using default protocol
// Server initialized/null reseted
// Then defining domain that server can communicate with
		int sockfd; struct sockaddr_in server, client;
		sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
		memset( &server, 0, sizeof( server ) );
		server.sin_family = AF_INET;

// Converting arguments from host byte order to network byte order
// Then binding address to socket using arguments
		server.sin_addr.s_addr = htonl( INADDR_ANY );
		server.sin_port = htons( atoi( argv[1] ) );
		bind( sockfd, (struct sockaddr *) &server, (socklen_t) sizeof(server) );

// Initializing filename, mode and filestream
		char filename[50] = "\0"; char mode[50] = "\0"; FILE* file = NULL;
	    int packet_size = 512;
// Loop while transmitting
		while ( packet_size == 512) {
// Receive (at least) one byte less than declared
// Making room for the NULL-terminating character
			socklen_t len = (socklen_t) sizeof(client);
			ssize_t n = recvfrom	( sockfd, packet, sizeof(packet) - 1, 0, (struct sockaddr *) &client, &len );
			packet[n] = '\0';

// Potential error code
// Defines an error if an error exists
// Default error type: file not found
			unsigned short potential_err = 1;

// Get type of packet
// Calls function to resolve packet
			file = resolve_packet( packet, &blockno, &potential_err,file, filename, mode, argv[2],inet_ntoa( client.sin_addr ) );
// If file is NULL, error occurred while reading file
// Or a packet could not be resolved
			if ( file == NULL) {
// Form an error message appropriate to error type
				char err_msg[512] = "\0";
				if ( potential_err == 1 ) {
					strcat( err_msg, "ERROR: File name and/or directory could not be resolved" );
				} else if ( potential_err == 3 ) {
					potential_err = 0;
					strcat ( err_msg, "ERROR: Uploading files is not supported on this TFTP Server");
				} else {
					strcat( err_msg, "ERROR: Unable to resolve packet" );
				}

// Use code and message to bundle an error packet
				get_error_packet( potential_err, err_msg, packet );

// Feedback printed for server
				printf("%s\n", err_msg);

// Sends error packet to client, then terminates
				sendto( sockfd, packet, (size_t) packet_size + 4, 0,(struct sockaddr *) &client, len );
				terminate_prog( );
			}
			if ( potential_err != 2 ) {

// Passes data into corresponding packet
// IF data was previously sent
				packet_size = bundle_data( blockno, file, packet );
				*prev_packet = *packet;

// No potential error, timeout value reset
				timeout_cnter = 0;
			} else {

// If potential_err is 2, this means that we need to resent packet
// So, packet becomes previous packet before sendoff
				*packet = *prev_packet; timeout_cnter++;

// If we have tried to resend same packet 4 times WE GIVE UP ALL HOPE :(
// Send error packet with appropriate message to client and terminate transfer
				if (timeout_cnter >= 4){

					get_error_packet( 0, "ERROR: Transfer timed out", packet );
					sendto( sockfd, packet, (size_t) packet_size + 4, 0,(struct sockaddr *) &client, len );
					terminate_prog( );
				}
			}

// Sends data packet to client
			sendto(	sockfd, packet, (size_t) packet_size + 4, 0,(struct sockaddr *) &client, len );
		}

printf( "SUCCESS: File transferred\n" );
		if ( file ) { fclose( file ); }

	} else { printf( "ERROR: Incorrect number of parameters\n" ); terminate_prog( ); }

	return 0;
}
