/*
 * Note: this file originally auto-generated by mib2c using
 *  $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <glib-2.0/glib.h>
#include <glib-unix.h>
#include <gstreamer-1.0/gst/gst.h>
#include "../../include/mibParameters.h"
#include "../../include/videoFormatInfo/videoFormatTable.h"
#include "../../include/handler.h"
#include "../../include/channelControl/channelTable.h"
#include "../../include/streaming/stream_registration.h"
#include "../../include/streaming/pipeline.h"
#include "../../include/streaming/stream.h"

/** Initializes the videoFormatTable module */
void
init_videoFormatTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_videoFormatTable();
}


/**
 * Create a new row in the (unsorted) table.
 * This create an entry and add it into the table
 * Howeber we need a function which is juste used to intitiate the entry's members
 * without adding in into the table: intiation a member but do not set the "next"
 * parameter.
 * */
struct videoFormatTable_entry * videoFormatTable_createEntry( 	long  videoFormatIndex, 			long videoFormatType,
																long videoFormatStatus,				char* videoFormatBase,
																char* videoFormatSampling, 			long videoFormatBitDepth,
																long videoFormatFps,				char* videoFormatColorimetry,
															 	long videoFormatInterlaced,			long videoFormatCompressionFactor, 	
																long videoFormatCompressionRate, 	long videoFormatMaxHorzRes,			
																long videoFormatMaxVertRes, 		long videoFormatRoiHorzRes,			
																long videoFormatRoiVertRes, 		long videoFormatRoiOriginTop,	
																long videoFormatRoiOriginLeft,		long videoFormatRoiExtentBottom,
																long videoFormatRoiExtentRight, 	long videoFormatRtpPt, 
																gpointer stream_datas) {

    struct videoFormatTable_entry *entry;
    entry = SNMP_MALLOC_TYPEDEF(struct videoFormatTable_entry);
    if (!entry)
        return NULL;

    entry->videoFormatIndex 			= videoFormatIndex;
	entry->videoFormatType 				= videoFormatType;
	entry->videoFormatStatus 			= videoFormatStatus;
	entry->videoFormatBase 				= strdup(videoFormatBase);
	entry->videoFormatBase_len 			= MIN(strlen(videoFormatBase), DisplayString16);
	entry->videoFormatSampling 			= strdup(videoFormatSampling);
	entry->videoFormatSampling_len 		= MIN(strlen(videoFormatSampling), DisplayString16);
	entry->videoFormatBitDepth 			= videoFormatBitDepth;
	entry->videoFormatFps 				= videoFormatFps;
	entry->videoFormatColorimetry 		= strdup(videoFormatColorimetry);
	entry->videoFormatColorimetry_len 	= MIN(strlen(videoFormatColorimetry), DisplayString16);
	entry->videoFormatInterlaced 		= videoFormatInterlaced;
	entry->videoFormatCompressionFactor = videoFormatCompressionFactor;
	entry->videoFormatCompressionRate 	= videoFormatCompressionRate;
	entry->videoFormatMaxHorzRes 		= videoFormatMaxHorzRes;
 	entry->videoFormatMaxVertRes 		= videoFormatMaxVertRes;
	entry->videoFormatRoiHorzRes 		= videoFormatRoiHorzRes;
 	entry->videoFormatRoiVertRes 		= videoFormatRoiVertRes;
	entry->videoFormatRoiOriginTop 		= videoFormatRoiOriginTop;
 	entry->videoFormatRoiOriginLeft 	= videoFormatRoiOriginLeft;
 	entry->videoFormatRoiExtentBottom 	= videoFormatRoiExtentBottom;
	entry->videoFormatRoiExtentRight 	= videoFormatRoiExtentRight;
	entry->videoFormatRtpPt 			= videoFormatRtpPt;

	/* add streaming information to a videoFormat */
	entry->stream_datas                                     = stream_datas;


	/* Link it into the list of entries */ 
    entry->next = videoFormatTable_head;
    videoFormatTable_head = entry;
	entry->valid = 1;
    return entry;
}

/** 
 * \brief retrieve the entry in the viodeFormatTable corresponding to the index given in parameter
 * \param index the index of the entry we want
 * \return videoFormatTable_entry*  a pointer on that entry
 */
struct videoFormatTable_entry * videoFormatTable_getEntry(int index){
	struct videoFormatTable_entry *iterator = videoFormatTable_head;
	
	/* check if the VF table is empty */
	if ( iterator == NULL )
		return NULL;

	while(iterator->videoFormatIndex != index){
		if(iterator->next != NULL)
			iterator = iterator->next;
		else
			return NULL;
	}
	return iterator;
}

/** 
 * \brief delete all entries in the table
 */
void videoFormatTable_delete(){
	struct videoFormatTable_entry *iterator = videoFormatTable_head;
	struct videoFormatTable_entry *temp;
	while(iterator != NULL){
		temp = iterator;
		iterator = iterator->next;
		free(temp);
	}
}
/* Initialize the videoFormatTable table by defining its contents and how it's structured */
void
initialize_table_videoFormatTable(void)
{
    const oid videoFormatTable_oid[] 		= {1,3,6,1,4,1,35990,3,1,2,2};
    const size_t videoFormatTable_oid_len   = OID_LENGTH(videoFormatTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;

    DEBUGMSGTL(("videoFormatTable:init", "initializing table videoFormatTable\n"));

    reg = netsnmp_create_handler_registration(
              "videoFormatTable",     videoFormatTable_handler,
              videoFormatTable_oid, videoFormatTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           			 ASN_INTEGER,  /* index: videoFormatIndex */
                           			 0);
    table_info->min_column = COLUMN_VIDEOFORMATTYPE;
    table_info->max_column = COLUMN_VIDEOFORMATRTPPT;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = videoFormatTable_get_first_data_point;
    iinfo->get_next_data_point  = videoFormatTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

	/* get all the source configuration for configuration file, init the corresponding streams */
	if( deviceInfo.parameters[num_DeviceType]._value.int_val != device_SP ){
		for(int i=1; i<=videoFormatNumber._value.int_val; i++)
			init_stream_SP(loop, i);
	}

}

#if ALLOW_REMOVING_ROW /* No right to remove a line from the table */
/* remove a row from the table */
void
videoFormatTable_removeEntry( struct videoFormatTable_entry *entry ) {
    struct videoFormatTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = videoFormatTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        videoFormatTable_head = ptr->next;
    else
        prev->next = ptr->next;

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}
#endif

void
videoFormatTable_free( netsnmp_cache *cache, void *vmagic ) {
    struct videoFormatTable_entry *this, *that;

    for ( this = videoFormatTable_head; this; this=that ) {
        that = this->next;
        SNMP_FREE( this );   /* XXX - release any other internal resources */
    }
    videoFormatTable_head = NULL;
}

/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
videoFormatTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = videoFormatTable_head;
    return videoFormatTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
videoFormatTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct videoFormatTable_entry *entry = (struct videoFormatTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->videoFormatIndex );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}


/** handles requests for the videoFormatTable table */
int
videoFormatTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       			*request;
    netsnmp_table_request_info 			*table_info;
    struct videoFormatTable_entry       *table_entry;
	int ret = 0 ;
    DEBUGMSGTL(("videoFormatTable:handler", "Processing request (%d)\n", reqinfo->mode));

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct videoFormatTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_VIDEOFORMATTYPE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatType);
                break;
            case COLUMN_VIDEOFORMATSTATUS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatStatus);
                break;
            case COLUMN_VIDEOFORMATBASE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->videoFormatBase,
                                          table_entry->videoFormatBase_len);
                break;
            case COLUMN_VIDEOFORMATSAMPLING:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->videoFormatSampling,
                                          table_entry->videoFormatSampling_len);
                break;
            case COLUMN_VIDEOFORMATBITDEPTH:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatBitDepth);
                break;
            case COLUMN_VIDEOFORMATFPS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatFps);
                break;
            case COLUMN_VIDEOFORMATCOLORIMETRY:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->videoFormatColorimetry,
                                          table_entry->videoFormatColorimetry_len);
                break;
            case COLUMN_VIDEOFORMATINTERLACED:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatInterlaced);
                break;
            case COLUMN_VIDEOFORMATCOMPRESSIONFACTOR:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatCompressionFactor);
                break;
            case COLUMN_VIDEOFORMATCOMPRESSIONRATE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatCompressionRate);
                break;
            case COLUMN_VIDEOFORMATMAXHORZRES:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatMaxHorzRes);
                break;
            case COLUMN_VIDEOFORMATMAXVERTRES:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatMaxVertRes);
                break;
            case COLUMN_VIDEOFORMATROIHORZRES:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatRoiHorzRes);
                break;
            case COLUMN_VIDEOFORMATROIVERTRES:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatRoiVertRes);
                break;
            case COLUMN_VIDEOFORMATROIORIGINTOP:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatRoiOriginTop);
                break;
            case COLUMN_VIDEOFORMATROIORIGINLEFT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatRoiOriginLeft);
                break;
            case COLUMN_VIDEOFORMATROIEXTENTBOTTOM:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatRoiExtentBottom);
                break;
            case COLUMN_VIDEOFORMATROIEXTENTRIGHT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatRoiExtentRight);
                break;
            case COLUMN_VIDEOFORMATRTPPT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->videoFormatRtpPt);
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
            table_entry = (struct videoFormatTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

    		/* check if the index we are trying to modify is in the table, if no, return */
			if ( index_out_of_range( reginfo,
                     			   	reqinfo,
								    requests,
								 	table_info,
									videoFormatNumber._value.int_val ) )
				return SNMP_ERR_NOERROR;

            switch (table_info->colnum) {
            case COLUMN_VIDEOFORMATCOMPRESSIONFACTOR:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_VIDEOFORMATCOMPRESSIONRATE:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_VIDEOFORMATROIHORZRES:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_VIDEOFORMATROIVERTRES:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_VIDEOFORMATROIORIGINTOP:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_VIDEOFORMATROIORIGINLEFT:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_VIDEOFORMATROIEXTENTBOTTOM:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_VIDEOFORMATROIEXTENTRIGHT:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_VIDEOFORMATRTPPT:
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

    case MODE_SET_RESERVE2:
        for (request=requests; request; request=request->next) {
            table_entry = (struct videoFormatTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_VIDEOFORMATCOMPRESSIONRATE:
				if ( deviceInfo.parameters[num_DeviceMode]._value.int_val	!= maintenanceMode){
					netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE  );
					return SNMP_ERR_NOERROR;
				}
                break;
            case COLUMN_VIDEOFORMATROIHORZRES:
			  	if ( deviceInfo.parameters[num_DeviceMode]._value.int_val	!= maintenanceMode){
					netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE  );
					return SNMP_ERR_NOERROR;
				}
                break;
            case COLUMN_VIDEOFORMATROIVERTRES:
				if ( deviceInfo.parameters[num_DeviceMode]._value.int_val	!= maintenanceMode){
					netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE  );
					return SNMP_ERR_NOERROR;
				}
                break;
            case COLUMN_VIDEOFORMATROIORIGINTOP:
				if ( deviceInfo.parameters[num_DeviceMode]._value.int_val	!= maintenanceMode){
					netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE  );
					return SNMP_ERR_NOERROR;
				}
                break;
            case COLUMN_VIDEOFORMATROIORIGINLEFT:
				if ( deviceInfo.parameters[num_DeviceMode]._value.int_val	!= maintenanceMode){
					netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE  );
					return SNMP_ERR_NOERROR;
				}
                break;
            case COLUMN_VIDEOFORMATROIEXTENTBOTTOM:
				if ( deviceInfo.parameters[num_DeviceMode]._value.int_val	!= maintenanceMode){
					netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE  );
					return SNMP_ERR_NOERROR;
				}
                break;
            case COLUMN_VIDEOFORMATROIEXTENTRIGHT:
				if ( deviceInfo.parameters[num_DeviceMode]._value.int_val	!= maintenanceMode){
					netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE  );
					return SNMP_ERR_NOERROR;
				}
                break;
            case COLUMN_VIDEOFORMATRTPPT:
				if ( deviceInfo.parameters[num_DeviceMode]._value.int_val	!= maintenanceMode){
					netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE  );
					return SNMP_ERR_NOERROR;
				}
                break;
			default:
				break; /* it might be a writable object not in maintenance group */
			}
        }
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        for (request=requests; request; request=request->next) {
            table_entry = (struct videoFormatTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_VIDEOFORMATCOMPRESSIONFACTOR:
                table_entry->old_videoFormatCompressionFactor = table_entry->videoFormatCompressionFactor;
                table_entry->videoFormatCompressionFactor     = *request->requestvb->val.integer;
                break;
            case COLUMN_VIDEOFORMATCOMPRESSIONRATE:
                table_entry->old_videoFormatCompressionRate = table_entry->videoFormatCompressionRate;
                table_entry->videoFormatCompressionRate     = *request->requestvb->val.integer;
                break;
            case COLUMN_VIDEOFORMATROIHORZRES:
                table_entry->old_videoFormatRoiHorzRes = table_entry->videoFormatRoiHorzRes;
                table_entry->videoFormatRoiHorzRes     = *request->requestvb->val.integer;
                break;
            case COLUMN_VIDEOFORMATROIVERTRES:
                table_entry->old_videoFormatRoiVertRes = table_entry->videoFormatRoiVertRes;
                table_entry->videoFormatRoiVertRes     = *request->requestvb->val.integer;
                break;
            case COLUMN_VIDEOFORMATROIORIGINTOP:
                table_entry->old_videoFormatRoiOriginTop = table_entry->videoFormatRoiOriginTop;
                table_entry->videoFormatRoiOriginTop     = *request->requestvb->val.integer;
                break;
            case COLUMN_VIDEOFORMATROIORIGINLEFT:
                table_entry->old_videoFormatRoiOriginLeft = table_entry->videoFormatRoiOriginLeft;
                table_entry->videoFormatRoiOriginLeft     = *request->requestvb->val.integer;
                break;
            case COLUMN_VIDEOFORMATROIEXTENTBOTTOM:
                table_entry->old_videoFormatRoiExtentBottom = table_entry->videoFormatRoiExtentBottom;
                table_entry->videoFormatRoiExtentBottom     = *request->requestvb->val.integer;
                break;
            case COLUMN_VIDEOFORMATROIEXTENTRIGHT:
                table_entry->old_videoFormatRoiExtentRight = table_entry->videoFormatRoiExtentRight;
                table_entry->videoFormatRoiExtentRight     = *request->requestvb->val.integer;
                break;
            case COLUMN_VIDEOFORMATRTPPT:
                table_entry->old_videoFormatRtpPt = table_entry->videoFormatRtpPt;
                table_entry->videoFormatRtpPt     = *request->requestvb->val.integer;
                break;
            }
        }
        break;

    case MODE_SET_UNDO:
        for (request=requests; request; request=request->next) {
            table_entry = (struct videoFormatTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_VIDEOFORMATCOMPRESSIONFACTOR:
                table_entry->videoFormatCompressionFactor    	 = table_entry->old_videoFormatCompressionFactor;
                table_entry->old_videoFormatCompressionFactor 	= 0;
                break;
            case COLUMN_VIDEOFORMATCOMPRESSIONRATE:
                table_entry->videoFormatCompressionRate     	= table_entry->old_videoFormatCompressionRate;
                table_entry->old_videoFormatCompressionRate 	= 0;
                break;
            case COLUMN_VIDEOFORMATROIHORZRES:
                table_entry->videoFormatRoiHorzRes     			= table_entry->old_videoFormatRoiHorzRes;
                table_entry->old_videoFormatRoiHorzRes 			= 0;
                break;
            case COLUMN_VIDEOFORMATROIVERTRES:
                table_entry->videoFormatRoiVertRes    			= table_entry->old_videoFormatRoiVertRes;
                table_entry->old_videoFormatRoiVertRes 			= 0;
                break;
            case COLUMN_VIDEOFORMATROIORIGINTOP:
                table_entry->videoFormatRoiOriginTop     		= table_entry->old_videoFormatRoiOriginTop;
                table_entry->old_videoFormatRoiOriginTop 		= 0;
                break;
            case COLUMN_VIDEOFORMATROIORIGINLEFT:
                table_entry->videoFormatRoiOriginLeft     		= table_entry->old_videoFormatRoiOriginLeft;
                table_entry->old_videoFormatRoiOriginLeft 		= 0;
                break;
            case COLUMN_VIDEOFORMATROIEXTENTBOTTOM:
                table_entry->videoFormatRoiExtentBottom     	= table_entry->old_videoFormatRoiExtentBottom;
                table_entry->old_videoFormatRoiExtentBottom 	= 0;
                break;
            case COLUMN_VIDEOFORMATROIEXTENTRIGHT:
                table_entry->videoFormatRoiExtentRight     		= table_entry->old_videoFormatRoiExtentRight;
                table_entry->old_videoFormatRoiExtentRight 		= 0;
                break;
            case COLUMN_VIDEOFORMATRTPPT:
                table_entry->videoFormatRtpPt     				= table_entry->old_videoFormatRtpPt;
                table_entry->old_videoFormatRtpPt 				= 0;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}
