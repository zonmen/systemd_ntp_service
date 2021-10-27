#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define TIME_DIFF 2208988800ull


int main( )
{
  struct timeval sys_time;
  int my_socket, count;
  int port = 123;
  char* host_address = "us.pool.ntp.org";

  //struct to get data from NTP, 48 bytes
  typedef struct
  {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint32_t refId;
    uint32_t refTm_s;
    uint32_t refTm_f;
    uint32_t origTm_s;
    uint32_t origTm_f;
    uint32_t rxTm_s;
    uint32_t rxTm_f;
    uint32_t txTm_s;
    uint32_t txTm_f;

  } ntp_structure;

  //zero packet
  ntp_structure packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  memset( &packet, 0, sizeof( ntp_structure ) );

  // 00,011,011 for li = 0, vn = 3, and mode = 3
  *( ( char * ) &packet + 0 ) = 0x1b;

  struct sockaddr_in server_address;
  struct hostent *server;
  //create udp socket
  my_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

  if ( my_socket < 0 ){
    printf( "ERROR opening socket" );
    exit( 0 );
  }
  server = gethostbyname( host_address );

  if ( server == NULL ){
	  printf( "ERROR, no such host" );
	  exit( 0 );
  }

  //zero server address
  bzero( ( char* ) &server_address, sizeof( server_address ) );
  server_address.sin_family = AF_INET;
  //copy server ip to server address
  bcopy( ( char* )server->h_addr, ( char* ) &server_address.sin_addr.s_addr, server->h_length );
  server_address.sin_port = htons( port );

  if ( connect( my_socket, ( struct sockaddr * ) &server_address, sizeof( server_address) ) < 0 ){
	  printf( "ERROR connecting" );
  	  exit( 0 );
	}

  count = write( my_socket, ( char* ) &packet, sizeof( ntp_structure ) );

  if ( count < 0 ) {
	  printf( "ERROR writing to socket" );
	  exit( 0 );
  }

  count = read( my_socket, ( char* ) &packet, sizeof( ntp_structure ) );

  if ( count < 0 ){
	  printf( "ERROR reading from socket" );
	  exit( 0 );
  }

  //get sys time to compare
  gettimeofday(&sys_time, NULL);
  //translate if need bits in right order
  packet.txTm_s = ntohl( packet.txTm_s );
  packet.txTm_f = ntohl( packet.txTm_f );
  //subtract time difference
  //NTP time format starts date from 1900, but unix from 1970
  packet.txTm_s -= TIME_DIFF;

  //count difference in seconds
  unsigned int dif_sec = packet.txTm_s - sys_time.tv_sec;
  //count difference in microseconds
  unsigned int dif_microsec = packet.txTm_f - sys_time.tv_usec;
  //return these differences in stdout
  printf("Difference between NTP and SYS time is %d.%u", dif_sec, dif_microsec);
  return 0;
}
