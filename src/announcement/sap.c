/*
 * Licence: GPL
 * Created: Wed, 02 Mar 2016 09:12:58 +0100
 * Main authors:
 *     - hoel <hoel.vasseur@openwide.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <gstreamer-1.0/gst/gst.h>
#include <gstreamer-1.0/gst/sdp/gstsdpmessage.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/* Header file */
#include "../../include/mibParameters.h"
#include "../../include/deviceInfo/ethernetIfTable.h"
#include "../../include/channelControl/channelTable.h"
#include "../../include/announcement/sdp.h"
#include "../../include/announcement/sap.h"
#include "../../include/streaming/stream_registration.h"
#include "../../include/streaming/stream.h"

/*
 * BYTE 0:
 * 	Version Number (V): 3 bits
		Specifies the SAP version. This field shall be set to 1.
	Addsap_multicast_addrs Type (A): 1 bit
		This bit shall be set to 0 to indicate that 32-bit IPv4 addsap_multicast_addrses are used.
	Reserved (R): 1 bit
		This bit shall be set to 0.
	Message Type (T): 1 bit
		This bit shall be set to 0 for session announcements and 1 for session deletions.
	Encryption (E): 1 bit
		SDP payload encryption shall not be used. This bit shall therefore be set to 0 for an unencrypted payload.
	Compsap_multicast_addrsed (C): 1 bit
		SDP payload compsap_multicast_addrsion shall not be used. This bit shall therefore be set to 0 for an uncompsap_multicast_addrsed payload
	SO te binary version is: 00100000 thus 0x20
*/
#define SAP_header_byte0 	0x20

/* 
 * BYTE 1:
 * Authentication Length: 8 bits
 *  This shall be set to 0 in VIVOE as no authentication data is psap_multicast_addrent
 */
#define SAP_header_AL 		0x00

/*
 * BYTE 9 and more:
 * Optional Payload Type:
 * This is a ASCII text string followed by null character.
 * this shall be set to "application/sdp"
 */
#define SAP_header_OPT 		"application/sdp"

/* 
 * After the really hard and scholar computation I have deduced that the total
 * size of the header of the SAP is 24 bytes:
 * byte 0 as described bedfore 	--> 	1 byte
 * Authentication Length: 		--> + 	1 byte
 * Message Identifier Hash: 	--> + 	2 bytes
 * Operating Source: 			--> + 	4 bytes
 * Optional Payload Type: 		--> + 	16 bytes
 * 									_____________
 * Total: 								24 bytes
 */
#define SAP_header_size 	24

/**
 * \brief 	Build the first byte corsap_multicast_addrponding to the Message Identifier Hash (MAI) according to the norm
 * \return 	the 16 bits corsap_multicast_addrponding to the Message Identifier Hash field of SAP header
 */
static const uint16_t build_MAI(int session_version){
	/*
	 * the Message Identifier Hash should be derived from the session version number of the SDP message
	 * As the Session Version is a 32-bits integer and the MAI is only a 16-bits field, we have to take only
	 * a part of the Session Version, but has the session version shall be increased by 1 each time a new SDP
	 * message is created, we should reverberate this on the MAI. So we will take only the 16 LSB of the session version
	 */
	return (char) (session_version & 0x00FF);
}

/**
 * \brief 	Build the 4 bytes of SAP header corsap_multicast_addrponding to Operating Source field
 * 			This should contained the IP source of the Service Provider
 * \return 	in_addr_t (uint32_t) the 32 bits corsap_multicast_addrponding to the IP of the Service Provider
 */
static const in_addr_t build_OS(void){
	/*
	 * VIVOE norm specifies that the field Operating Source of the SAP header should contain the IP of the Service Provider
	 * This IP is the IP of the Primary interface that we retrieve from the ethernetIfTable
	 */

	/* Primary interface is the head of the list of entries of ethernetIfTable */
	return ethernetIfTable_head->ethernetIfIpAddress;
}

/**
 * \brief 	build the SAP header of datagram, as defined by the VIVOE nor
 * \param  	session_version the number of session_version to use to create the Message Identifier Hash
 * \return 	char* an array of byte corsap_multicast_addrponding to the built header
 */
static gboolean build_SAP_header(char *header, int session_version){
	

	char returnv[SAP_header_size];
	int offset 						= 0; /* an offset use to concatenate the arrays */

	returnv[0] = SAP_header_byte0;
	offset++;
	returnv[1] = SAP_header_AL;
	offset++;
	uint16_t mai = build_MAI(session_version);
	memcpy(returnv+offset, &mai, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	in_addr_t  operatig_source  = build_OS();
	memcpy(returnv+offset, &operatig_source, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(returnv+offset, SAP_header_OPT , sizeof(SAP_header_OPT)/sizeof(char) );
	offset += sizeof(SAP_header_OPT)/sizeof(char);
	memcpy( header,returnv,SAP_header_size );
	return TRUE;	
}

static char*  build_SAP_msg(gpointer entry, int *sap_msg_length){

	GstSDPMessage *msg;
	/* create a new SDP message */
	if (gst_sdp_message_new(&msg)){
		g_printerr("Failed to create SDP message\n");
		return FALSE;
	}
	create_SDP(msg, entry);

	/* extract the randomly generated session_version integer */
	const GstSDPOrigin 	*origin 			= gst_sdp_message_get_origin (msg);
	int 				session_version 	= htonl(atoi(origin->sess_version));

	/* Build the  header */
	char header[SAP_header_size];
   	build_SAP_header(header, session_version);

	/* Build the *payload */
	char *payload = g_strdup (gst_sdp_message_as_text(msg));

	/* concat header and payload */
	int 	sap_msg_size 	= SAP_header_size+strlen(payload);
	char 	*sap_msg 		= (char*) malloc(sap_msg_size*sizeof(char));
	memcpy(sap_msg, header, SAP_header_size);
	memcpy(sap_msg+SAP_header_size, payload, strlen(payload));
	*sap_msg_length = sap_msg_size; // save the length if the message into sap_msg_length
	return sap_msg;
}


/**
 * \biref a structure to registeer the information to send the UDP datagram
 */
typedef struct {
	struct 	addrinfo 	*sap_multicast_addr; 
	int 				udp_socket_fd;
	char 				*udp_payload;
	int 				udp_payload_length;
}udp_data;

udp_data udp_datas;

/**
 * \brief This function will send the SAP message, encapsulate into the payload of an UDP datagram, on the UDP socket with IP: 224.2.127.254 port 9875.
 * \gpointer data, the data to pass to the function (an entry in the channelTable)
 * \gboolean TRUE on succed, FALSE on failure, failure happened if the channel is in stop mode 
 */
gboolean prepare_socket(gpointer entry){
	/* define multicast and port number for SAP */
	const char* hostname="224.2.127.254";
	const char* portname="9875";

	/* define parameter for the address to use in a variable named sap_addr_info */
	struct 	addrinfo sap_addr_info; 	
	memset(&sap_addr_info,0,sizeof(sap_addr_info));
	sap_addr_info.ai_family=AF_INET;
	sap_addr_info.ai_socktype=SOCK_DGRAM;
	sap_addr_info.ai_protocol=0;
	sap_addr_info.ai_flags=AI_ADDRCONFIG;
	
	/* initialize sap_multicast_addr member of udp_datas structure */
   //	udp_datas->sap_multicast_addr;
	int err=getaddrinfo(hostname,portname,&sap_addr_info,&(udp_datas.sap_multicast_addr));
	if (err!=0) {
		g_printerr("failed to open remote socket (err=%d)\n",err);
		return FALSE;		
	}
	
//	printf("%d\n",((struct sockaddr_in*) sap_multicast_addr->ai_addr)->sin_port);
	
	/* open UDP socket */
	int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);	
	if (udp_socket_fd==-1) {
		g_printerr(" Failed to create UDP socket to send SAP/SDP announcement: %s\n",strerror(errno));
		return FALSE;		
	}
	/* save value of socket into global structure */	
	udp_datas.udp_socket_fd = udp_socket_fd;

	/* create UDP payload and save the payload into udp_datas structure */
	udp_datas.udp_payload_length = 0;
	udp_datas.udp_payload =  build_SAP_msg(entry, &(udp_datas.udp_payload_length));	

/*	for(int i = 0; i< sizeof(udp_datas.udp_payload); i++)
		printf("%02X\t%d\n", udp_datas.udp_payload[i], i);*/

	return TRUE;
}

gboolean send_announcement(gpointer entry){

	/* check channel status, this is check as a stop condition
	 * if the status in stop, we return false, which means that 
	 * we will stop to call repeteadly create_SDP
	 */

/*	for(int i = 0; i<udp_datas.udp_payload_length ; i++)
		printf("%02X\t%d\n", udp_datas.udp_payload[i], i);*/


	struct channelTable_entry * channel_entry = entry;
	if( channel_entry->channelStatus == stop ){
		/* Build and send deletion packet */
		return FALSE;
	}
	/* Build the UDP payload */
	int n_bytes = sendto( 	udp_datas.udp_socket_fd,
							udp_datas.udp_payload,
							udp_datas.udp_payload_length,
							0,
							udp_datas.sap_multicast_addr->ai_addr,
							udp_datas.sap_multicast_addr->ai_addrlen);
	/* check if the number of bytes send is -1, if so an error as occured */
	if ( n_bytes	== -1 )
	{
    	g_printerr("Failed to send SAP/SDP announcement: %s",strerror(errno));
		return FALSE;
	}
	return TRUE;
}

