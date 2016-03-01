/*
 * Note: this file originally auto-generated by mib2c using
 *  $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-unix.h>
#include <gstreamer-1.0/gst/gst.h>
#include "../../include/videoFormatInfo/videoFormatTable.h"
#include "../../include/channelControl/channelTable.h"
#include "../../include/handler.h"
#include "../../include/mibParameters.h"
#include "../../include/streaming/stream_registration.h"
#include "../../include/streaming/stream.h"
#include "../../include/streaming/sdp.h"

/** Initializes the channelTable module */
void
init_channelTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_channelTable();
}


/** Initialize the channelTable table by defining its contents and how it's structured */
void
initialize_table_channelTable(void)
{
    const oid channelTable_oid[] = {1,3,6,1,4,1,35990,3,1,3,3};
    const size_t channelTable_oid_len   = OID_LENGTH(channelTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;

    DEBUGMSGTL(("channelTable:init", "initializing table channelTable\n"));

    reg = netsnmp_create_handler_registration(
              "channelTable",     channelTable_handler,
              channelTable_oid, channelTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: channelIndex */
                           0 /* to terminate the list of parameters*/);
    table_info->min_column = COLUMN_CHANNELTYPE;
    table_info->max_column = COLUMN_CHANNELDEFAULTRECEIVEIPADDRESS;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = channelTable_get_first_data_point;
    iinfo->get_next_data_point  = channelTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );
}

/* create a new row in the (unsorted) table */
struct channelTable_entry *
	channelTable_createEntry(
    	        				long 		channelIndex,
								long  		channelType,
						    	char* 		channelUserDesc,
    							long 		channelStatus,
							    long 		channelVideoFormatIndex,
							    char*  		channelVideoFormat,
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
								gpointer 	stream_datas
                			) {
    struct channelTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct channelTable_entry);
    if (!entry)
        return NULL;

    entry->channelIndex 					= channelIndex;
	entry->channelType 						= channelType;
 	entry->channelUserDesc 					= strdup(channelUserDesc);
	entry->channelUserDesc_len 				= MIN(strlen(channelUserDesc), DisplayString64);
	entry->channelStatus 					= channelStatus;
	entry->channelVideoFormatIndex 			= channelVideoFormatIndex;
	entry->channelVideoFormat 	 			= strdup(channelVideoFormat);
	entry->channelVideoFormat_len			= MIN(strlen(channelVideoFormat), DisplayString16);
	entry->channelVideoSampling 			= strdup(channelVideoSampling);
	entry->channelVideoSampling_len			= MIN(strlen(channelVideoSampling), DisplayString16);	
	entry->channelVideoBitDepth 			= channelVideoBitDepth;
	entry->channelFps 						= channelFps;
	entry->channelColorimetry 				= strdup(channelColorimetry);
	entry->channelColorimetry_len 			= MIN(strlen(channelColorimetry), DisplayString16);	
	entry->channelInterlaced 				= channelInterlaced;
	entry->channelCompressionFactor 		= channelCompressionFactor;
	entry->channelCompressionRate 			= channelCompressionRate;
	entry->channelHorzRes 					= channelHorzRes;
	entry->channelVertRes 					= channelVertRes;
	entry->channelRoiOriginTop 				= channelRoiOriginTop;
	entry->channelRoiOriginLeft 			= channelRoiOriginLeft;
	entry->channelRoiExtentBottom 			= channelRoiExtentBottom;
	entry->channelRoiExtentRight 			= channelRoiExtentRight;
	entry->channelRtpPt 					= channelRtpPt;
	entry->channelReceiveIpAddress 			= channelReceiveIpAddress;
	entry->channelInterPacketDelay 			= channelInterPacketDelay;
	entry->channelSapMessageInterval 		= channelSapMessageInterval;
	entry->channelDefaultVideoFormatIndex 	= channelDefaultVideoFormatIndex;
	entry->channelDefaultReceiveIpAddress 	= channelDefaultReceiveIpAddress;

	entry->stream_datas 					= stream_datas;

    entry->next 		= channelTable_head;
	entry->valid 		= 1;
    channelTable_head 	= entry;
    return entry;
}
/**
 * \brief update an entry in the ChannelTable when changing its videoFormat
 * \param entry the entry in channelTable to update
 * \param videoFormatIndex the new videoFormatIndex to use for this channel
 * \return TRUE if we succeed to update the parameters
 */
gboolean channelTable_updateEntry(struct channelTable_entry * entry, int videoFormatNumberIndex){
       /* get the correspondante entry in the table of VideoFormat */
       struct videoFormatTable_entry *videoFormatentry         = videoFormatTable_getEntry( videoFormatNumberIndex );
       if ( videoFormatentry == NULL)
               return FALSE;
       /* upate ChannelTable_entry parameter with the ones retrieve from videoFormatTable */
       entry->channelVideoFormatIndex 						= videoFormatNumberIndex;
       entry->channelVideoFormat 							= strdup(videoFormatentry->videoFormatBase);
       entry->channelVideoFormat_len 						= MIN(strlen(videoFormatentry->videoFormatBase), 		DisplayString16);
       entry->channelVideoSampling 							= strdup(videoFormatentry->videoFormatSampling);
       entry->channelVideoSampling_len 						= MIN(strlen(videoFormatentry->videoFormatSampling), 	DisplayString16);
       entry->channelVideoBitDepth 							= videoFormatentry->videoFormatBitDepth;
       entry->channelFps 									= videoFormatentry->videoFormatFps;
       entry->channelColorimetry 							= strdup(videoFormatentry->videoFormatColorimetry);
       entry->channelColorimetry_len 						= MIN(strlen(videoFormatentry->videoFormatColorimetry), DisplayString16);
       entry->channelInterlaced 							= videoFormatentry->videoFormatInterlaced;
       entry->channelCompressionFactor 						= videoFormatentry->videoFormatCompressionFactor;
       entry->channelCompressionRate 						= videoFormatentry->videoFormatCompressionRate;
       entry->channelHorzRes 								= videoFormatentry->videoFormatMaxHorzRes;
       entry->channelVertRes 								= videoFormatentry->videoFormatMaxVertRes;
       /* update the stream data */
       entry->stream_datas 									= videoFormatentry->stream_datas;
       return TRUE;
}


#if  ALLOW_REMOVING_ROW
/* remove a row from the table */
void
channelTable_removeEntry( struct channelTable_entry *entry ) {
    struct channelTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = channelTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        channelTable_head = ptr->next;
    else
        prev->next = ptr->next;

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}
#endif //if ALLOW_REMOVING_ROW


void
channelTable_free( netsnmp_cache *cache, void *vmagic ) {
    struct channelTable_entry *this, *that;

    for ( this = channelTable_head; this; this=that ) {
        that = this->next;
        SNMP_FREE( this );   /* XXX - release any other internal resources */
    }
    channelTable_head = NULL;
}

/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
channelTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = channelTable_head;
    return channelTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
channelTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct channelTable_entry *entry = (struct channelTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->channelIndex );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}


/** handles requests for the channelTable table */
int
channelTable_handler(
    netsnmp_mib_handler             *handler,
    netsnmp_handler_registration    *reginfo,
    netsnmp_agent_request_info      *reqinfo,
    netsnmp_request_info            *requests) {

    netsnmp_request_info       		*request;
    netsnmp_table_request_info 		*table_info;
    struct channelTable_entry   	*table_entry;
	int 							ret 			= 0;
    DEBUGMSGTL(("channelTable:handler", "Processing request (%d)\n", reqinfo->mode));

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct channelTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_CHANNELTYPE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelType);
                break;
            case COLUMN_CHANNELUSERDESC:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->channelUserDesc,
                                          table_entry->channelUserDesc_len);
                break;
            case COLUMN_CHANNELSTATUS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelStatus);
                break;
            case COLUMN_CHANNELVIDEOFORMATINDEX:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelVideoFormatIndex);
                break;
            case COLUMN_CHANNELVIDEOFORMAT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->channelVideoFormat,
                                          table_entry->channelVideoFormat_len);
                break;
            case COLUMN_CHANNELVIDEOSAMPLING:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->channelVideoSampling,
                                          table_entry->channelVideoSampling_len);
                break;
            case COLUMN_CHANNELVIDEOBITDEPTH:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelVideoBitDepth);
                break;
            case COLUMN_CHANNELFPS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelFps);
                break;
            case COLUMN_CHANNELCOLORIMETRY:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->channelColorimetry,
                                          table_entry->channelColorimetry_len);
                break;
            case COLUMN_CHANNELINTERLACED:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelInterlaced);
                break;
            case COLUMN_CHANNELCOMPRESSIONFACTOR:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelCompressionFactor);
                break;
            case COLUMN_CHANNELCOMPRESSIONRATE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelCompressionRate);
                break;
            case COLUMN_CHANNELHORZRES:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelHorzRes);
                break;
            case COLUMN_CHANNELVERTRES:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelVertRes);
                break;
            case COLUMN_CHANNELROIORIGINTOP:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelRoiOriginTop);
                break;
            case COLUMN_CHANNELROIORIGINLEFT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelRoiOriginLeft);
                break;
            case COLUMN_CHANNELROIEXTENTBOTTOM:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelRoiExtentBottom);
                break;
            case COLUMN_CHANNELROIEXTENTRIGHT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelRoiExtentRight);
                break;
            case COLUMN_CHANNELRTPPT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelRtpPt);
                break;
            case COLUMN_CHANNELRECEIVEIPADDRESS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
				snmp_set_var_typed_value( request->requestvb, ASN_IPADDRESS,
                                         (u_char*) &table_entry->channelReceiveIpAddress, IPV4_SIZE );
                break;
            case COLUMN_CHANNELINTERPACKETDELAY:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelInterPacketDelay);
                break;
            case COLUMN_CHANNELSAPMESSAGEINTERVAL:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelSapMessageInterval);
                break;
            case COLUMN_CHANNELDEFAULTVIDEOFORMATINDEX:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->channelDefaultVideoFormatIndex);
                break;
            case COLUMN_CHANNELDEFAULTRECEIVEIPADDRESS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
				snmp_set_var_typed_value( request->requestvb, ASN_IPADDRESS,
                                         (u_char*) &table_entry->channelDefaultReceiveIpAddress, IPV4_SIZE );
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

        /*
         * Write-support
         */
    case MODE_SET_RESERVE1:
        for (request=requests; request; request=request->next) {
            table_entry = (struct channelTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_CHANNELUSERDESC:
				/* check this is at most a 64 byte string */
                ret = netsnmp_check_vb_type_and_max_size(
                          request->requestvb, ASN_OCTET_STR, DisplayString64);
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELSTATUS:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int_range( request->requestvb, start, singleFrame );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELVIDEOFORMATINDEX:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int_range( request->requestvb, 1, videoFormatNumber._value.int_val );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELCOMPRESSIONRATE:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELHORZRES:
                ret = netsnmp_check_vb_int_range( request->requestvb, 576, 1080 );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELVERTRES:
                ret = netsnmp_check_vb_int_range( request->requestvb, 576, 1920 );				
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELROIORIGINTOP:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int_range( request->requestvb, 0, 1080 );
				if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELROIORIGINLEFT:
                ret = netsnmp_check_vb_int_range( request->requestvb, 0, 1920 );				
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELROIEXTENTBOTTOM:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELROIEXTENTRIGHT:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELRECEIVEIPADDRESS:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELINTERPACKETDELAY:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELSAPMESSAGEINTERVAL:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELDEFAULTVIDEOFORMATINDEX:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int_range( request->requestvb, 1, videoFormatNumber._value.int_val );				
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_CHANNELDEFAULTRECEIVEIPADDRESS:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            default:
				netsnmp_set_request_error( reqinfo, request,
							SNMP_ERR_NOTWRITABLE );
					return SNMP_ERR_NOERROR;
            }
        }
        break;
	/* protect yoursefl against inconsistent (meaningless values) */
    case MODE_SET_RESERVE2:
			break;

	case MODE_SET_FREE:
		break;

    case MODE_SET_ACTION:
        for (request=requests; request; request=request->next) {
            table_entry = (struct channelTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
            switch (table_info->colnum) {
            case COLUMN_CHANNELUSERDESC:
				table_entry->old_channelUserDesc = strdup(table_entry->channelUserDesc);
                table_entry->old_channelUserDesc_len =
                        table_entry->channelUserDesc_len;
                strcpy ( table_entry->channelUserDesc,
                        (char*) request->requestvb->val.string);
                table_entry->channelUserDesc_len =
                        request->requestvb->val_len;
                break;
            case COLUMN_CHANNELSTATUS:
                table_entry->old_channelStatus 					= table_entry->channelStatus;
                table_entry->channelStatus     					= *request->requestvb->val.integer;
				if ( table_entry->channelStatus == start){
					g_timeout_add_seconds (1,create_SDP, table_entry )	;				
					start_streaming( table_entry->stream_datas, table_entry->channelVideoFormatIndex);
				}
				else if ( table_entry->channelStatus == stop){
					stop_streaming( table_entry->stream_datas, table_entry->channelVideoFormatIndex );
				}
                break;
            case COLUMN_CHANNELVIDEOFORMATINDEX:
                table_entry->old_channelVideoFormatIndex 		= table_entry->channelVideoFormatIndex;
                table_entry->channelVideoFormatIndex     		= *request->requestvb->val.integer;
				channelTable_updateEntry(table_entry, table_entry->channelVideoFormatIndex);
                break;
            case COLUMN_CHANNELCOMPRESSIONRATE:
                table_entry->old_channelCompressionRate 		= table_entry->channelCompressionRate;
                table_entry->channelCompressionRate     		= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELHORZRES:
                table_entry->old_channelHorzRes 				= table_entry->channelHorzRes;
                table_entry->channelHorzRes     				= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELVERTRES:
                table_entry->old_channelVertRes 				= table_entry->channelVertRes;
                table_entry->channelVertRes     				= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELROIORIGINTOP:
                table_entry->old_channelRoiOriginTop 			= table_entry->channelRoiOriginTop;
                table_entry->channelRoiOriginTop     			= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELROIORIGINLEFT:
                table_entry->old_channelRoiOriginLeft 			= table_entry->channelRoiOriginLeft;
                table_entry->channelRoiOriginLeft     			= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELROIEXTENTBOTTOM:
                table_entry->old_channelRoiExtentBottom 		= table_entry->channelRoiExtentBottom;
                table_entry->channelRoiExtentBottom     		= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELROIEXTENTRIGHT:
                table_entry->old_channelRoiExtentRight 			= table_entry->channelRoiExtentRight;
                table_entry->channelRoiExtentRight    			= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELRECEIVEIPADDRESS:
                table_entry->old_channelReceiveIpAddress 		= table_entry->channelReceiveIpAddress;
                table_entry->channelReceiveIpAddress     		= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELINTERPACKETDELAY:
                table_entry->old_channelInterPacketDelay 		= table_entry->channelInterPacketDelay;
                table_entry->channelInterPacketDelay     		= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELSAPMESSAGEINTERVAL:
                table_entry->old_channelSapMessageInterval 		= table_entry->channelSapMessageInterval;
                table_entry->channelSapMessageInterval     		= *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELDEFAULTVIDEOFORMATINDEX:
                table_entry->old_channelDefaultVideoFormatIndex = table_entry->channelDefaultVideoFormatIndex;
                table_entry->channelDefaultVideoFormatIndex     = *request->requestvb->val.integer;
                break;
            case COLUMN_CHANNELDEFAULTRECEIVEIPADDRESS:
                table_entry->old_channelDefaultReceiveIpAddress = table_entry->channelDefaultReceiveIpAddress;
                table_entry->channelDefaultReceiveIpAddress     = *request->requestvb->val.integer;
                break;
            }
        }
        break;

    case MODE_SET_UNDO:
        for (request=requests; request; request=request->next) {
            table_entry = (struct channelTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_CHANNELUSERDESC:
                strcpy( table_entry->channelUserDesc,
                        table_entry->old_channelUserDesc); 
                table_entry->channelUserDesc_len =
                        table_entry->old_channelUserDesc_len;
                break;
            case COLUMN_CHANNELSTATUS:
                table_entry->channelStatus    		 			= table_entry->old_channelStatus;
                table_entry->old_channelStatus 					= 0;
                break;
            case COLUMN_CHANNELVIDEOFORMATINDEX:
                table_entry->channelVideoFormatIndex     		= table_entry->old_channelVideoFormatIndex;
                table_entry->old_channelVideoFormatIndex 		= 0;
                break;
            case COLUMN_CHANNELCOMPRESSIONRATE:
                table_entry->channelCompressionRate     		= table_entry->old_channelCompressionRate;
                table_entry->old_channelCompressionRate 		= 0;
                break;
            case COLUMN_CHANNELHORZRES:
                table_entry->channelHorzRes     				= table_entry->old_channelHorzRes;
                table_entry->old_channelHorzRes 				= 0;
                break;
            case COLUMN_CHANNELVERTRES:
                table_entry->channelVertRes     				= table_entry->old_channelVertRes;
                table_entry->old_channelVertRes 				= 0;
                break;
            case COLUMN_CHANNELROIORIGINTOP:
                table_entry->channelRoiOriginTop     			= table_entry->old_channelRoiOriginTop;
                table_entry->old_channelRoiOriginTop 			= 0;
                break;
            case COLUMN_CHANNELROIORIGINLEFT:
                table_entry->channelRoiOriginLeft     			= table_entry->old_channelRoiOriginLeft;
                table_entry->old_channelRoiOriginLeft 			= 0;
                break;
            case COLUMN_CHANNELROIEXTENTBOTTOM:
                table_entry->channelRoiExtentBottom     		= table_entry->old_channelRoiExtentBottom;
                table_entry->old_channelRoiExtentBottom 		= 0;
                break;
            case COLUMN_CHANNELROIEXTENTRIGHT:
                table_entry->channelRoiExtentRight     			= table_entry->old_channelRoiExtentRight;
                table_entry->old_channelRoiExtentRight 			= 0;
                break;
            case COLUMN_CHANNELRECEIVEIPADDRESS:
                table_entry->channelReceiveIpAddress     		= table_entry->old_channelReceiveIpAddress;
                table_entry->old_channelReceiveIpAddress 		= 0;
                break;
            case COLUMN_CHANNELINTERPACKETDELAY:
                table_entry->channelInterPacketDelay     		= table_entry->old_channelInterPacketDelay;
                table_entry->old_channelInterPacketDelay 		= 0;
                break;
            case COLUMN_CHANNELSAPMESSAGEINTERVAL:
                table_entry->channelSapMessageInterval     		= table_entry->old_channelSapMessageInterval;
                table_entry->old_channelSapMessageInterval 		= 0;
                break;
            case COLUMN_CHANNELDEFAULTVIDEOFORMATINDEX:
                table_entry->channelDefaultVideoFormatIndex     = table_entry->old_channelDefaultVideoFormatIndex;
                table_entry->old_channelDefaultVideoFormatIndex = 0;
                break;
            case COLUMN_CHANNELDEFAULTRECEIVEIPADDRESS:
                table_entry->channelDefaultReceiveIpAddress     = table_entry->old_channelDefaultReceiveIpAddress;
                table_entry->old_channelDefaultReceiveIpAddress = 0;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}
