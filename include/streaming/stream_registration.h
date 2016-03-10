/*
 * Licence: GPL
 * Created: Wed, 10 Feb 2016 14:36:51 +0100
 * Main authors:
 *     - hoel <hoel.vasseur@openwide.fr>
 */
#ifndef STREAM_REGISTRATION_H
# define STREAM_REGISTRATION_H

#include "../../include/videoFormatInfo/videoFormatTable.h"
typedef struct{
	long 	rtp_type; /* for all video Formats */
	int 	clock_rate; /* for all video Formats */
	char 	*profile_level_id; /* Only for MPEG-4 */
	char 	*config; /* Only for MPEG-4 */
}rtp_data ;

void fill_entry(GstStructure* source_str_caps, struct videoFormatTable_entry *video_info,gpointer stream_datas);
int initialize_videoFormat(struct videoFormatTable_entry *video_info, gpointer stream_datas,long *channel_entry_index );

#endif /* STREAM_REGISTRATION_H */

