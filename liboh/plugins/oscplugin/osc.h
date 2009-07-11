//
// C++ Interface: osc
//
// Description: 
//
//
// Author: Robert Hamilton <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

namespace oscplugin {
typedef struct {
	float ball_x;
	float ball_y;
	float ball_z;
	char *hostname;
	char *port;
	int clientnum;
	float origin[3];	
} ball_coordinates;


typedef struct {
  char *hostname;
  char *port;
  int clientnum;
  float origin[3];
  int pm_flags;
  int weapon;
  int weaponstate;
  int jumppad_ent;
  int damageEvent;
  int damageYaw;
  int damagePitch;
  int damageCount;
  int surfaceFlags;
  int groundEntityNum;
  char *classname;
  char *hostnames[20]; // rkh - added array of client IPs
/* additional Data points used in q3apd and for future use in q3osc: */
  int velocity[3];
  int viewangles[3];
  int delta_angles[3];
} osc_client_vars;



typedef struct {
  float g_gravity;
  int g_gravity_update;
  int g_speed;
  int g_speed_update;
  int g_homing_speed;
  int g_homing_speed_update;
  int testvar;
} osc_input_vars;


//void sendOSCmessage(int clientno, char *hostname, char *portnumber);
void sendOSCbundle(osc_client_vars currentClient);
void sendOSCmessage(osc_client_vars currentClient);

//void sendOSCbundle_projectile(osc_projectile_vars currentProjectile);
//void sendOSCmessage_projectile(osc_projectile_vars currentProjectile); //, char *type);

void receiveOSCmessage( void ); // osc listener method

osc_input_vars getOSCmessage( void ); // test data retrieval

void getIPAddress (int index);

}

