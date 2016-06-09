/*
 * Note: this file originally auto-generated by mib2c using
 *  $
 */
#ifndef CHANNELTABLE_H
#define CHANNELTABLE_H

/* function declarations */
void init_channelTable(void);
void initialize_table_channelTable(void);
Netsnmp_Node_Handler channelTable_handler;
Netsnmp_First_Data_Point  channelTable_get_first_data_point;
Netsnmp_Next_Data_Point   channelTable_get_next_data_point;
NetsnmpCacheLoad channelTable_load;
NetsnmpCacheFree channelTable_free;

#define CHANNELTABLE_TIMEOUT  60

/* column number definitions for table channelTable */
       #define COLUMN_CHANNELINDEX							1
       #define COLUMN_CHANNELTYPE							2
       #define COLUMN_CHANNELUSERDESC						3
       #define COLUMN_CHANNELSTATUS							4
       #define COLUMN_CHANNELVIDEOFORMATINDEX				5
       #define COLUMN_CHANNELVIDEOFORMAT					6
       #define COLUMN_CHANNELVIDEOSAMPLING					7
       #define COLUMN_CHANNELVIDEOBITDEPTH					8
       #define COLUMN_CHANNELFPS							9
       #define COLUMN_CHANNELCOLORIMETRY					10
       #define COLUMN_CHANNELINTERLACED						11
       #define COLUMN_CHANNELCOMPRESSIONFACTOR				12
       #define COLUMN_CHANNELCOMPRESSIONRATE				13
       #define COLUMN_CHANNELHORZRES						14
       #define COLUMN_CHANNELVERTRES						15
       #define COLUMN_CHANNELROIORIGINTOP					16
       #define COLUMN_CHANNELROIORIGINLEFT					17
       #define COLUMN_CHANNELROIEXTENTBOTTOM				18
       #define COLUMN_CHANNELROIEXTENTRIGHT					19
       #define COLUMN_CHANNELRTPPT							20
       #define COLUMN_CHANNELRECEIVEIPADDRESS				21
       #define COLUMN_CHANNELINTERPACKETDELAY				22
       #define COLUMN_CHANNELSAPMESSAGEINTERVAL				23
       #define COLUMN_CHANNELDEFAULTVIDEOFORMATINDEX		24
       #define COLUMN_CHANNELDEFAULTRECEIVEIPADDRESS		25

    /* Typical data structure for a row entry */
struct channelTable_entry {
    /* Index values */
    long channelIndex;

    /* Column values */
    long channelType;
    char* channelUserDesc;
    size_t channelUserDesc_len;
    char* old_channelUserDesc;
    size_t old_channelUserDesc_len;
    long channelStatus;
    long old_channelStatus;
    long channelVideoFormatIndex;
    long old_channelVideoFormatIndex;
    char* channelVideoFormat;
    size_t channelVideoFormat_len;
    char* channelVideoSampling;
    size_t channelVideoSampling_len;
    long channelVideoBitDepth;
    long channelFps;
    char* channelColorimetry;
    size_t channelColorimetry_len;
    long channelInterlaced;
    long channelCompressionFactor;
    long channelCompressionRate;
    long old_channelCompressionRate;
    long channelHorzRes;
    long old_channelHorzRes;
    long channelVertRes;
    long old_channelVertRes;
    long channelRoiOriginTop;
    long old_channelRoiOriginTop;
    long channelRoiOriginLeft;
    long old_channelRoiOriginLeft;
    long channelRoiExtentBottom;
    long old_channelRoiExtentBottom;
    long channelRoiExtentRight;
    long old_channelRoiExtentRight;
    long channelRtpPt;
    in_addr_t channelReceiveIpAddress;
    in_addr_t old_channelReceiveIpAddress;
    long channelInterPacketDelay;
    long old_channelInterPacketDelay;
    long channelSapMessageInterval;
    long old_channelSapMessageInterval;
    long channelDefaultVideoFormatIndex;
    long old_channelDefaultVideoFormatIndex;
    in_addr_t channelDefaultReceiveIpAddress;
    in_addr_t old_channelDefaultReceiveIpAddress;

	/* Other values required by OpenVivoe, but not in the MIB */
	gpointer stream_datas; /* a stream assoicated to the channel */
	sap_data *sap_datas; /* the SAP/SDP announcement associated to the stream and the channel */

	/*
	 * These are tow variable used to save the SDP resolution.
	 * They will be used when the users tries to modifies the ROI origin and extent parameters or channel resolution parameters
	 * to be sure that this won't set wrong values to vivoecrop and vivoecaps element
	 */
	long sdp_width;
	long sdp_height;

    /* Illustrate using a simple linked list */
    int   valid;
    struct channelTable_entry *next;
    struct channelTable_entry *next_SU;

};

/**\brief head of the channelTalbe
 */
struct channelTable_entry  *channelTable_head;

/** \brief head of the Service USezr channelTalbe
 */
struct channelTable_entry  *channelTable_SU_head;

/* create a new row in the (unsorted) table */
struct channelTable_entry * channelTable_createEntry(
    	        									long 		channelIndex,
													long 		channelType,
 												 	char* 		channelUserDesc,
    												long 		channelStatus,
												    long 		channelVideoFormatIndex,
												    char* 		channelVideoFormat,
												    char* 		channelVideoSampling,
												    long 		channelVideoBitDepth,
													long 		channelFps,
													char* 		channelColorimetry,
												    long 		channelInterlaced,
												    long 		channelCompressionFactor,
												    long 		channelCompressionRate,
    												long 		channelHorzRes,
    												long 		channelVertRes,
							  						long 		channelRoiOriginTop,
							  					  	long 		channelRoiOriginLeft,
							  					  	long 		channelRoiExtentBottom,
												  	long 		channelRoiExtentRight,
						  						  	long 		channelRtpPt,
						 						  	in_addr_t 	channelReceiveIpAddress,
						   						 	long 		channelInterPacketDelay,
												 	long 		channelSapMessageInterval,
					 							   	long 		channelDefaultVideoFormatIndex,
						 						   	in_addr_t 	channelDefaultReceiveIpAddress,
													gpointer 	stream_datas);

gboolean channelTable_fill_entry(struct channelTable_entry * entry, struct videoFormatTable_entry *videoFormatentry);

struct channelTable_entry *	channelTable_create_empty_entry(int index,long channel_type, char *channelUserDesc , long videoFormatNumber ,  long default_IP_address, gpointer stream_datas );

struct channelTable_entry * channelTable_getEntry(int index);

struct channelTable_entry * channelTable_get_from_VF_index(int index);

gboolean update_videoFormat_entry_roi_from_channelTable_entry ( struct videoFormatTable_entry *videoFormat_entry , struct channelTable_entry *channel_entry ) ;

void update_channelTable_entry_roi_from_caps ( struct channelTable_entry *channel_entry , GstCaps *caps ) ;

void channelTable_delete();

int channelSatus_requests_handler( struct channelTable_entry * table_entry );

#endif /* CHANNELTABLE_H */
