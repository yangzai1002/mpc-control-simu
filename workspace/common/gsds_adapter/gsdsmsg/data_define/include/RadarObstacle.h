#pragma once

//#include <msgpack.hpp>
#include "msghandle.hpp"
#include "register_datatype.h"
#include "vector"
/*
毫米波雷达数据格式
*/

typedef struct RadarObstacle{
	std::vector<msghandle::type::fix_int32>	key_hash_list;	//
	int	id;	//obstacle ID
	float	x;	//longitude distance to the radar; (+) = forward; unit = m
	float	y;	//lateral distance to the radar; (+) = left; unit = m
	float	vxrel;	//longitude velocity to the radar; (+) = forward; unit = m/s
	float	vyrel;	//lateral velocity to the radar; (+) = left; unit = m/s
	float	dist_long_rms;	//longitude distance standard deviation to the radar; (+) = forward; unit = m
	float	dist_lat_rms;	//lateral distance standard deviation to the radar; (+) = left; unit = m
	float	vrel_long_rms;	//longitude velocity standard deviation to the radar; (+) = forward; unit = m/s
	float	vrel_lat_rms;	//lateral velocity standard deviation to the radar; (+) = left; unit = m/s
	unsigned char	dyn_prop;	//0: moving, 1: stationary, 2: oncoming, 3: stationary candidate, 4: unknown, 5: crossing stationary, 6: crossing moving, 7: stopped
	float	rcs;	//obstacle Radar Cross-Section; unit = dBsm
	float	prob_of_exist;	//obstacle probability of existence
	float	arel_long;	//This is only valid for the track object message. Longitude acceleration to the radar; (+) = forward; unit = m/s2
	float	arel_lat;	//This is only valid for the track object message. Lateral acceleration to the radar; (+) = left; unit = m/s2
	float	arel_long_rms;	//longitude acceleration standard deviation to the radar; (+) = forward; unit = m/s2
	float	arel_lat_rms;	//lateral acceleration standard deviation to the radar; (+) = left; unit = m/s2
	float	orientation_angle;	//orientation angle to the radar; (+) = counterclockwise; unit = degree
	float	orientation_rms;	//orientation angle standard deviation to the radar; (+) = counterclockwise; unit = degree
	unsigned char	object_class;	//0: point; 1: car; 2: truck; 3: pedestrian; 4: motorcycle; 5: bicycle; 6: wide; 7: unknown
	unsigned char	meas_state;	//0: deleted; 1: new; 2: measured; 3: predicted; 4: deleted for; 5: new from merge
	float	length;	//obstacle length; unit = m
	float	width;	//# obstacle width; unit = m
	unsigned char	ambig_state;	//This is only valid for the cluster object message. 0: invalid; 1: ambiguous; 2: staggered ramp; 3: unambiguous; 4: stationary candidates
	unsigned char	invalid_state;	//This is only valid for the cluster object message. 0: Valid; 1: Invalid due to low RCS; 2: Invalid due to near-field artefact, 3: Invalid far range Cluster because not confirmed in near range; 4....

	RadarObstacle(){
		key_hash_list.resize(28);
		key_hash_list.push_back(msghandle::type::fix_int32(1092350640));
		key_hash_list.push_back(msghandle::type::fix_int32(1623684982));
		key_hash_list.push_back(msghandle::type::fix_int32(263367596));
		key_hash_list.push_back(msghandle::type::fix_int32(143281483));
		key_hash_list.push_back(msghandle::type::fix_int32(1910968997));
		key_hash_list.push_back(msghandle::type::fix_int32(1954172272));
		key_hash_list.push_back(msghandle::type::fix_int32(1900642544));
		key_hash_list.push_back(msghandle::type::fix_int32(305880245));
		key_hash_list.push_back(msghandle::type::fix_int32(1909223615));
		key_hash_list.push_back(msghandle::type::fix_int32(91431668));
		key_hash_list.push_back(msghandle::type::fix_int32(825424600));
		key_hash_list.push_back(msghandle::type::fix_int32(257892411));
		key_hash_list.push_back(msghandle::type::fix_int32(1735191038));
		key_hash_list.push_back(msghandle::type::fix_int32(1652454485));
		key_hash_list.push_back(msghandle::type::fix_int32(819645334));
		key_hash_list.push_back(msghandle::type::fix_int32(187016953));
		key_hash_list.push_back(msghandle::type::fix_int32(558494562));
		key_hash_list.push_back(msghandle::type::fix_int32(934861631));
		key_hash_list.push_back(msghandle::type::fix_int32(1631908010));
		key_hash_list.push_back(msghandle::type::fix_int32(1139706361));
		key_hash_list.push_back(msghandle::type::fix_int32(1120435128));
		key_hash_list.push_back(msghandle::type::fix_int32(436706170));
		key_hash_list.push_back(msghandle::type::fix_int32(620650691));
		key_hash_list.push_back(msghandle::type::fix_int32(276454031));
		key_hash_list.push_back(msghandle::type::fix_int32(1312989635));
		Clear();
	}

void Clear(){
	id={};
	x={};
	y={};
	vxrel={};
	vyrel={};
	dist_long_rms={};
	dist_lat_rms={};
	vrel_long_rms={};
	vrel_lat_rms={};
	dyn_prop={};
	rcs={};
	prob_of_exist={};
	arel_long={};
	arel_lat={};
	arel_long_rms={};
	arel_lat_rms={};
	orientation_angle={};
	orientation_rms={};
	object_class={};
	meas_state={};
	length={};
	width={};
	ambig_state={};
	invalid_state={};
}
	MSGHANDLE_IS_CONTINUOUS_MEM(1)
	MSGHANDLE_DEFINE(key_hash_list,id,x,y,vxrel,vyrel,dist_long_rms,dist_lat_rms,vrel_long_rms,vrel_lat_rms,dyn_prop,rcs,prob_of_exist,arel_long,arel_lat,arel_long_rms,arel_lat_rms,orientation_angle,orientation_rms,object_class,meas_state,length,width,ambig_state,invalid_state);
	MSGHANDLE_PARAKLIST_DEFINE("key_hash_list","id","x","y","vxrel","vyrel","dist_long_rms","dist_lat_rms","vrel_long_rms","vrel_lat_rms","dyn_prop","rcs","prob_of_exist","arel_long","arel_lat","arel_long_rms","arel_lat_rms","orientation_angle","orientation_rms","object_class","meas_state","length","width","ambig_state","invalid_state");
	MSGHANDLE_PARAVLIST_DEFINE(key_hash_list,id,x,y,vxrel,vyrel,dist_long_rms,dist_lat_rms,vrel_long_rms,vrel_lat_rms,dyn_prop,rcs,prob_of_exist,arel_long,arel_lat,arel_long_rms,arel_lat_rms,orientation_angle,orientation_rms,object_class,meas_state,length,width,ambig_state,invalid_state);
} RadarObstacle;
REGISTER(RadarObstacle,RadarObstacle_Func);
