/*
   wireshark로 패킷을 캡처해 나의 컴퓨터가 내가 지정한 컴퓨터의 
   MAC 주소를 알아오기 위한 코드를 짜봄
*/   





#include <stdio.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <string.h>
#include <netinet/in.h>


//이더넷 헤더 구성
struct eth_hdr{
	unsigned char h_dest[6];
	unsigned char h_source[6];
	unsigned short h_type;
} __attribute__((packed));
//arp 헤더 구성
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
	sll.sll_ifindex = if_nametoindex("enp0s8");
	sll.sll_halen =6;

	g_sock = socket(PF_PACKET, SOCK_RAW, 0);

	
	//도착지 이더넷 헤더를 모르기 때문에 broadcast로  구성
	ether.h_dest[0] = 0xff;
	ether.h_dest[1] = 0xff;
	ether.h_dest[2] = 0xff;
	ether.h_dest[3] = 0xff;
	ether.h_dest[4] = 0xff;
	ether.h_dest[5] = 0xff;
	
	//수신자(나)의 MAC 주소 입력
	ether.h_source[0] = 0x08;	
	ether.h_source[1] = 0x00;
	ether.h_source[2] = 0x27;
	ether.h_source[3] = 0xf2;
	ether.h_source[4] = 0x82;
	ether.h_source[5] = 0x1b;
	
	//arp임을 나타냄
	ether.h_type = htons(0x0806);
	
	
	arp.ar_hrd = htons(0x0001);
	arp.ar_pro = htons(0x0800);
	arp.ar_hln = 0x06;
	arp.ar_pln = 0x04;
	arp.ar_op = htons(0x0001);
	
	//출발지 MAC 주소
	arp.ar_sha[0] = 0x08;	
	arp.ar_sha[1] = 0x00;
	arp.ar_sha[2] = 0x27;
	arp.ar_sha[3] = 0xf2;
	arp.ar_sha[4] = 0x82;
	arp.ar_sha[5] = 0x1b;
	
	//출발지 IP 주소
	arp.ar_sip[0] = 0xc0;
	arp.ar_sip[1] = 0xa8;
	arp.ar_sip[2] = 0x16;
	arp.ar_sip[3] = 0x04;
	
	//도착지 MAC주소 모르기 때문에 비워놓음
	arp.ar_tha[0] = 0x00;
	arp.ar_tha[1] = 0x00;
	arp.ar_tha[2] = 0x00;
	arp.ar_tha[3] = 0x00;
	arp.ar_tha[4] = 0x00;
	arp.ar_tha[5] = 0x00;
	
	//도착지 IP는 안다는 가정 하에 진행함, VM2의 IP 주소
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



