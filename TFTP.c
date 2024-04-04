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

ISKE AAGE SOCKET PROG H. MUJHE WO NAI AATA. YE SAT SUN MAI SEEKH DAALUGI. TUMHE SEEKHNA HO TO https://www.youtube.com/watch?v=_lQ-3S4fJ0U&list=PLPyaR5G9aNDvs6TtdpLcVO43_jvxp4emI DEKH LO. 
