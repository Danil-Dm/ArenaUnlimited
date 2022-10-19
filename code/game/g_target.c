/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
#include "g_local.h"

//==========================================================

/*QUAKED target_give (1 0 0) (-8 -8 -8) (8 8 8)
Gives the activator all the items pointed to.
GIVE_PLAYER : targeted item is always given to player instead of activating entity.
*/
void Use_Target_Give( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	gentity_t	*t;
	trace_t		trace;

	if ( !activator->client ) {
		return;
	}

	if ( !ent->target ) {
		return;
	}

	memset( &trace, 0, sizeof( trace ) );
	t = NULL;
	while ( (t = G_Find (t, FOFS(targetname), ent->target)) != NULL ) {
		if ( !t->item ) {
			continue;
		}
		Touch_Item( t, activator, &trace );

		// make sure it isn't going to respawn or show any events
		t->nextthink = 0;
		trap_UnlinkEntity( t );
	}
}

void SP_target_give( gentity_t *ent ) {
	ent->use = Use_Target_Give;
	SandboxObject( ent );
}


//==========================================================

/*QUAKED target_remove_powerups (1 0 0) (-8 -8 -8) (8 8 8)
takes away all the activators powerups.
Used to drop flight powerups into death puts.
*/
void Use_target_remove_powerups( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	if( !activator->client ) {
		return;
	}

	if( activator->client->ps.powerups[PW_REDFLAG] ) {
		Team_ReturnFlag( TEAM_RED );
	} else if( activator->client->ps.powerups[PW_BLUEFLAG] ) {
		Team_ReturnFlag( TEAM_BLUE );
	} else if( activator->client->ps.powerups[PW_NEUTRALFLAG] ) {
		Team_ReturnFlag( TEAM_FREE );
	}

	memset( activator->client->ps.powerups, 0, sizeof( activator->client->ps.powerups ) );
}

void SP_target_remove_powerups( gentity_t *ent ) {
	ent->use = Use_target_remove_powerups;
SandboxObject( ent );
}


//==========================================================

/*QUAKED target_delay (1 0 0) (-8 -8 -8) (8 8 8)
"wait" seconds to pause before firing targets.
"random" delay variance, total delay = delay +/- random seconds
*/
void Think_Target_Delay( gentity_t *ent ) {
	if (!ent->r.linked)
		return;

	ent->nextthink = 0;
	G_UseTargets( ent, ent->activator );
}

void Use_Target_Delay( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	if (!ent->r.linked)
		return;

	if ( ent->nextthink && (ent->spawnflags & 1) ) {
		ent->nextthink = 0;
	} else {
	ent->nextthink = level.time + ( ent->wait + ent->random * crandom() ) * 1000;
	ent->think = Think_Target_Delay;
	ent->activator = activator;
	}
}

void SP_target_delay( gentity_t *ent ) {
	// check delay for backwards compatability
	if ( !G_SpawnFloat( "delay", "0", &ent->wait ) ) {
		G_SpawnFloat( "wait", "1", &ent->wait );
	}

	if ( !ent->wait ) {
		ent->wait = 1;
	}
	ent->use = Use_Target_Delay;
	ent->r.linked = qtrue;
SandboxObject( ent );
}


//==========================================================

/*QUAKED target_score (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void Use_Target_Score (gentity_t *ent, gentity_t *other, gentity_t *activator) {
	AddScore( activator, ent->r.currentOrigin, ent->count );
}

void SP_target_score( gentity_t *ent ) {
	if ( !ent->count ) {
		ent->count = 1;
	}
	ent->use = Use_Target_Score;
SandboxObject( ent );
}

//==========================================================

/*QUAKED target_cmd (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void Use_Target_Cmd (gentity_t *ent, gentity_t *other, gentity_t *activator) {
	trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", ent->target ) );
}

void SP_target_cmd( gentity_t *ent ) {
	if ( !ent->target ) {
G_Printf ("No target in target_cmd\n");
	}
	ent->use = Use_Target_Cmd;
SandboxObject( ent );
}


/*QUAKED target_cmd (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void Use_Target_Music (gentity_t *ent, gentity_t *other, gentity_t *activator) {
trap_SendServerCommand(activator-g_entities, va( "cmusic %s", ent->target));
}

void SP_target_music( gentity_t *ent ) {
	if ( !ent->target ) {
G_Printf ("No target in target_music\n");
	}
	ent->use = Use_Target_Music;
SandboxObject( ent );
}

/*QUAKED target_stats for ArenaUnlimited (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void Use_Target_Stats (gentity_t *ent, gentity_t *other, gentity_t *activator) {
if(!ent->spawnflags){
if(ent->type == 1){
activator->client->ps.stats[STAT_HEALTH] = ent->count;
}
if(ent->type == 2){
activator->client->ps.stats[STAT_HOLDABLE_ITEM] = ent->count;
}
if(ent->type == 3){
activator->client->ps.stats[STAT_PERSISTANT_POWERUP] = ent->count;
}
if(ent->type == 4){
activator->client->ps.stats[STAT_WEAPONS] = ent->count;
}
if(ent->type == 5){
activator->client->ps.stats[STAT_ARMOR] = ent->count;
}
if(ent->type == 6){
activator->client->ps.stats[STAT_MAX_HEALTH] = ent->count;
}
if(ent->type == 7){
activator->client->ps.stats[STAT_NO_PICKUP] = ent->count;
}
if(ent->type == 8){
activator->client->ps.stats[STAT_WEAPONLIST] = ent->count;
activator->weaponpack = ent->count;
}
if(ent->type == 9){
activator->client->ps.stats[STAT_MONEY] = ent->count;
activator->client->pers.oldmoney = ent->count;
}
if(ent->type == 10){
activator->client->ps.stats[STAT_FLASH] = ent->count;
}
}
if(ent->spawnflags == 2){
if(ent->type == 1){
activator->client->ps.stats[STAT_HEALTH] += ent->count;
}
if(ent->type == 2){
activator->client->ps.stats[STAT_HOLDABLE_ITEM] += ent->count;
}
if(ent->type == 3){
activator->client->ps.stats[STAT_PERSISTANT_POWERUP] += ent->count;
}
if(ent->type == 4){
activator->client->ps.stats[STAT_WEAPONS] += ent->count;
}
if(ent->type == 5){
activator->client->ps.stats[STAT_ARMOR] += ent->count;
}
if(ent->type == 6){
activator->client->ps.stats[STAT_MAX_HEALTH] += ent->count;
}
if(ent->type == 7){
activator->client->ps.stats[STAT_NO_PICKUP] += ent->count;
}
if(ent->type == 8){
activator->client->ps.stats[STAT_WEAPONLIST] += ent->count;
activator->weaponpack += ent->count;
}
if(ent->type == 9){
activator->client->ps.stats[STAT_MONEY] += ent->count;
activator->client->pers.oldmoney += ent->count;
}
if(ent->type == 10){
activator->client->ps.stats[STAT_FLASH] += ent->count;
}
}
}

void SP_target_stats( gentity_t *ent ) {
	if ( !ent->type ) {
G_Printf ("No type in target_stats\n");
	}
	ent->use = Use_Target_Stats;
SandboxObject( ent );
}

/*QUAKED target_cmd (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void Use_Target_Sound (gentity_t *ent, gentity_t *other, gentity_t *activator) {
trap_SendServerCommand(activator-g_entities, va( "csound %s", ent->target));
}

void SP_target_sound( gentity_t *ent ) {
	if ( !ent->target ) {
G_Printf ("No target in target_sound\n");
	}
	ent->use = Use_Target_Sound;
SandboxObject( ent );
}

/*QUAKED target_cmd (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void Use_Target_Model (gentity_t *ent, gentity_t *other, gentity_t *activator) {
trap_SendServerCommand(activator-g_entities, va( "cmodel %s", ent->target));
}

void SP_target_model( gentity_t *ent ) {
	if ( !ent->target ) {
G_Printf ("No target in target_model\n");
	}
	ent->use = Use_Target_Model;
SandboxObject( ent );
}

/*QUAKED target_cmd (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void Use_Target_Head (gentity_t *ent, gentity_t *other, gentity_t *activator) {
trap_SendServerCommand(activator-g_entities, va( "chead %s", ent->target));
}

void SP_target_head( gentity_t *ent ) {
	if ( !ent->target ) {
G_Printf ("No target in target_head\n");
	}
	ent->use = Use_Target_Head;
SandboxObject( ent );
}

/*QUAKED target_cmd (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void Use_Target_Legs (gentity_t *ent, gentity_t *other, gentity_t *activator) {
trap_SendServerCommand(activator-g_entities, va( "clegs %s", ent->target));
}

void SP_target_legs( gentity_t *ent ) {
	if ( !ent->target ) {
G_Printf ("No target in target_legs\n");
	}
	ent->use = Use_Target_Legs;
SandboxObject( ent );
}

//==========================================================

/*QUAKED target_print (1 0 0) (-8 -8 -8) (8 8 8) redteam blueteam private
"message"	text to print
If "private", only the activator gets the message.  If no checks, all clients get the message.
*/
void Use_Target_Print (gentity_t *ent, gentity_t *other, gentity_t *activator) {
	if ( activator->client && ( ent->spawnflags & 4 ) ) {
		trap_SendServerCommand( activator-g_entities, va("cp \"%s\"", ent->message ));
		return;
	}

	if ( ent->spawnflags & 3 ) {
		if ( ent->spawnflags & 1 ) {
			G_TeamCommand( TEAM_RED, va("cp \"%s\"", ent->message) );
		}
		if ( ent->spawnflags & 2 ) {
			G_TeamCommand( TEAM_BLUE, va("cp \"%s\"", ent->message) );
		}
		return;
	}

	trap_SendServerCommand( -1, va("cp \"%s\"", ent->message ));
}

void SP_target_print( gentity_t *ent ) {
	ent->use = Use_Target_Print;
SandboxObject( ent );
}


//==========================================================


/*QUAKED target_speaker (1 0 0) (-8 -8 -8) (8 8 8) looped-on looped-off global activator
"noise"		wav file to play

A global sound will play full volume throughout the level.
Activator sounds will play on the player that activated the target.
Global and activator sounds can't be combined with looping.
Normal sounds play each time the target is used.
Looped sounds will be toggled by use functions.
Multiple identical looping sounds will just increase volume without any speed cost.
"wait" : Seconds between auto triggerings, 0 = don't auto trigger
"random"	wait variance, default is 0
*/
void Use_Target_Speaker (gentity_t *ent, gentity_t *other, gentity_t *activator) {
	if (ent->spawnflags & 3) {	// looping sound toggles
		if (ent->s.loopSound)
			ent->s.loopSound = 0;	// turn it off
		else
			ent->s.loopSound = ent->noise_index;	// start it
	}else {	// normal sound
		if ( ent->spawnflags & 8 ) {
			G_AddEvent( activator, EV_GENERAL_SOUND, ent->noise_index );
		} else if (ent->spawnflags & 4) {
			G_AddEvent( ent, EV_GLOBAL_SOUND, ent->noise_index );
		} else {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->noise_index );
		}
	}
}

void SP_target_speaker( gentity_t *ent ) {
	char	buffer[MAX_QPATH];
	char	*s;

	G_SpawnFloat( "wait", "0", &ent->wait );
	G_SpawnFloat( "random", "0", &ent->random );

	if ( !G_SpawnString( "noise", "NOSOUND", &s ) ) {
		//G_Error( "target_speaker without a noise key at %s", vtos( ent->s.origin ) );
	}

	// force all client reletive sounds to be "activator" speakers that
	// play on the entity that activates it
	if ( s[0] == '*' ) {
		ent->spawnflags |= 8;
	}

	if (!strstr( s, ".wav" )) {
		Com_sprintf (buffer, sizeof(buffer), "%s.wav", s );
	} else {
		Q_strncpyz( buffer, s, sizeof(buffer) );
	}
	ent->noise_index = G_SoundIndex(buffer);

	// a repeating speaker can be done completely client side
	ent->s.eType = ET_SPEAKER;
	ent->s.eventParm = ent->noise_index;
	ent->s.frame = ent->wait * 10;
	ent->s.clientNum = ent->random * 10;


	// check for prestarted looping sound
	if ( ent->spawnflags & 1 ) {
		ent->s.loopSound = ent->noise_index;
	}

	ent->use = Use_Target_Speaker;

	if (ent->spawnflags & 4) {
		ent->r.svFlags |= SVF_BROADCAST;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	// must link the entity so we get areas and clusters so
	// the server can determine who to send updates to
	trap_LinkEntity( ent );
}



//==========================================================

/*QUAKED target_laser (0 .5 .8) (-8 -8 -8) (8 8 8) START_ON
When triggered, fires a laser.  You can either set a target or a direction.
*/
void target_laser_think (gentity_t *self) {
	vec3_t	end;
	trace_t	tr;
	vec3_t	point;

	// if pointed at another entity, set movedir to point at it
	if ( self->enemy ) {
		VectorMA (self->enemy->s.origin, 0.5, self->enemy->r.mins, point);
		VectorMA (point, 0.5, self->enemy->r.maxs, point);
		VectorSubtract (point, self->s.origin, self->movedir);
		VectorNormalize (self->movedir);
	}

	// fire forward and see what we hit
	VectorMA (self->s.origin, 2048, self->movedir, end);

	trap_Trace( &tr, self->s.origin, NULL, NULL, end, self->s.number, CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE);

	if ( tr.entityNum ) {
		// hurt it if we can
		G_Damage ( &g_entities[tr.entityNum], self, self->activator, self->movedir, 
			tr.endpos, self->damage, DAMAGE_NO_KNOCKBACK, MOD_TARGET_LASER);
	}

	VectorCopy (tr.endpos, self->s.origin2);

	trap_LinkEntity( self );
	self->nextthink = level.time + FRAMETIME;
}

void target_laser_on (gentity_t *self)
{
	if (!self->activator)
		self->activator = self;
	target_laser_think (self);
}

void target_laser_off (gentity_t *self)
{
	trap_UnlinkEntity( self );
	self->nextthink = 0;
}

void target_laser_use (gentity_t *self, gentity_t *other, gentity_t *activator)
{
	self->activator = activator;
	if ( self->nextthink > 0 )
		target_laser_off (self);
	else
		target_laser_on (self);
}

void target_laser_start (gentity_t *self)
{
	gentity_t *ent;

	self->s.eType = ET_BEAM;

	if (self->target) {
		ent = G_Find (NULL, FOFS(targetname), self->target);
		if (!ent) {
			G_Printf ("%s at %s: %s is a bad target\n", self->classname, vtos(self->s.origin), self->target);
		}
		self->enemy = ent;
	} else {
		G_SetMovedir (self->s.angles, self->movedir);
	}

	self->use = target_laser_use;
	self->think = target_laser_think;

	if ( !self->damage ) {
		self->damage = 1;
	}

	if (self->spawnflags & 1)
		target_laser_on (self);
	else
		target_laser_off (self);
}

void SP_target_laser (gentity_t *self)
{
	char        *sound;
	vec3_t		color;
	int			r, g, b, i;

	// if the "noise" key is set, use a constant looping sound when moving
	if (G_SpawnString("noise", "100", &sound)) {
		self->s.loopSound = G_SoundIndex(sound);
	}
	// let everything else get spawned before we start firing
	self->think = target_laser_start;
	self->nextthink = level.time + FRAMETIME;

	//beam color
	G_SpawnVector("color", "1 0 0", color);

	r = color[0] * 255;
	if (r > 255) {
		r = 255;
	}
	g = color[1] * 255;
	if (g > 255) {
		g = 255;
	}
	b = color[2] * 255;
	if (b > 255) {
		b = 255;
	}
	/*
	i = light / 4;
	if (i > 255) {
		i = 255;
	}
	self->s.constantLight = r | (g << 8) | (b << 16) | (i << 24);
	*/
	self->s.constantLight = r | (g << 8) | (b << 16);
}


//==========================================================

void target_teleporter_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	gentity_t	*dest;

	if (!activator->client)
		return;
	dest = 	G_PickTarget( self->target );
	if (!dest) {
                G_Printf ("Couldn't find teleporter destination\n");
		return;
	}

	TeleportPlayer( activator, dest->s.origin, dest->s.angles );
}

/*QUAKED target_teleporter (1 0 0) (-8 -8 -8) (8 8 8)
The activator will be teleported away.
*/
void SP_target_teleporter( gentity_t *self ) {
	if (!self->targetname)
            G_Printf("untargeted %s at %s\n", self->classname, vtos(self->s.origin));

	self->use = target_teleporter_use;
SandboxObject( self );
}

//==========================================================


/*QUAKED target_relay (.5 .5 .5) (-8 -8 -8) (8 8 8) RED_ONLY BLUE_ONLY RANDOM
This doesn't perform any actions except fire its targets.
The activator can be forced to be from a certain team.
if RANDOM is checked, only one of the targets will be fired, not all of them
A count key can be set to delay the triggering until the entity has been triggered [count] number of times
*/
void target_relay_use (gentity_t *self, gentity_t *other, gentity_t *activator) {
	if (!self->r.linked)
		return;

	if ( ( self->spawnflags & 1 ) && activator->client 
		&& activator->client->sess.sessionTeam != TEAM_RED ) {
		return;
	}
	if ( ( self->spawnflags & 2 ) && activator->client 
		&& activator->client->sess.sessionTeam != TEAM_BLUE ) {
		return;
	}
	if ( self->spawnflags & 4 ) {
		gentity_t	*ent;

		ent = G_PickTarget( self->target );
		if ( ent && ent->use ) {
			ent->use( ent, self, activator );
		}

		if ( self->target2 ) {
			ent = G_PickTarget( self->target2 );
			if ( ent && ent->use ) {
				ent->use( ent, self, activator );
			}
		}
		return;
	}

	if (!self->count || self->count < 0) {
		self->count = 1;
	}

	if (!self->damage)		//damage is used to keep track of the number of times the target_relay was triggered
		self->damage = 1;
	else
		self->damage++;

	if (self->damage == self->count)
	{
	G_UseTargets (self, activator);
		
		if ( self->spawnflags & 8 )
			G_FreeEntity( self );
		else
			self->damage = 0;
	}
}

void SP_target_relay (gentity_t *self) {
	self->use = target_relay_use;
	self->r.linked = qtrue;
}

//==========================================================

/*QUAKED target_logic (.5 .5 .5) (-8 -8 -8) (8 8 8) RED_ONLY BLUE_ONLY RANDOM STAY_ON
This doesn't perform any actions except fire its targets when it's triggered by two different triggers.
The activator can be forced to be from a certain team.
if RANDOM is checked, only one of the targets will be fired, not all of them
*/
void target_logic_reset (gentity_t *self) {
	int i;
	for (i = 0; i < MAX_LOGIC_ENTITIES; i++)
		self->logicEntities[i] = 0;
}

void target_logic_use (gentity_t *self, gentity_t *other, gentity_t *activator) {
	int			i;
	int			triggerCount;		//number of entities targetting this target_logic
	int			triggeredCount;		//number of entities targetting this target_logic that have already been triggered
	qboolean	found;
	gentity_t	*t;

	//determine the number of entities (triggers) targeting this target_logic
	triggerCount = triggeredCount = 0;
	t = NULL;
	while ( (t = G_Find (t, FOFS(target), self->targetname)) != NULL ) {
		found = qfalse;
		if ( t == self ) {
			G_Printf ("WARNING: Entity %s at %s used itself.\n", self->classname, vtos(self->s.origin));
		} else {
			triggerCount++;
		}
	}

	//count the number of entities that have already triggered the target_logic
	for ( i = 0; i < MAX_LOGIC_ENTITIES; i++ ) {
		if ( self->logicEntities[i] ) {
			triggeredCount++;

			//if the entity that is currently being triggered is in the list, see if we should remove it again
			if ( self->logicEntities[i] == other->s.number ) {
				found = qtrue;
				
				//STAY_ON is not selected, remove the trigger for the list of triggered entities
				if ( !(self->spawnflags & 8) ) {
					self->logicEntities[i] = 0;
					triggeredCount--;		//the trigger was counted as being triggerd, but is not so anymore
				}
			}
		}
	}

	//the entity was not yet in the list of triggered entities, so add it
	if ( !found ) {
		for ( i = 0; i < MAX_LOGIC_ENTITIES; i++ ) {
			if ( !self->logicEntities[i] )
			{
				self->logicEntities[i] = other->s.number;
				triggeredCount++;
				break;
			}
		}
	}
       
	if ( triggerCount == triggeredCount ) {
		target_logic_reset( self );

		//If RANDOM is selected so use a random target
		if ( self->spawnflags & 4 ) {
			gentity_t	*ent;

			ent = G_PickTarget( self->target );
			if ( ent && ent->use ) {
				ent->use( ent, self, activator );
			}
			return;
		}
		
		//The RANDOM spawnflag wasn't selected, so use all targets
		G_UseTargets (self, activator);
	}
}



void SP_target_logic (gentity_t *self) {
	self->use = target_logic_use;
	target_logic_reset( self );
}

//==========================================================
/*QUAKED target_disable (.5 .5 .5) (-8 -8 -8) (8 8 8) - - ALWAYS_UNLINK ALWAYS_LINK IMMEDIATELY
links or unlinks entities from the world, effectively enabling or disabling triggers
*/
void target_unlink_use (gentity_t *self, gentity_t *other, gentity_t *activator) {
	G_ToggleTargetsLinked( self );
}

//used for immediately spawnflag
void target_unlink_think (gentity_t *self) {
	self->nextthink = 0;
	G_ToggleTargetsLinked( self );
}

void SP_target_unlink (gentity_t *self) {
	self->use = target_unlink_use;
	
	if ( ( self->spawnflags & 16 ) ) {
		self->nextthink = level.time + FRAMETIME * 3;	//unlink entities next frame so they can spawn first
		self->think = target_unlink_think;
	}
}

//==========================================================
//==========================================================

/*QUAKED target_debrisemitter (.5 .5 .5) (-8 -8 -8) (8 8 8) see PickDebrisType in g_util.c for spawnflags
Emits chunks of debris.
If no spawnflag is set, the entity will emit light chunks of concrete
*/

void target_debrisemitter_use (gentity_t *self, gentity_t *other, gentity_t *activator) {
	gentity_t *ent;
	
	if ( !self->count )
		self->count = 10;

	ent = G_TempEntity( self->s.origin, PickDebrisType( self->spawnflags ) );
	ent->s.eventParm = self->count;
}

void SP_target_debrisemitter (gentity_t *self) {
	self->use = target_debrisemitter_use;
}

//==========================================================
//==========================================================

/*QUAKED target_effect (.5 .5 .5) (-8 -8 -8) (8 8 8) EXPLOSION PARTICLES_GRAVITY PARTICLES_LINEAR PARTICLES_LINEAR_UP PARTICLES_LINEAR_DOWN OVERLAY FADE
shows animated environmental effect
The EXPLOSION spawnflag will cause the entity to show an explosion
The PARTICLES_GRAVITY spawnflag will cause the entity to show particles which are affected by gravity and drop to the ground
The PARTICLES_LINEAR spawnflag will cause the entity to show particles which are not affected by gravity and move in a straight line
color key takes normalized color values (0.0 - 1.0)
startcolor key takes normalized color values (0.0 - 1.0)
endcolor key takes normalized color values (0.0 - 1.0)
count key takes int (0 - 255)
speed key takes int (0 - 255)
*/

void target_effect_use (gentity_t *self, gentity_t *other, gentity_t *activator) {
	gentity_t	*ent2;
	gentity_t	*ent3;
	gentity_t	*ent4;
	gentity_t	*ent5;
	gentity_t   *ent6;

	if ( !self->r.linked )
		return;

	//explosion
	if ( self->spawnflags & SF_EFFECT_EXPLOSION ) {
		G_TempEntity( self->s.origin, EV_EXPLOSION );
	}

	//particles_gravity
	if ( self->spawnflags & SF_EFFECT_PARTICLES_GRAVITY ) {
		ent2 = G_TempEntity( self->s.origin, EV_PARTICLES_GRAVITY );
		ent2->s.constantLight = self->s.constantLight;	//constantLight is used to determine particle color
		ent2->s.eventParm = self->count; //eventParm is used to determine the number of particles
		ent2->s.generic1 = self->speed; //generic1 is used to determine the speed of the particles
	}

	//particles_linear
	if ( self->spawnflags & SF_EFFECT_PARTICLES_LINEAR ) {
		ent3 = G_TempEntity( self->s.origin, EV_PARTICLES_LINEAR );
		ent3->s.constantLight = self->s.constantLight;	//constantLight is used to determine particle color
		ent3->s.eventParm = self->count; //eventParm is used to determine the number of particles
		ent3->s.generic1 = self->speed; //generic1 is used to determine the speed of the particles
	}

	//particles_linear_up
	if ( self->spawnflags & SF_EFFECT_PARTICLES_LINEAR_UP ) {
		ent4 = G_TempEntity( self->s.origin, EV_PARTICLES_LINEAR_UP );
		ent4->s.constantLight = self->s.constantLight;	//constantLight is used to determine particle color
		ent4->s.eventParm = self->count; //eventParm is used to determine the number of particles
		ent4->s.generic1 = self->speed; //generic1 is used to determine the speed of the particles
	}

	//particles_linear_down
	if ( self->spawnflags & SF_EFFECT_PARTICLES_LINEAR_DOWN ) {
		ent5 = G_TempEntity( self->s.origin, EV_PARTICLES_LINEAR_DOWN );
		ent5->s.constantLight = self->s.constantLight;	//constantLight is used to determine particle color
		ent5->s.eventParm = self->count; //eventParm is used to determine the number of particles
		ent5->s.generic1 = self->speed; //generic1 is used to determine the speed of the particles
	}

	//overlay
	if ( self->spawnflags & SF_EFFECT_OVERLAY ) {
		if ( self->overlay ) {
			trap_SetConfigstring( CS_OVERLAY, self->overlay );
		} else {
			trap_SetConfigstring( CS_OVERLAY, "" );
		}
		
		//send event to player which will register the asset so it may subsequently be drawn each frame.
		G_TempEntity( self->s.origin, EV_OVERLAY );
	}

	//fade
	if ( self->spawnflags & SF_EFFECT_FADE ) {
		G_Fade( self->wait, self->rgba1, self->rgba2 ); 
	}

	//smoke
	if (self->spawnflags & SF_EFFECT_SMOKEPUFF) {
		ent6 = G_TempEntity(self->s.origin, EV_SMOKEPUFF);
		ent6->s.constantLight = self->s.constantLight;
		ent6->s.eventParm = self->wait;	//eventParm is used to determine the amount of time the smoke puff exists
		ent6->s.generic1 = self->speed;	//generic1 is used to determine the movement speed of the smoke puff
		ent6->s.otherEntityNum = self->distance * 32; //otherEntityNum is used to determine the size of the smokepuff. The default is 32.
		VectorCopy(self->s.angles, ent6->s.angles);
	}
}

void target_effect_think(gentity_t* self) {
	gentity_t* tmp;
	vec3_t		wut;
	vec3_t		dir;

	if (self->target) {
		tmp = G_PickTarget(self->target);
		VectorSubtract(tmp->s.origin, self->s.origin, dir);
		VectorCopy(dir, self->s.angles);
	}
	else if (self->target2) {
		tmp = G_PickTarget(self->target2);
		VectorSubtract(tmp->s.origin, self->s.origin, dir);
		VectorCopy(dir, self->s.angles);
	}
	else {
		self->s.angles[0] = 0;
		self->s.angles[1] = 0;
		self->s.angles[2] = 1;
	}
	VectorNormalize(self->s.angles);

	self->nextthink = 0;
}

void SP_target_effect (gentity_t *self) {
	vec3_t		color;
	float		light;
	int			r, g, b, i;
	float		scale;
	
	//check if effects are selected
	if ( !self->spawnflags ) {
		G_Printf( va( S_COLOR_YELLOW "WARNING: %s without selected effects at %s\n", self->classname, vtos(self->s.origin) ) );
		G_FreeEntity( self );
	}
	
	// particle/smoke color
	if ( 
		self->spawnflags & SF_EFFECT_PARTICLES_GRAVITY || 
		self->spawnflags & SF_EFFECT_PARTICLES_LINEAR || 
		self->spawnflags & SF_EFFECT_PARTICLES_LINEAR_UP ||
		self->spawnflags & SF_EFFECT_PARTICLES_LINEAR_DOWN ||
		self->spawnflags & SF_EFFECT_SMOKEPUFF
	) {
		G_SpawnVector( "color", "1 1 1", color );

		r = color[0] * 255;
		if ( r > 255 ) {
			r = 255;
		}
		g = color[1] * 255;
		if ( g > 255 ) {
			g = 255;
		}
		b = color[2] * 255;
		if ( b > 255 ) {
			b = 255;
		}
		i = light / 4;
		if ( i > 255 ) {
			i = 255;
		}
		self->s.constantLight = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );
	}

	//particle count
	if ( 
		self->spawnflags & SF_EFFECT_PARTICLES_GRAVITY || 
		self->spawnflags & SF_EFFECT_PARTICLES_LINEAR || 
		self->spawnflags & SF_EFFECT_PARTICLES_LINEAR_UP ||
		self->spawnflags & SF_EFFECT_PARTICLES_LINEAR_DOWN
	) {
		if ( !self->count )
			self->count = 100;
		else if (self->count > 255)
			self->count = 255;

		if ( !self->speed )
			self->speed = 100;
	}

	//smoke speed, duration and angle
	if (self->spawnflags & SF_EFFECT_SMOKEPUFF) {
		if ( !self->speed ) {
			self->speed = 16;
		}
		if (!self->wait) {
			self->wait = 2;
		}
		if (!G_SpawnFloat("scale", "1", &scale)) {
			scale = 1;
		}

		self->distance = scale;		//abuse the distance field for scale
		
		self->nextthink = level.time + FRAMETIME * 3;
		self->think = target_effect_think;
	}

	//preload explosion assets if necessary
	if ( self->spawnflags & SF_EFFECT_EXPLOSION ) {
		RegisterItem( BG_FindItemForWeapon( WP_ROCKET_LAUNCHER ) );	//uses RL gfx so we must register the RL
	}

	//fade info
	if ( self->spawnflags & SF_EFFECT_FADE ) {
		G_SpawnVector4( "startcolor", "0 0 0 0", self->rgba1 );
		G_SpawnVector4( "endcolor", "0 0 0 1", self->rgba2 );
		if ( !self->wait ) {
			self->wait = 2;
		}
	}

	self->use = target_effect_use;

	trap_LinkEntity( self );
}

//==========================================================
//==========================================================

/*QUAKED target_modify (.5 .5 .5) (-8 -8 -8) (8 8 8)
When triggered, modifies the value of the specified key on the entities the target_modify targets
*/
void modify_entity ( gentity_t *self, gentity_t *ent ) {
	if ( !strcmp( self->key, "spawnflags" ) ) {
		ent->spawnflags = atoi(self->value);
		return;
	}

	if (!strcmp(self->key, "wait")) {
		ent->wait = atof(self->value);
		return;
	}

	if ( !strcmp( self->key, "message" ) ) {
		ent->message = self->value;
		return;
	}

	if ( !strcmp( self->key, "target" ) ) {
		ent->target = self->value;
		if ( !strcmp( ent->classname, "path_corner" ) )
			Think_SetupTrainTargets( ent );
		
		if ( strstr( ent->classname, "shooter_" ) ) {
			InitShooter( ent, ent->s.weapon );
		}

		return;
	}

	if ( !strcmp( self->key, "targetname" ) ) {
		ent->targetname = self->value;
		if ( !strcmp( ent->classname, "path_corner" ) )
			Think_SetupTrainTargets( ent );
		return;
	}

	if ( !strcmp( self->key, "targetshadername" ) ) {
		ent->targetShaderName = self->value;
		return;
	}

	if ( !strcmp( self->key, "targetnewshadername" ) ) {
		ent->targetShaderNewName = self->value;
		return;
	}

	if ( !strcmp( self->key, "angle" ) ) {
		ent->angle = atof(self->value);
		return;
	}

	if ( !strcmp( self->key, "speed" ) ) {
		ent->speed = atof(self->value);
		return;
	}

	if ( !strcmp( self->key, "health" ) ) {
		ent->health = atoi(self->value);
		return;
	}

	if ( !strcmp( self->key, "count" ) ) {
		ent->count = atoi(self->value);
		return;
	}

	if ( !strcmp( self->key, "wait" ) ) {
		ent->wait = atof(self->value);
		return;
	}

	if ( !strcmp( self->key, "clientname" ) ) {
		ent->clientname = self->value;
		return;
	}

	if ( !strcmp( self->key, "mapname" ) ) {
		ent->mapname = self->value;
		return;
	}
	
	if ( !strcmp( self->key, "teleportertarget" ) ) {
		ent->teleporterTarget = self->value;
		return;
	}

	if ( !strcmp( self->key, "target2" ) ) {
		ent->target2 = self->value;
		return;
	}

	if ( !strcmp( self->key, "targetname2" ) ) {
		ent->targetname2 = self->value;
		return;
	}

	if ( !strcmp( self->key, "deathtarget" ) ) {
		ent->deathTarget = self->value;
		return;
	}

	if ( !strcmp( self->key, "loottarget" ) ) {
		ent->lootTarget = self->value;
		return;
	}

	if ( !strcmp( self->key, "skill" ) ) {
		ent->skill = atof(self->value);
		return;
	}
	
	if ( !strcmp( self->key, "overlay" ) ) {
		ent->overlay = self->value;
		return;
	}

	if ( !strcmp( self->key, "armor" ) ) {
		ent->armor = atoi(self->value);
		return;
	}

	if ( !strcmp( self->key, "key" ) ) {
		ent->key = self->value;
		return;
	}

	if ( !strcmp( self->key, "value" ) ) {
		ent->value = self->value;
		return;
	}

	if ( !strcmp( self->key, "light" ) ) {
		int		cl;
		int r, g, b, i;

		cl = ent->s.constantLight;

		r = (cl & 0xFF);
		g = ((cl >> 8) & 0xFF);
		b = ((cl >> 16) & 0xFF);

		ent->s.constantLight = r | ( g << 8 ) | ( b << 16 ) | ( atoi(self->value) << 24 );		
		return;
	}

	if ( !strcmp( self->key, "color" ) ) {
		int		cl, gn;
		int r, g, b, i;

		cl = ent->s.constantLight;
		gn = self->s.generic1;		

		r = (gn & 0xFF);
		g = ((gn >> 8) & 0xFF);
		b = ((gn >> 16) & 0xFF);
		i = ((cl >> 24) & 0xFF) * 4.0;

		ent->s.constantLight = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );		
		return;
	}

	if ( !strcmp( self->key, "damage" ) || !strcmp( self->key, "dmg" ) ) {
		ent->damage = atoi(self->value);
		return;
	}

	G_Printf("WARNING: Incorrect key \"%s\" for %s at %s\n", self->key, self->classname, vtos(self->s.origin));
}

void target_modify_use (gentity_t *self, gentity_t *other, gentity_t *activator) {
	gentity_t *t;

	t = NULL;
	while ( (t = G_Find (t, FOFS(targetname), self->target)) != NULL ) {
		modify_entity(self, t);
		if ( !self->inuse ) {
			G_Printf("entity was removed while using targets\n");
			return;
		}
	}

	t = NULL;
	while ( (t = G_Find (t, FOFS(targetname2), self->target)) != NULL ) {
		modify_entity(self, t);
		if ( !self->inuse ) {
			G_Printf("entity was removed while using targets\n");
			return;
		}
	}

	t = NULL;
	while ( (t = G_Find (t, FOFS(targetname), self->target2)) != NULL ) {
		modify_entity(self, t);
		if ( !self->inuse ) {
			G_Printf("entity was removed while using targets\n");
			return;
		}
	}

	t = NULL;
	while ( (t = G_Find (t, FOFS(targetname2), self->target2)) != NULL ) {
		modify_entity(self, t);
		if ( !self->inuse ) {
			G_Printf("entity was removed while using targets\n");
			return;
		}
	}
}

void SP_target_modify (gentity_t *self) {
	vec3_t	color;
	int		r, g, b, i;

	self->use = target_modify_use;

	//store color value in generic1
	if ( !strcmp(self->key, "color" ) ) {
		G_SpawnVector( "value", "1 1 1", color );

		r = color[0] * 255;
		if ( r > 255 ) {
			r = 255;
		}
		g = color[1] * 255;
		if ( g > 255 ) {
			g = 255;
		}
		b = color[2] * 255;
		if ( b > 255 ) {
			b = 255;
		}
		i = 0;
		self->s.generic1 = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );
	}
}

//==========================================================

/*QUAKED target_kill (.5 .5 .5) (-8 -8 -8) (8 8 8)
Kills the activator.
*/
void target_kill_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	G_Damage ( activator, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
}

void SP_target_kill( gentity_t *self ) {
	self->use = target_kill_use;
SandboxObject( self );
}

/*QUAKED target_position (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for in-game calculation, like jumppad targets.
*/
void SP_target_position( gentity_t *self ){
	G_SetOrigin( self, self->s.origin );
SandboxObject( self );
}

static void target_location_linkup(gentity_t *ent)
{
	int i;
	int n;
        //static char *gametypeNames[] = {"ffa", "tournament", "single", "team", "ctf", "oneflag", "obelisk", "harvester", "elimination", "ctf", "lms", "dd", "dom"};

	if (level.locationLinked) 
		return;

	level.locationLinked = qtrue;

	level.locationHead = NULL;

	trap_SetConfigstring( CS_LOCATIONS, "unknown" );

	for (i = 0, ent = g_entities, n = 1;
			i < level.num_entities;
			i++, ent++) {
		if (ent->classname && !Q_stricmp(ent->classname, "target_location") ) {
			// lets overload some variables!
			ent->health = n; // use for location marking
			trap_SetConfigstring( CS_LOCATIONS + n, ent->message );
			n++;
			ent->nextTrain = level.locationHead;
			level.locationHead = ent;
		}
	}

	// All linked together now
}

/*QUAKED target_location (0 0.5 0) (-8 -8 -8) (8 8 8)
Set "message" to the name of this location.
Set "count" to 0-7 for color.
0:white 1:red 2:green 3:yellow 4:blue 5:cyan 6:magenta 7:white

Closest target_location in sight used for the location, if none
in site, closest in distance
*/
void SP_target_location( gentity_t *self ){
	self->think = target_location_linkup;
	self->nextthink = level.time + 200;  // Let them all spawn first

	G_SetOrigin( self, self->s.origin );
}

