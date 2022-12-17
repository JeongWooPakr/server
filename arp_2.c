#include <stdio.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <string.h>
#include <netinet/in.h>

struct eth_hdr{
	unsigned char h_dest[6];
	unsigned char h_source[6];
	unsigned short h_type;
} __attribute__((packed));

struct arp_hdr{
	unsigned short ar_hrd;
	unsigned short ar_pro;
	unsigned char ar_hln;
	unsigned char ar_pln;
	unsigned short ar_op;
	unsigned char ar_sha[6];
	unsigned char ar_sip[4];
	unsigned char ar_tha[6];
	unsigned char ar_tip[4];
}__attribute__((packed));

static unsigned char g_buf[sizeof(struct eth_hdr)+sizeof(struct arp_hdr)];
static int g_sock;

int main(int argc, char *argv[]){
	struct eth_hdr ether;
	struct arp_hdr arp;
	
	struct sockaddr_ll sll;

	bzero(&sll, sizeof(sll));
	memset(&sll, 0, sizeof(sll));

	sll.sll_family = PF_PACKET;
	sll.sll_ifindex = if_nametoindex("eth0");
	sll.sll_halen =6;

	g_sock = socket(PF_PACKET, SOCK_RAW, 0);

	ether.h_dest[0] = 0xff;
	ether.h_dest[1] = 0xff;
	ether.h_dest[2] = 0xff;
	ether.h_dest[3] = 0xff;
	ether.h_dest[4] = 0xff;
	ether.h_dest[5] = 0xff;

	ether.h_source[0] = 0x08;	
	ether.h_source[1] = 0x00;
	ether.h_source[2] = 0x27;
	ether.h_source[3] = 0xf2;
	ether.h_source[4] = 0x82;
	ether.h_source[5] = 0x1b;

	ether.h_type = htons(0x0806);

	arp.ar_hrd = htons(0x0001);
	arp.ar_pro = htons(0x0800);
	arp.ar_hln = 0x06;
	arp.ar_pln = 0x04;
	arp.ar_op = htons(0x0001);

	arp.ar_sha[0] = 0x08;	
	arp.ar_sha[1] = 0x00;
	arp.ar_sha[2] = 0x27;
	arp.ar_sha[3] = 0xf2;
	arp.ar_sha[4] = 0x82;
	arp.ar_sha[5] = 0x1b;

	arp.ar_sip[0] = 0xc0;
	arp.ar_sip[1] = 0xa8;
	arp.ar_sip[2] = 0x16;
	arp.ar_sip[3] = 0x04;

	arp.ar_tha[0] = 0x00;
	arp.ar_tha[1] = 0x00;
	arp.ar_tha[2] = 0x00;
	arp.ar_tha[3] = 0x00;
	arp.ar_tha[4] = 0x00;
	arp.ar_tha[5] = 0x00;

	arp.ar_tip[0] = 0xc0;
	arp.ar_tip[1] = 0xa8;
	arp.ar_tip[2] = 0x16;
	arp.ar_tip[3] = 0x05;

	memcpy(g_buf, &ether, sizeof(struct eth_hdr));
	memcpy(g_buf+14, &arp, sizeof(struct arp_hdr));

	sendto(g_sock,g_buf, 
		sizeof(struct eth_hdr) + sizeof(struct arp_hdr), 0, 
		(struct sockaddr*)&sll, sizeof(struct sockaddr_ll));
	return 0;

}



