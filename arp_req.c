#include <stdio.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>

typedef unsigned char uchar;
typedef unsigned char short ushort;

struct eth_hdr
{

	uchar h_dest[6];
	uchar h_source[6];
	ushort h_proto;
} __attribute__((packed));


static const int ETHERNET_SIZE = sizeof(struct eth_hdr);

struct arp_hdr
{
	ushort ar_hrd;
	ushort ar_pro;
	uchar ar_hln;
	uchar ar_pln;
	ushort ar_op;
	uchar ar_sha[6];
	uchar ar_sip[4];
	uchar ar_tha[6];
	uchar ar_tip[4];
} __attribute__((packed));

static const int ARP_SIZE = sizeof(struct arp_hdr);

static uchar g_buff[sizeof(struct eth_hdr)+sizeof(struct arp_hdr)];
static const char *g_source_ip = NULL;
static const har *g_interface = NULL;
static int g_sock = -1;

void dump(const uchar *data_buffer, const unsigned int lenght){
	uchar byte;
	unsigned int i, j;
	for(i=0; i<lenght;i++){
		printf("%02x", data_buffer[i]);
		if(((i%16)==15) || (i==lenght-1)){
			for(j=0;j<15-(i%16);j++)
				printf("   ");
			printf("|  ");
			for (j=(i-(i%16));j<=i;j++){
				byte = data_buffer[j];
				if((byte>31) && (byte<127))
					printf("%c",byte);
				else
					printf(". ");
			}

			printf("\n");
		}
	}



