/*
 * Licence: GPL
 * Created: Mon, 08 Feb 2016 09:32:44 +0100
 * Main authors:
 *     - hoel <hoel.vasseur@openwide.fr>
 */


#include <glib-2.0/glib.h>
#include <gstreamer-1.0/gst/gst.h>
#include <gst/video/video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conf/stream-conf.h"
#include "streaming/detect.h"
#include "streaming/filter.h"
#include "log.h"


#define KEY_NO_FILTER_VALUE 	"no-filter"

/*
 * \brief tells if key "encoding" is set to no-filter
 * \param ecoding the list of key value from configuration file
 * \return TRUE if key "encoding" is set to no-filter or FALSE
 */
static gboolean no_filter_encoding(const gchar * const * encoding){
	return (g_strv_contains(encoding, KEY_NO_FILTER_VALUE ));
}

/*
 * \brief tells if key "resolution" is set to no-filter
 * \param resolution the list of key value from configuration file
 * \return TRUE if key "encoding" is set to no-filter or FALSE
 */
static gboolean no_filter_resolution(const gchar * const * resolution){
	return (g_strv_contains(resolution, KEY_NO_FILTER_VALUE ));
}

/*
 * return TRUE if all encoding are used
 */
static gboolean all_encodings(const gchar * const * encoding){
	return (g_strv_contains(encoding, "all") ||
			( g_strv_contains(encoding, "RGB") &&
			  g_strv_contains(encoding, "YUV") &&
			  g_strv_contains(encoding, "Monochrome")));
}

/*
 * return TRUE if all resolutions are used
 */
static gboolean all_resolutions(const gchar * const * resolution){
	return (g_strv_contains(resolution, "all") ||
				( 	g_strv_contains(resolution, "576i") 	&&
			  		g_strv_contains(resolution, "720p") 	&&
			  		g_strv_contains(resolution, "1080i") 	&&
			  		g_strv_contains(resolution, "1080p") ));
}


/*
 * This function build the default GValue for widths
 * iparameters taken are the indexes of desired min and max width
 */
/* local define for the array of widths */
static GValue build_list(int* int_array, gchar** char_array, int length_array ){
	GValue resolution = G_VALUE_INIT;
	g_value_init (&resolution, GST_TYPE_LIST);
	GValue item = G_VALUE_INIT;
	if( int_array != NULL){
		g_value_init (&item, G_TYPE_INT);
		/* Build list of values */
		for(int i=0; i < length_array; i++){
			g_value_set_int (&item, int_array[i]);
			gst_value_list_append_value (&resolution, &item);
		}
	}
	else if ( char_array != NULL){
		g_value_init (&item, G_TYPE_STRING);
		/* Build list of values */
		for(int i=0; i < length_array; i++){
			g_value_set_static_string	(&item, char_array[i]);
			gst_value_list_append_value (&resolution, &item);
		}
	}
	else{
		g_critical("problem while building list");
	}
	return resolution;
}


static void set_filter_field( 	GstStructure* gst,
								const gchar* field,
								int* int_array,
								gchar** char_array,
								int length_array
							   	){
	GValue set = G_VALUE_INIT;
	/* if Value is NULL, we are initializing a list of values */
	g_value_init (&set,GST_TYPE_LIST);
	set = build_list(int_array, char_array,length_array);

	gst_structure_set_value(gst,
                        	field,
							&set);
}

/*
 * Copy the content of array 2 into array 1 from offset "offset"
 */
static void copy(char* array1[], int length1, int offset, char* array2[], int length2){
	int i = 0;
	if(array1 != NULL && array2 != NULL){
		if( length1 >= (length2 + offset)){
			for(i=0; i<length2; i++)
				array1[i + offset] = array2[i];
		}
	}
}

/*
 * Build the RAW filter caps
 */
/* define lenght for the Arrays of encodings */
#define length_RGB 			4
#define length_YUV 			5
#define length_Mono			3
/* define lenght for the Arrays of resolutions */
#define length_width_576 	4
#define length_height_576 	2
#define length_width 		6
#define length_height 		4
/* define number of interlace-mode used */
#define num_inter_mode 		2

/**
 * \brief Build the RAW filter caps
 * \param gkf a pointer to the opened configuration file
 * \return GstStructure* a caps structure built from the incormation found in configuration file under RAW group name
 */
static GstStructure* build_RAW_filter(GKeyFile* gkf){
	const gchar * const * 		encoding 	= (const gchar * const *)	get_raw_encoding(gkf);
	const gchar * const * 		resolution 	= (const gchar * const *)	get_raw_res(gkf);
	GstStructure* 				raw_filter 	= gst_structure_new_from_string ("video/x-raw");

	/* Define the array used to build the filter */
	char* RGB[length_RGB] 					= { "RGB", 		"RGBA", 	 "BGR", 	"BGRA" };
	char* YUV[length_YUV] 					= { "AYUV", 	"UYVY", 	 "I420", 	"Y41B", 	"UYVP" };
	char* Monochrome[length_Mono] 			= { "GRAY8", 	"GRAY16_BE", "GRAY16_LE" };
	int length 								= length_RGB + length_YUV + length_Mono;
	int offset 								= 0;
	char* formats[length];

	if ( !no_filter_encoding(encoding)){

		/* Add encoding part */
		if( all_encodings(encoding) ){
			/* Build array of full formats */
			copy(formats, length, 0, 						RGB, 		length_RGB);
			copy(formats, length, length_RGB, 				YUV, 		length_YUV );
			copy(formats, length, length_RGB+length_YUV, 	Monochrome, length_Mono );
			set_filter_field(raw_filter, "format", NULL, formats , length);
		}else{
			length = 0 ;
			if( g_strv_contains(encoding, "RGB") ){
				length += 	length_RGB;
				copy(formats, length, offset, RGB, length_RGB);
				offset += 	length_RGB;
			}
			if( g_strv_contains(encoding, "YUV") ){
				length += length_YUV;
				copy(formats, length, offset, YUV, length_YUV );
				offset += length_YUV;
			}
			if( g_strv_contains(encoding, "Monochrome") ){
				length += length_Mono;
				copy(formats, length, offset, Monochrome, length_Mono );
				offset += length_Mono;
			}
			set_filter_field(raw_filter, "format",NULL, formats, length);
		}

	}

	int width_small[length_width_576] 	= {544,576,720,768};
	int heigth_small[length_height_576] = {576,704};
	int* width 							= (int*) malloc(length_width * sizeof(int));
	int* height 						= (int*) malloc(length_height * sizeof(int));
	char* interlace_mode[num_inter_mode];
	char* interlaced[1]					={"interlaced"};
	char* progressive[1] 				={"progressive"};
	int offset_width					= 0;
	int offset_height					= 0;
	int offset_im 						= 0;

	if ( !no_filter_resolution(resolution)){

		/* Add resolution part */
		if( all_resolutions( resolution ) )
		{
			/* witdht */
			memcpy(width, width_small, length_width_576 * sizeof(int));
			offset_width += length_width_576;
			*(width + offset_width) = 1280;
			offset_width ++;
			*(width + offset_width) = 1920;
			offset_width ++;
			set_filter_field(raw_filter, "width", 	width, 	NULL, length_width);
			/* height */
			memcpy(height, heigth_small, length_height_576 * sizeof(int));
			offset_height += length_height_576;
			*(height + offset_height) = 720;
			offset_height ++;
			*(height + offset_height) = 1080;
			offset_height ++;
			set_filter_field(raw_filter, "height", 	height, NULL, length_height);
			/* interlaced-mode */
			copy(interlace_mode, num_inter_mode, 0, interlaced, 1 );
			offset_im++;
			copy(interlace_mode, num_inter_mode, offset_im, progressive, 1 );
			set_filter_field(raw_filter,"interlace-mode",NULL, interlace_mode, num_inter_mode);
		}else{
			if( g_strv_contains(resolution,"576i") ){
				memcpy(width, width_small, length_width_576 * sizeof(int));
				offset_width += length_width_576;
				memcpy(height, heigth_small, length_height_576 * sizeof(int));
				offset_height += length_height_576;
				copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
				offset_im++;
			}
			if( g_strv_contains(resolution, "720p") ){
				*(width + offset_width) = 1280;
				offset_width ++;
				*(height + offset_height) = 720;
				offset_height ++;
				copy(interlace_mode, num_inter_mode,offset_im , progressive, 1 );
				offset_im++;
			}
			if( g_strv_contains(resolution, "1080p" ) ){
				*(width + offset_width) = 1920;
				offset_width ++;
				*(height + offset_height) = 1080;
				offset_height ++;
				copy(interlace_mode, num_inter_mode,offset_im , progressive, 1 );
				offset_im++;
			}
			else if( g_strv_contains(resolution, "1080i") ){
				if( g_strv_contains(resolution, "1080p" ) ){
					copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
					offset_im++;
				}else{
					*(width + offset_width) = 1920;
					offset_width ++;
					*(height + offset_height) = 1080;
					offset_height ++;
					copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
					offset_im++;
				}
			}
			set_filter_field(raw_filter, "width", 			width, 	NULL, offset_width);
			set_filter_field(raw_filter, "height", 			height, NULL, offset_height);
			set_filter_field(raw_filter,"interlace-mode",NULL, interlace_mode, MIN(num_inter_mode,offset_im));
		}
	}

//	set_filter_field(raw_filter, "framerate", GST_VIDEO_FPS_RANGE);
//	printf("%s", gst_structure_to_string (raw_filter) );
	free(width);
	free(height);
	return raw_filter;
}

/**
 * \brief Build the MPEG-4 filter caps
 * \param gkf a pointer to the opened configuration file
 * \return GstStructure* a caps structure built from the incformation found in configuration file under MPEG-4 group name
 */
static GstStructure* build_MPEG4_filter(GKeyFile* gkf){
	const gchar * const * 		resolution 	= (const gchar * const *)	get_mp4_res(gkf);
	GstStructure* 				mpeg_filter = gst_structure_new_from_string ("video/mpeg, mpegversion=(int)4");
	int width_small[length_width_576] 	= {544, 576, 720, 768};
	int heigth_small[length_height_576] = {576, 704};
	int* width 							= (int*) malloc(length_width * sizeof(int));
	int* height 						= (int*) malloc(length_height * sizeof(int));
	char* interlace_mode[num_inter_mode];
	char* interlaced[1]					={"interlaced"};
	char* progressive[1] 				={"progressive"};
	int offset_width					= 0;
	int offset_height					= 0;
	int offset_im 						= 0;

	if ( !no_filter_resolution(resolution)){

		/* Add resolution part */
		if( all_resolutions( resolution ) )
		{
			/* witdth */
			memcpy(width, width_small, length_width_576 * sizeof(int));
			offset_width += length_width_576;
			*(width + offset_width) = 1280;
			offset_width ++;
			*(width + offset_width) = 1920;
			offset_width ++;
			set_filter_field(mpeg_filter, "width", 	width, 	NULL, length_width);
			/* height */
			memcpy(height, heigth_small, length_height_576 * sizeof(int));
			offset_height += length_height_576;
			*(height + offset_height) = 720;
			offset_height ++;
			*(height + offset_height) = 1080;
			offset_height ++;
			set_filter_field(mpeg_filter, "height", 	height, NULL, length_height);
			/* interlaced-mode */
			copy(interlace_mode, num_inter_mode, 0, interlaced, 1 );
			offset_im++;
			copy(interlace_mode, num_inter_mode, offset_im, progressive, 1 );
			set_filter_field(mpeg_filter,"interlace-mode",NULL, interlace_mode, num_inter_mode);
		}else{
			if( g_strv_contains(resolution,"576i") ){
				memcpy(width, width_small, length_width_576 * sizeof(int));
				offset_width += length_width_576;
				memcpy(height, heigth_small, length_height_576 * sizeof(int));
				offset_height += length_height_576;
				copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
				offset_im++;
			}
			if( g_strv_contains(resolution, "720p") ){
				*(width + offset_width) = 1280;
				offset_width ++;
				*(height + offset_height) = 720;
				offset_height ++;
				copy(interlace_mode, num_inter_mode,offset_im , progressive, 1 );
				offset_im++;
			}
			if( g_strv_contains(resolution, "1080p" ) ){
				*(width + offset_width) = 1920;
				offset_width ++;
				*(height + offset_height) = 1080;
				offset_height ++;
				copy(interlace_mode, num_inter_mode,offset_im , progressive, 1 );
				offset_im++;
			}
			else if( g_strv_contains(resolution, "1080i") ){
				if( g_strv_contains(resolution, "1080p" ) ){
					copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
					offset_im++;
				}else{
					*(width + offset_width) = 1920;
					offset_width ++;
					*(height + offset_height) = 1080;
					offset_height ++;
					copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
					offset_im++;
				}
			}
			set_filter_field(mpeg_filter, "width", 			width, 	NULL, 			offset_width);
			set_filter_field(mpeg_filter, "height", 		height, NULL, 			offset_height);
			set_filter_field(mpeg_filter,"interlace-mode", 	NULL, 	interlace_mode, MIN(num_inter_mode,offset_im));
		}
	}

	//	set_filter_field(raw_filter, "framerate", GST_VIDEO_FPS_RANGE);
	//	printf("%s", gst_structure_to_string (mpeg_filter) );
	free(width);
	free(height);
	return mpeg_filter;

}

/**
 * \brief Build the J2K filter caps
 * \param gkf a pointer to the opened configuration file
 * \return GstStructure* a caps structure built from the incformation found in configuration file under MPEG-4 group name
 */
static GstStructure* build_J2K_filter(GKeyFile* gkf){
	const gchar * const * 		resolution 	= (const gchar * const *)	get_j2k_res(gkf);
	GstStructure* 				j2k_filter = gst_structure_new_from_string ("image/x-jpc");
	int width_small[length_width_576] 	= {544, 576, 720, 768};
	int heigth_small[length_height_576] = {576, 704};
	int* width 							= (int*) malloc(length_width * sizeof(int));
	int* height 						= (int*) malloc(length_height * sizeof(int));
	char* interlace_mode[num_inter_mode];
	char* interlaced[1]					={"interlaced"};
	char* progressive[1] 				={"progressive"};
	int offset_width					= 0;
	int offset_height					= 0;
	int offset_im 						= 0;

	if ( !no_filter_resolution(resolution)){

		/* Add resolution part */
		if( all_resolutions( resolution ) )
		{
			/* witdth */
			memcpy(width, width_small, length_width_576 * sizeof(int));
			offset_width += length_width_576;
			*(width + offset_width) = 1280;
			offset_width ++;
			*(width + offset_width) = 1920;
			offset_width ++;
			set_filter_field(j2k_filter, "width", 	width, 	NULL, length_width);
			/* height */
			memcpy(height, heigth_small, length_height_576 * sizeof(int));
			offset_height += length_height_576;
			*(height + offset_height) = 720;
			offset_height ++;
			*(height + offset_height) = 1080;
			offset_height ++;
			set_filter_field(j2k_filter, "height", 	height, NULL, length_height);
			/* interlaced-mode */
			copy(interlace_mode, num_inter_mode, 0, interlaced, 1 );
			offset_im++;
			copy(interlace_mode, num_inter_mode, offset_im, progressive, 1 );
			set_filter_field(j2k_filter,"interlace-mode",NULL, interlace_mode, num_inter_mode);
		}else{
			if( g_strv_contains(resolution,"576i") ){
				memcpy(width, width_small, length_width_576 * sizeof(int));
				offset_width += length_width_576;
				memcpy(height, heigth_small, length_height_576 * sizeof(int));
				offset_height += length_height_576;
				copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
				offset_im++;
			}
			if( g_strv_contains(resolution, "720p") ){
				*(width + offset_width) = 1280;
				offset_width ++;
				*(height + offset_height) = 720;
				offset_height ++;
				copy(interlace_mode, num_inter_mode,offset_im , progressive, 1 );
				offset_im++;
			}
			if( g_strv_contains(resolution, "1080p" ) ){
				*(width + offset_width) = 1920;
				offset_width ++;
				*(height + offset_height) = 1080;
				offset_height ++;
				copy(interlace_mode, num_inter_mode,offset_im , progressive, 1 );
				offset_im++;
			}
			else if( g_strv_contains(resolution, "1080i") ){
				if( g_strv_contains(resolution, "1080p" ) ){
					copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
					offset_im++;
				}else{
					*(width + offset_width) = 1920;
					offset_width ++;
					*(height + offset_height) = 1080;
					offset_height ++;
					copy(interlace_mode, num_inter_mode,offset_im , interlaced, 1 );
					offset_im++;
				}
			}
			set_filter_field(j2k_filter, "width", 			width, 	NULL, 			offset_width);
			set_filter_field(j2k_filter, "height", 		height, NULL, 			offset_height);
			set_filter_field(j2k_filter,"interlace-mode", 	NULL, 	interlace_mode, MIN(num_inter_mode,offset_im));
		}
	}

	//	set_filter_field(raw_filter, "framerate", GST_VIDEO_FPS_RANGE);
//	printf("%s", gst_structure_to_string (j2k_filter) );
	free(width);
	free(height);
	return j2k_filter;
}

/**
 * \brief filter out input videos which do not belong to the set of VIVOE's videos by creating a filter
 * froms the caps defined by the user into the configuration file
 * \param input the input video source to be filter
 * \param output the element to which connect filtered the input video
 * \return TRUE if the video is filter out, false otherwise
 */
gboolean filter_VIVOE(GstStructure* input_caps_str, GstElement* input, GstElement* output){
	GstStructure 	*raw_filter 	= NULL;
	GstStructure 	*mpeg_filter 	= NULL;
 	GstStructure 	*j2k_filter 	= NULL;
	GstCaps 		*vivoe_filter 	= NULL; /* the final filter to use to filters out non stream video */

	/* this function does the following:
	 * _ open configuration file
	 * _ read and build filters (RAW, MPEG, J2k) from configuration file
	 *  _close configuration file
	 * _ create full caps from the three caps return
	 * _ link filtered input --> output
	 */

	/* open configuration file */
	GKeyFile*  gkf = open_configuration_file();
	if(gkf == NULL)
		return FALSE; /* if not configuration file was found, return NULL, meaning that we will be using the defaults filter for VIVOE formats*/
	/* Check if RAW fortmat is used bu the user */
	if( vivoe_use_format(gkf, RAW_GROUP_NAME ))
		raw_filter 	= build_RAW_filter(gkf);
	if( vivoe_use_format(gkf,  MPEG4_GROUP_NAME) )
		mpeg_filter = build_MPEG4_filter(gkf);
	if( vivoe_use_format(gkf, J2K_GROUP_NAME ))
		j2k_filter 	= build_J2K_filter(gkf);

	/* close configuration file */
	close_configuration_file(gkf);
	vivoe_filter = gst_caps_new_full (  raw_filter,
										mpeg_filter,
										j2k_filter,
										NULL);
	/* When building a new caps from a Structure, the structure is not copied, instead the caps own the structure
	 * it is not wan we want there, so we built a copy the structure before building the input_caps */
	GstCaps *input_caps = gst_caps_new_full(gst_structure_copy(input_caps_str), NULL);

	if(	gst_caps_can_intersect(input_caps, vivoe_filter)){
		if ( !gst_element_link_log (input, output ))
			return FALSE;
		return TRUE;
	}else{
		g_print ("WARNING: Video source input has been filtered out: not a VIVOE format!");
		return FALSE;
	}
}

