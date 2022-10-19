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


void InitTrigger( gentity_t *self ) {
	char		*string;
	int			modelid;
	
	if (!VectorCompare (self->s.angles, vec3_origin))
		G_SetMovedir (self->s.angles, self->movedir);
	
	string = self->model;
	modelid = atof( string );
	if(modelid == 1){
	VectorSet( self->r.mins, -25, -25, -25);
	VectorSet( self->r.maxs, 25, 25, 25 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass.md3" );
	} else 
	if(modelid == 2){
	VectorSet( self->r.mins, -50, -50, -50);
	VectorSet( self->r.maxs, 50, 50, 50 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass2.md3" );
	} else
	if(modelid == 3){
	VectorSet( self->r.mins, -250, -250, -250);
	VectorSet( self->r.maxs, 250, 250, 250 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass10.md3" );
	} else
	if(modelid == 4){
	VectorSet( self->r.mins, -500, -500, -500);
	VectorSet( self->r.maxs, 500, 500, 500 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass20.md3" );
	} else
	if(modelid == 5){
	VectorSet( self->r.mins, -50000, -50000, -25);
	VectorSet( self->r.maxs, 50000, 50000, 25 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass.md3" );
	} else
	if(modelid == 6){
	VectorSet( self->r.mins, -50000, -50000, -50);
	VectorSet( self->r.maxs, 50000, 50000, 50 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass2.md3" );
	} else
	if(modelid == 7){
	VectorSet( self->r.mins, -50000, -50000, -250);
	VectorSet( self->r.maxs, 50000, 50000, 250 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass10.md3" );
	} else
	if(modelid == 8){
	VectorSet( self->r.mins, -50000, -50000, -500);
	VectorSet( self->r.maxs, 50000, 50000, 500 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass10.md3" );
	} else
	if(modelid == 9){
	VectorSet( self->r.mins, -50000, -50000, -50000);
	VectorSet( self->r.maxs, 50000, 50000, 50000 );
	self->r.contents = CONTENTS_TRIGGER;
	self->s.modelindex = G_ModelIndex( "models/mapobjects/oasb/glass20.md3" );
	} else {
if(modelid != 12345){
	trap_SetBrushModel( self, self->model );
}
}
	self->r.contents = CONTENTS_TRIGGER;		// replaces the -1 from trap_SetBrushModel
	if(modelid != 1){
	if(modelid != 2){
	if(modelid != 3){
	if(modelid != 4){
	if(modelid != 5){
	if(modelid != 6){
	if(modelid != 7){
	if(modelid != 8){
	if(modelid != 9){
	self->r.svFlags = SVF_NOCLIENT;
	}
	}
	}
	}
	}
	}
	}
	}
	}
SandboxObject( self );
}


// the wait time has passed, so set back up for another activation
void multi_wait( gentity_t *ent ) {
	ent->nextthink = 0;
}


// the trigger was just activated
// ent->activator should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
void multi_trigger( gentity_t *ent, gentity_t *activator ) {
	//if nobots flag is set and activator is a bot, do nothing
	if ( (ent->flags & FL_NO_BOTS) && IsBot( activator ) )
		return;

	//if nohumans flag is set and activtaor is a human, do nothing
	if ( (ent->flags & FL_NO_HUMANS) && !IsBot( activator ) )
		return;
	
	
	ent->activator = activator;
	if ( ent->nextthink ) {
		return;		// can't retrigger until the wait is over
	}

	if ( activator->client ) {
		if ( ( ent->spawnflags & 1 ) &&
			activator->client->sess.sessionTeam != TEAM_RED ) {
			return;
		}
		if ( ( ent->spawnflags & 2 ) &&
			activator->client->sess.sessionTeam != TEAM_BLUE ) {
			return;
		}
	}

	G_UseTargets (ent, ent->activator);

	if ( ent->wait > 0 ) {
		ent->think = multi_wait;
		ent->nextthink = level.time + ( ent->wait + ent->random * crandom() ) * 1000;
	} else {
		// we can't just remove (self) here, because this is a touch function
		// called while looping through area links...
		ent->touch = 0;
		ent->nextthink = level.time + FRAMETIME;
		ent->think = G_FreeEntity;
	}
}

void Use_Multi( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
if(ent->owner != activator->s.clientNum + 1){
if(ent->owner != 0){
trap_SendServerCommand( activator->s.clientNum, va( "cp Owned_by_%s\n", ent->ownername ));
return;
}	
}
if(ent->locked != 0){
return;
}
	multi_trigger( ent, activator );
}

void Touch_Multi( gentity_t *self, gentity_t *other, trace_t *trace ) {
	if( !other->client ) {
		return;
	}
if(self->owner != other->s.clientNum + 1){
if(self->owner != 0){
trap_SendServerCommand( other->s.clientNum, va( "cp Owned_by_%s\n", self->ownername ));
return;
}	
}
if(self->locked != 0){
return;
}
	multi_trigger( self, other );
}

/*QUAKED trigger_multiple (.5 .5 .5) ?
"wait" : Seconds between triggerings, 0.5 default, -1 = one time only.
"random"	wait variance, default is 0
Variable sized repeatable trigger.  Must be targeted at one or more entities.
so, the basic time between firing is a random time between
(wait - random) and (wait + random)
*/
void SP_trigger_multiple( gentity_t *ent ) {
	int		i;

	G_SpawnInt( "nobots", "0", &i);
	if ( i ) {
		ent->flags |= FL_NO_BOTS;
	}
	G_SpawnInt( "nohumans", "0", &i );
	if ( i ) {
		ent->flags |= FL_NO_HUMANS;
	}
	
	G_SpawnFloat( "wait", "0.5", &ent->wait );
	G_SpawnFloat( "random", "0", &ent->random );

	if ( ent->random >= ent->wait && ent->wait >= 0 ) {
		ent->random = ent->wait - FRAMETIME;
                G_Printf( "trigger_multiple has random >= wait\n" );
	}

	ent->touch = Touch_Multi;
	ent->use = Use_Multi;

	InitTrigger( ent );
	trap_LinkEntity (ent);
}



/*
==============================================================================

trigger_always

==============================================================================
*/

void trigger_always_think( gentity_t *ent ) {
	G_UseTargets(ent, ent);
	G_FreeEntity( ent );
}

/*QUAKED trigger_always (.5 .5 .5) (-8 -8 -8) (8 8 8)
This trigger will always fire.  It is activated by the world.
*/
void SP_trigger_always (gentity_t *ent) {
	// we must have some delay to make sure our use targets are present
	ent->nextthink = level.time + 300;
	ent->think = trigger_always_think;
}


/*
==============================================================================

trigger_push

==============================================================================
*/

void trigger_push_touch (gentity_t *self, gentity_t *other, trace_t *trace ) {

	if ( !other->client ) {
		return;
	}
if(self->locked != 0){
return;
}
	BG_TouchJumpPad( &other->client->ps, &self->s );
}


/*
=================
AimAtTarget

Calculate origin2 so the target apogee will be hit
=================
*/
void AimAtTarget( gentity_t *self ) {
	gentity_t	*ent;
	vec3_t		origin;
	float		height, gravity, time, forward;
	float		dist;

	VectorAdd( self->r.absmin, self->r.absmax, origin );
	VectorScale ( origin, 0.5, origin );

	ent = G_PickTarget( self->target );
	if ( !ent ) {
		G_FreeEntity( self );
		return;
	}

	height = ent->s.origin[2] - origin[2];
	gravity = g_gravity.value*g_gravityModifier.value;
	time = sqrt( height / ( .5 * gravity ) );
	if ( !time ) {
		G_FreeEntity( self );
		return;
	}

	// set s.origin2 to the push velocity
	VectorSubtract ( ent->s.origin, origin, self->s.origin2 );
	self->s.origin2[2] = 0;
	dist = VectorNormalize( self->s.origin2);

	forward = dist / time;
	VectorScale( self->s.origin2, forward, self->s.origin2 );

	self->s.origin2[2] = time * gravity;
}


/*QUAKED trigger_push (.5 .5 .5) ?
Must point at a target_position, which will be the apex of the leap.
This will be client side predicted, unlike target_push
*/
void SP_trigger_push( gentity_t *self ) {
	InitTrigger (self);

	// unlike other triggers, we need to send this one to the client
	self->r.svFlags = SVF_NOCLIENT;

	// make sure the client precaches this sound
	G_SoundIndex("sound/world/jumppad.wav");

	self->s.eType = ET_PUSH_TRIGGER;
	self->touch = trigger_push_touch;
	self->think = AimAtTarget;
	self->nextthink = level.time + FRAMETIME;
	trap_LinkEntity (self);
}


void Use_target_push( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( !activator->client ) {
		return;
	}

	if ( activator->client->ps.pm_type != PM_NORMAL ) {
		return;
	}
	if ( activator->client->ps.powerups[PW_FLIGHT] ) {
		return;
	}
if(self->locked != 0){
return;
}

	VectorCopy (self->s.origin2, activator->client->ps.velocity);

	// play fly sound every 1.5 seconds
	if ( activator->fly_sound_debounce_time < level.time ) {
		activator->fly_sound_debounce_time = level.time + 1500;
		G_Sound( activator, CHAN_AUTO, self->noise_index );
	}
}

/*QUAKED target_push (.5 .5 .5) (-8 -8 -8) (8 8 8) bouncepad
Pushes the activator in the direction.of angle, or towards a target apex.
"speed"		defaults to 1000
if "bouncepad", play bounce noise instead of windfly
*/
void SP_target_push( gentity_t *self ) {
	if (!self->speed) {
		self->speed = 1000;
	}
	G_SetMovedir (self->s.angles, self->s.origin2);
	VectorScale (self->s.origin2, self->speed, self->s.origin2);

	if ( self->spawnflags & 1 ) {
		self->noise_index = G_SoundIndex("sound/world/jumppad.wav");
	} else {
		self->noise_index = G_SoundIndex("sound/misc/windfly.wav");
	}
	if ( self->target ) {
		VectorCopy( self->s.origin, self->r.absmin );
		VectorCopy( self->s.origin, self->r.absmax );
		self->think = AimAtTarget;
		self->nextthink = level.time + FRAMETIME;
	}
	self->use = Use_target_push;
}

/*
==============================================================================

trigger_teleport

==============================================================================
*/

void trigger_teleporter_touch (gentity_t *self, gentity_t *other, trace_t *trace ) {
	gentity_t	*dest;
    vec3_t		origin, angles;

	if ( !other->client ) {
		return;
	}
if(self->locked != 0){
return;
}
	if ( other->client->ps.pm_type == PM_DEAD ) {
		return;
	}
	// Spectators only?
	if ( ( self->spawnflags & 1 ) && 
		(other->client->sess.sessionTeam != TEAM_SPECTATOR && other->client->ps.pm_type != PM_SPECTATOR) ) {
		return;
	}


	dest = 	G_PickTarget( self->target );
	if (!dest) {
                G_Printf ("Couldn't find teleporter destination\n");
		return;
	}

    if ( g_randomteleport.integer ) {
        SelectSpawnPoint ( other->client->ps.origin, origin, angles );
        TeleportPlayer( other, origin, angles );
	    return;
	} else {
	TeleportPlayer( other, dest->s.origin, dest->s.angles );
	}
}


/*QUAKED trigger_teleport (.5 .5 .5) ? SPECTATOR
Allows client side prediction of teleportation events.
Must point at a target_position, which will be the teleport destination.

If spectator is set, only spectators can use this teleport
Spectator teleporters are not normally placed in the editor, but are created
automatically near doors to allow spectators to move through them
*/
void SP_trigger_teleport( gentity_t *self ) {
	InitTrigger (self);

	// unlike other triggers, we need to send this one to the client
	// unless is a spectator trigger
	if ( self->spawnflags & 1 ) {
		self->r.svFlags |= SVF_NOCLIENT;
	} else {
	self->r.svFlags = SVF_NOCLIENT;
	}

	// make sure the client precaches this sound
	G_SoundIndex("sound/world/jumppad.wav");

	self->s.eType = ET_TELEPORT_TRIGGER;
	self->touch = trigger_teleporter_touch;

	trap_LinkEntity (self);
}


/*
==============================================================================

trigger_hurt

==============================================================================
*/

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF - SILENT NO_PROTECTION SLOW
Any entity that touches this will be hurt.
It does dmg points of damage each server frame
Targeting the trigger will toggle its on / off state.

SILENT			supresses playing the sound
SLOW			changes the damage rate to once per second
NO_PROTECTION	*nothing* stops the damage

"dmg"			default 5 (whole numbers only)

*/
void hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( self->r.linked ) {
		trap_UnlinkEntity( self );
	} else {
		trap_LinkEntity( self );
	}
}

void hurt_touch( gentity_t *self, gentity_t *other, trace_t *trace ) {
	int		dflags;

	if ( !other->takedamage ) {
		return;
	}

	if ( self->timestamp > level.time ) {
		return;
	}

	if ( self->spawnflags & 16 ) {
		self->timestamp = level.time + 1000;
	} else {
		self->timestamp = level.time + FRAMETIME;
	}

	// play sound
	if ( !(self->spawnflags & 4) ) {
		G_Sound( other, CHAN_AUTO, self->noise_index );
	}

	if (self->spawnflags & 8)
		dflags = DAMAGE_NO_PROTECTION;
	else
		dflags = 0;
	G_Damage (other, self, self, NULL, NULL, self->damage, dflags, MOD_TRIGGER_HURT);
}

void SP_trigger_hurt( gentity_t *self ) {
	InitTrigger (self);

	self->noise_index = G_SoundIndex( "sound/world/electro.wav" );
	self->touch = hurt_touch;

	if ( !self->damage ) {
		self->damage = 5;
	}

	self->r.contents = CONTENTS_TRIGGER;

	self->use = hurt_use;

	// link in to the world if starting active
	if ( self->spawnflags & 1 ) {
            trap_UnlinkEntity (self);
        }
        else
        {
		trap_LinkEntity (self);
	}
}


/*
==============================================================================

timer

==============================================================================
*/


/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON START_DELAYED
This should be renamed trigger_timer...
Repeatedly fires its targets.
Can be turned on or off by using.

"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0
so, the basic time between firing is a random time between
(wait - random) and (wait + random)
START_DELAYED	When entity is turned on, the timer will activate its target after the wait period instead of immediately

*/
void func_timer_think( gentity_t *self ) {
	G_UseTargets (self, self->activator);

	// increase timer's damage value to indicate it has been used once more
	if ( self->count && self->damage < self->count )
		self->damage++;

	// set time before next firing
	if ( !self->count || self->damage < self->count )
	self->nextthink = level.time + 1000 * ( self->wait + crandom() * self->random );
	else {
		//timer has activated its targets [count] number of times, so turn it off and reset its trigger counter (damage)
		self->nextthink = 0;	
		self->damage = 0;
	}
}

void func_timer_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	self->activator = activator;

	// if on, turn it off
	if ( self->nextthink ) {
		self->nextthink = 0;
		return;
	}

	// turn it on
	if ( self->spawnflags & 2 )
		self->nextthink = level.time + 1000 * ( self->wait + crandom() * self->random );
	else
	func_timer_think (self);
}

void SP_func_timer( gentity_t *self ) {
	G_SpawnFloat( "random", "1", &self->random);
	G_SpawnFloat( "wait", "1", &self->wait );

	self->use = func_timer_use;
	self->think = func_timer_think;

	if ( self->random >= self->wait ) {
		self->random = self->wait - FRAMETIME;
                G_Printf( "func_timer at %s has random >= wait\n", vtos( self->s.origin ) );
	}

	if ( self->spawnflags & 1 ) {
		if (self->spawnflags & 2)
			self->nextthink = level.time + 1000 * (self->wait + crandom() * self->random);
		else
		self->nextthink = level.time + FRAMETIME;
		self->activator = self;
	}

	G_SpawnInt( "count", "0", &self->count);
	self->damage = 0; //damage is used to keep track of the number of times this timer has activated its targets.

	self->r.svFlags = SVF_NOCLIENT;
}


