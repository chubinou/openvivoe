/*
 * Licence: GPL
 * Created: Tue, 09 Feb 2016 14:58:20 +0100
 * Main authors:
 *     - hoel <hoel.vasseur@openwide.fr>
 */
#include <signal.h>
#include <glib-2.0/glib.h>
#include <glib-unix.h>
#include <gstreamer-1.0/gst/gst.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
/* In order to initialize the MIB */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "../../include/mibParameters.h"
#include "../../include/conf/mib-conf.h"
#include "../../include/log.h"
#include "../../include/videoFormatInfo/videoFormatTable.h"
#include "../../include/channelControl/channelTable.h"
#include "../../include/streaming/pipeline.h"
#include "../../include/streaming/detect.h"
#include "../../include/streaming/stream_registration.h"
#include "../../include/streaming/stream.h"
/*
 * Macro for testing purposes
 */
#define DEFAULT_FORMAT "raw" /*this is the default video format used for the stream */

/**
 * \brief Handle the Bus: display message, get interruption, etc...
 * \param bus the Bus element
 * \param msg the masseges receive on the bus
 * \return gboolean TRUE (always)
 */
static gboolean bus_call (  GstBus     *bus,
							GstMessage *msg,
							gpointer    data)
{
	GMainLoop *loop = (GMainLoop *) data;
	switch (GST_MESSAGE_TYPE (msg)) {

		case GST_MESSAGE_EOS:
			g_print ("End of stream\n");
			g_main_loop_quit (loop);
			break;

		case GST_MESSAGE_ERROR:{
								   gchar  *debug;
								   GError *error;
								   gst_message_parse_error (msg, &error, &debug);
								   g_free (debug);

								   g_printerr ("Error: %s\n", error->message);
								   g_error_free (error);
								   internal_error = TRUE;
								   g_main_loop_quit (loop);
			break;
		}

		case GST_MESSAGE_WARNING:{
								   gchar  *debug;
								   GError *error;
								   gst_message_parse_warning (msg, &error, &debug);
								   g_free (debug);

								   g_printerr ("Error: %s\n", error->message);
								   g_error_free (error);
								   internal_error = TRUE;
								   g_main_loop_quit (loop);
			break;
		}

		default:
			break;
	}

	return TRUE;
}
#if 0
/**
 * \brief create a fake source for test purposes
 * \param pipeline: the pipeline in which add the source
 * \param format: the video format for the source raw or mp4
 * \param width the width to give to the video source
 * \param height the height to give to the video source
 * \param encoding the encoding to use for the video source
 * \return GstElement* the last element add and link into the pipeline
 */
static GstElement* source_creation(GstElement* pipeline, char* format, int width, int height/*, char* encoding*/){
	/*
	 * For now, the source is created manually, directly into the code
	 * */
	GstElement 	*source, *capsfilter, *enc, *last;
	GstCaps 	*caps;
	/*
	 * For now, the source is created manually, directly into the code
	 * */
	source = gst_element_factory_make ("v4l2src", "source");
//	source = gst_element_factory_make ("videotestsrc", "source");
	if(source == NULL){
		g_printerr ("error cannot create element: %s\n", "videotestsrc" );
		return NULL;
	}
	g_object_set(source, "device", "/dev/video0", NULL);
	capsfilter = gst_element_factory_make ("capsfilter","capsfilter");
	if(capsfilter == NULL){
		g_printerr ( "error cannot create element: %s\n", "capsfilter" );
		return NULL;
	}

	caps = gst_caps_new_full( 	gst_structure_new( 	"video/x-raw" 	,
													"format" 		, G_TYPE_STRING ,"I420" ,
													"width" 		, G_TYPE_INT 	, width,
													"height" 		, G_TYPE_INT 	, height,
													"interlace-mode", G_TYPE_STRING , "progressive",
													"framerate" 	, GST_TYPE_FRACTION, 30, 1,
													NULL),
								NULL);
	g_return_if_fail (gst_caps_is_fixed (caps));

	/* Put the source in the pipeline */
	g_object_set (capsfilter, "caps", caps, NULL);
	gst_bin_add_many (GST_BIN(pipeline), source, capsfilter, NULL);
	gst_element_link (source, capsfilter);
	/* save last pipeline element*/
	last = capsfilter;
	/* For test purposes, if user specify mp4 as format, encrypt the vidoe */
	if( !strcmp(format, "MP4V-ES")){
		/* For test puposes, if we wanna test our program with a fake mpeg-4 source, it necessary to create is mannually with an encoder */
		/*create the MPEG-4 encoder */
		enc = gst_element_factory_make ("avenc_mpeg4", "enc");
		/* save last pipeline element */
		if(enc == NULL){
			g_printerr ( "error cannot create element for: %s\n","enc");
			return NULL;
		}
		/* add encryptor to pipeline, link it to the source */
		gst_bin_add_many (GST_BIN(pipeline), enc, NULL);
		gst_element_link_many (capsfilter, enc,NULL);
		last = enc;
	}
	if( !strcmp(format, "JPEG2000")){
		/* For test puposes, if we wanna test our program with a fake mpeg-4 source, it necessary to create is mannually with an encoder */
		/*create the MPEG-4 encoder */
		enc = gst_element_factory_make ("openjpegenc", "enc");
		/* save last pipeline element */
		if(enc == NULL){
			g_printerr ( "error cannot create element for: %s\n","enc");
			return NULL;
		}
		/* add encryptor to pipeline, link it to the source */
		gst_bin_add_many (GST_BIN(pipeline), enc, NULL);
		gst_element_link_many (capsfilter, enc,NULL);
		last = enc;
	}

	return last;
}
#endif

/**
 * \brief specify if a SP is a redirection (check if it is in the redirection_channel array)
 * \param videoFormatIndex the index of the videoFormat to check 
 * \return the corresponding redirect_data found or NULL 
 */
static redirect_data *SP_is_redirection(long videoFormatIndex){

	int i = 0;
	for ( i = 0;  i< redirection.size; i ++ ){	
		if ( redirection.redirect_channels[i]->video_SP_index == videoFormatIndex ) /* if found, then returns */
			return redirection.redirect_channels[i];
	}

	return NULL; /* if not found, returns NULL */

}

/**
 * \brief specify if a SU is a redirection (check if it is in the redirection_channel array)
 * \param channelIndex the index of the channel to check 
 * \return the corresponding redirect_data found or NULL 
 */
static redirect_data  *SU_is_redirection(long channelIndex, long *videoFormatIndex){

	int i = 0;
	for ( i = 0; i < redirection.size; i ++ ){	
		if (redirection.redirect_channels[i]->channel_SU_index == channelIndex ){ /* if found, then returns */
			*videoFormatIndex = redirection.redirect_channels[i]->video_SP_index;
			return redirection.redirect_channels[i];
		}
	}

	return NULL; /* if not found, returns NULL */

}

/** 
 * \brief create emtpy entries in VFT and CT for the redirection entries for a SP
 * \param steam_data the stream data to associate to the entry 
 * \param videoFormatnIndex the index of the VF to create
 */ 
static void init_redirection( gpointer stream_datas, long videoFormatIndex ){
	
	stream_data *data 			= stream_datas;

	/* create an empty entry for the redirection */
	videoFormatTable_createEntry( 	videoFormatIndex, 	videoChannel,
									disable, 			"",
									"", 				0,
									0,				 	"",
									0, 					0,
									0, 					0,
									0, 					0,
									0, 					0,
									0,					0,
									0, 					0,
									data);
	
	char *channelUserDesc = get_desc_from_conf(videoFormatIndex);

	if ( !channelUserDesc )
		channelUserDesc = "";

	channelTable_create_empty_entry( channelNumber._value.int_val+1 , videoChannel , channelUserDesc , videoFormatIndex , 0 , data );
	/* increase channelNumber as we added an entry */
	channelNumber._value.int_val++;	

}

/**
 * \brief get the command line to use to get the source from configuration file
 * \param pipeline the pipeline to wich adding the bin made from the cmd line description 
 * return GstElement* the last element added in the pipeline: the bin made
 */
static GstElement *get_source( GstElement* pipeline, long videoFormatIndex){
	GError 		*error 		= NULL; /* an Object to save errors when they occurs */
	GstElement 	*bin 		= NULL; /* to return last element of pipeline */
	gchar 		*cmdline 	= init_sources_from_conf( videoFormatIndex );

	/* check if everything went ok */	
	if (cmdline == NULL)
		return NULL;

	/* check if it is a redirection */
	redirect_data *redirection_data = SP_is_redirection( videoFormatIndex ); 

	if( redirection_data ){
		/* if so build the appropriate source */
		bin = gst_element_factory_make_log( "appsrc", "src-redirection");
		if ( !bin )
	   		return NULL;
		
		/* save the pipeline value in the redirection data */
		redirection_data->pipeline_SP = pipeline;

		/* configure for time-based format */
		g_object_set (bin, "format", GST_FORMAT_TIME, NULL);

	}
	else
		bin  = gst_parse_bin_from_description ( cmdline,
												TRUE,
												&error);	

	if ( error != NULL){
		g_printerr("Failed to parse: %s\n",error->message);
	   	return NULL;	
	}

	/* add bin in pipeline */
	gst_bin_add (GST_BIN(pipeline), bin);
	free(cmdline);
	return bin;
}

int handle_SP_default_StartUp_mode(long videoFormatIndex ){
	struct channelTable_entry *entry 	= channelTable_get_from_VF_index(videoFormatIndex);
	if (entry == NULL ){
		g_printerr("ERROR: try to start a source that has no channel associated\n");
		return EXIT_FAILURE;
	}
	entry->channelStatus 				= start;

	if ( channelSatus_requests_handler( entry ))
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

/**
 * \brief initiate a pipeline for a SP
 * \param main_loop the main loop
 * \param videoFormatIndex the videoFormatIndex for which the pipeline will be built
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int init_stream_SP( gpointer main_loop, int videoFormatIndex){

	/* Initialization of elements needed */
	GMainLoop 	*loop 			= main_loop;
    GstElement 	*pipeline;
    GstElement 	*last;	
    GstBus 		*bus;
    guint 		bus_watch_id;
	
	/* Create the pipeline */
	pipeline  = gst_pipeline_new ("pipeline");
	if(pipeline  == NULL){
		g_printerr ( "error cannot create: %s\n","pipeline" );
		return EXIT_FAILURE;
	}

	/* Reference all data relevant to the stream */
	stream_data 	*data 		= malloc(sizeof(stream_data));

	/* allocate memory for the structure rtp_data of stream_data */
	/* this will be free in the delete_stream function */
	rtp_data 	*rtp_datas 		= malloc(sizeof(rtp_data));
	data->rtp_datas 			= rtp_datas;
	
	data->pipeline 		= pipeline;	
	/* we add a message handler */
	bus = gst_pipeline_get_bus ( GST_PIPELINE(pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

	data->bus 			= bus;
	data->bus_watch_id 	= bus_watch_id;

	last = get_source(pipeline, videoFormatIndex);

	if (last == NULL ){
		g_printerr ( "Failed to create videosource\n");
		return EXIT_FAILURE;
	}
	
	gboolean redirection = FALSE; 
	/* if this is a redirection */
	if ( !strcmp(GST_ELEMENT_NAME(last), "src-redirection") ){ 
		init_redirection( data, videoFormatIndex );
		/* for convenience we store the last element added in pipeline here, even if it is not it purpose , this will be used to retrieve the  last element of the pipeline later */
		data->sink = last ; 
		redirection = TRUE;
	}
	else
		last = create_pipeline_videoChannel( data ,	main_loop, last, videoFormatIndex ); /* Create pipeline  - save videoFormatIndex into stream_data data*/

	/* Check if everything went ok*/
	if (last == NULL){
		g_printerr ( "Failed to create pipeline\n");
		return EXIT_FAILURE;
	}

	if ( ! redirection ){	
		/* set pipeline to PAUSED state will open /dev/video0, so it will not be done in start_streaming */
		gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
		/* if we are in the defaultStartUp Mode, launch the last VF register in the table */
		if ( deviceInfo.parameters[num_DeviceMode]._value.int_val == defaultStartUp){
			return handle_SP_default_StartUp_mode(videoFormatIndex);
		}
	}

	return EXIT_SUCCESS;

}

/**
 * \brief initiate a pipeline to display received stream
 * \param main_loop the main loop
 * \param stream_datas a structure in which we will save the pipeline and the bus elements
 * \param caps the GstCaps made from the SDP file received with the stream
 * \param channel_entry the serviceUser corresponding channel 
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int init_stream_SU( gpointer main_loop,GstCaps *caps, struct channelTable_entry *channel_entry)
{
    
  /* Initialization of elements needed */
	GMainLoop 	*loop 			= main_loop;
    GstElement 	*pipeline;
    GstBus 		*bus;
    guint 		bus_watch_id;
	GstElement *last;

	/* Create the pipeline */
	pipeline  = gst_pipeline_new ("pipeline");
	if(pipeline  == NULL){
		g_printerr ( "error cannot create: %s\n","pipeline" );
		return EXIT_FAILURE;
	}

	/* Reference all data relevant to the stream */
	stream_data 	*data 		= malloc(sizeof(stream_data));

	/* allocate memory for the structure rtp_data of stream_data */
	/* this will be free in the delete_stream function */
	rtp_data 	*rtp_datas 		= malloc(sizeof(rtp_data));
	data->rtp_datas 			= rtp_datas;
	
	data->pipeline 		= pipeline;	
	/* we add a message handler */
	bus = gst_pipeline_get_bus ( GST_PIPELINE(pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

	data->bus 			= bus;
	data->bus_watch_id 	= bus_watch_id;

	gchar 		*cmdline 	= init_sink_from_conf( channel_entry->channelIndex );

	/* check if everything went ok */	
	if (cmdline == NULL )
		return EXIT_FAILURE;

	/* declare a variable where we will store the corresponding potential entry of redirect_channels */
	redirect_data *redirection_data;	
	
	long videoFormatIndex = -1 ;
	/* 
	 * check if this is a redirection, if so the mapping of the videoFormatIndex of the source to which redirect the stream will be 
	 * stored in videoFormatIndex
	 */

	 redirection_data = SU_is_redirection( channel_entry->channelIndex, &videoFormatIndex ); 

	/* check is the mapping have been done succesfully */
	if ( redirection_data  && videoFormatIndex == -1 ){
			g_printerr("ERROR: no source was found to redirect the stream of service Users's channel: %ld\n", channel_entry->channelIndex);
			return EXIT_FAILURE;
	}

	/* Create pipeline  - save videoFormatIndex into stream_data data*/
	last = create_pipeline_serviceUser( data, main_loop, caps, channel_entry, cmdline, redirection_data );
	
	/* Check if everything went ok*/
	if (last == NULL){
		g_printerr ( "Failed to create pipeline\n");
		return EXIT_FAILURE;
	}
	
	gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
	channel_entry->stream_datas = data;

	/* 
	 * If this is a redirection, now that we know the video caps of the stream,
	 * we need to add the rtp depayloader to the pipeline of the source that will 
	 * redirect the steam: this is done in append_SP_pipeline_for_redirection() 
	 * of pipeline.c.
	 * Also, we need to call handle_SP_default_StartUp_mode() to handle the default start up 
	 * on this source after is pipeline has been completed.
	 */
	if ( redirection_data ){
		if ( !append_SP_pipeline_for_redirection( caps,  videoFormatIndex) )
			return EXIT_FAILURE;
		handle_SP_default_StartUp_mode( videoFormatIndex ) ;
	}
	
   	return EXIT_SUCCESS;

}

/**
 * \brief start the streaming: set pipeling into PLAYING state
 * \param data of the stream (pipeline, bus and bust_watch_id) - see stream_data structure
 * \return 0
 */
gboolean start_streaming (gpointer stream_datas, long channelVideoFormatIndex ){
	stream_data *data 								= stream_datas;
	struct videoFormatTable_entry * stream_entry 	= videoFormatTable_getEntry(channelVideoFormatIndex);
	if ( data->pipeline != NULL){
		/* Set the pipeline to "playing" state*/
		g_print ("Now playing channel: %ld\n",channelVideoFormatIndex );
		gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
		if ( stream_entry != NULL)
			stream_entry->videoFormatStatus = enable;
		return TRUE;
	}
	return FALSE;
}

/**
 * \brief stop the streaming: set pipeling into NULL state
 * \param data of the stream (pipeline, bus and bust_watch_id) - see stream_data structure
 * \return 0
 */
int stop_streaming( gpointer stream_datas, long channelVideoFormatIndex ){
	stream_data *data 								= stream_datas;
	struct videoFormatTable_entry * stream_entry 	= videoFormatTable_getEntry(channelVideoFormatIndex);
	/* Out of the main loop, clean up nicely */
	g_print ("Returned, stopping playback\n");
	gst_element_set_state (data->pipeline, GST_STATE_NULL);
	if ( stream_entry != NULL)	
		stream_entry->videoFormatStatus = disable;
	return 0;
}

/**
 * \brief delete the stream: free pipeline and bus element
 * \param data of the stream (pipeline, bus and bust_watch_id) - see stream_data structure
 * \return 0
 */
int delete_steaming_data(gpointer channel_entry){
	struct channelTable_entry 	*entry 	= channel_entry;
	stream_data 				*data 	= entry->stream_datas;
	/* delete pipeline */
	if (data != NULL ){
		g_print ("Deleting pipeline\n");
		gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
		gst_element_set_state (data->pipeline, GST_STATE_READY);
		gst_element_set_state (data->pipeline, GST_STATE_NULL);
		gst_object_unref (GST_OBJECT (data->pipeline));
		g_source_remove (data->bus_watch_id);
		/* free rtp_data */
		free(data->rtp_datas);
		/* free the sap_data */
		free(data);
		entry->stream_datas = NULL;
	}

	/* check if their is socket open in /tmp/openvivoe (in case their is a redirection) */


	return 0;
}
