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

#define	MISSILE_PRESTEP_TIME	50

/*
================
G_BounceMissile

================
*/
void G_BounceMissile( gentity_t *ent, trace_t *trace ) {
	vec3_t	velocity;
	float	dot;
	int		hitTime;

	// reflect the velocity on the trace plane
	hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
	BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
	dot = DotProduct( velocity, trace->plane.normal );
	VectorMA( velocity, -2*dot, trace->plane.normal, ent->s.pos.trDelta );

	if ( ent->s.eFlags & EF_BOUNCE_HALF ) {
		if(ent->classname == "plasma"){
		VectorScale( ent->s.pos.trDelta, g_pgbouncemodifier.value, ent->s.pos.trDelta );
		}
		if(ent->classname == "grenade"){
		VectorScale( ent->s.pos.trDelta, g_glbouncemodifier.value, ent->s.pos.trDelta );
		}
		if(ent->classname == "bfg"){
		VectorScale( ent->s.pos.trDelta, g_bfgbouncemodifier.value, ent->s.pos.trDelta );
		}
		if(ent->classname == "rocket"){
		VectorScale( ent->s.pos.trDelta, g_rlbouncemodifier.value, ent->s.pos.trDelta );
		}
		if(ent->classname == "nail"){
		VectorScale( ent->s.pos.trDelta, g_ngbouncemodifier.value, ent->s.pos.trDelta );
		}
		if(ent->classname == "flame"){
		VectorScale( ent->s.pos.trDelta, g_ftbouncemodifier.value, ent->s.pos.trDelta );
		}
		if(ent->classname == "antimatter"){
		VectorScale( ent->s.pos.trDelta, g_ambouncemodifier.value, ent->s.pos.trDelta );
		}
		// check for stop
		if ( trace->plane.normal[2] > 0.2 && VectorLength( ent->s.pos.trDelta ) < 40 ) {
			G_SetOrigin( ent, trace->endpos );
                        ent->s.time = level.time / 4;
			return;
		}
	}

	VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin);
	VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
	ent->s.pos.trTime = level.time;
}


/*
================
G_ExplodeMissile

Explode a missile without an impact
================
*/
void G_ExplodeMissile( gentity_t *ent ) {
	vec3_t		dir;
	vec3_t		origin;

	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
	SnapVector( origin );
	G_SetOrigin( ent, origin );

	// we don't have a valid direction, so just point straight up
	dir[0] = dir[1] = 0;
	dir[2] = 1;

	ent->s.eType = ET_GENERAL;
	G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( dir ) );

	ent->freeAfterEvent = qtrue;

	// splash damage
	if ( ent->splashDamage ) {
		if( G_RadiusDamage( ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius, ent
			, ent->splashMethodOfDeath ) ) {
			g_entities[ent->r.ownerNum].client->accuracy_hits++;
                        g_entities[ent->r.ownerNum].client->accuracy[ent->s.weapon][1]++;
		}
	}

	trap_LinkEntity( ent );
}

/*
================
ProximityMine_Explode
================
*/
static void ProximityMine_Explode( gentity_t *mine ) {
	G_ExplodeMissile( mine );
	// if the prox mine has a trigger free it
	if (mine->activator) {
		G_FreeEntity(mine->activator);
		mine->activator = NULL;
	}
}

/*
================
ProximityMine_Die
================
*/
static void ProximityMine_Die( gentity_t *ent, gentity_t *inflictor, gentity_t *attacker, int damage, int mod ) {
	ent->think = ProximityMine_Explode;
	ent->nextthink = level.time + 1;
}

/*
================
ProximityMine_Trigger
================
*/
void ProximityMine_Trigger( gentity_t *trigger, gentity_t *other, trace_t *trace ) {
	vec3_t		v;
	gentity_t	*mine;

	if( !other->client ) {
		return;
	}

	// trigger is a cube, do a distance test now to act as if it's a sphere
	VectorSubtract( trigger->s.pos.trBase, other->s.pos.trBase, v );
	if( VectorLength( v ) > trigger->parent->splashRadius ) {
		return;
	}


	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		// don't trigger same team mines
		if (trigger->parent->s.generic1 == other->client->sess.sessionTeam) {
			return;
		}
	}

	// ok, now check for ability to damage so we don't get triggered thru walls, closed doors, etc...
	if( !CanDamage( other, trigger->s.pos.trBase ) ) {
		return;
	}

	// trigger the mine!
	mine = trigger->parent;
	mine->s.loopSound = 0;
	G_AddEvent( mine, EV_PROXIMITY_MINE_TRIGGER, 0 );
	mine->nextthink = level.time + 500;

	G_FreeEntity( trigger );
}

/*
================
ProximityMine_Activate
================
*/
static void ProximityMine_Activate( gentity_t *ent ) {
	gentity_t	*trigger;
	float		r;
        vec3_t          v1;
        gentity_t       *flag;
        char            *c;
        qboolean        nearFlag = qfalse;

        // find the flag
        switch (ent->s.generic1) {
        case TEAM_RED:
                c = "team_CTF_redflag";
                break;
        case TEAM_BLUE:
                c = "team_CTF_blueflag";
                break;
        default:
            c = NULL;
        }

        if(c) {
            flag = NULL;
            while ((flag = G_Find (flag, FOFS(classname), c)) != NULL) {
                    if (!(flag->flags & FL_DROPPED_ITEM))
                            break;
            }

            if(flag) {
                VectorSubtract(ent->r.currentOrigin,flag->r.currentOrigin , v1);
                if(VectorLength(v1) < 500)
                    nearFlag = qtrue;
            }
        }

	ent->think = ProximityMine_Explode;
        if( nearFlag)
            ent->nextthink = level.time + g_proxMineTimeout.integer/15;
        else
            ent->nextthink = level.time + g_proxMineTimeout.integer;

	ent->takedamage = qtrue;
	ent->health = 1;
	ent->die = ProximityMine_Die;

	ent->s.loopSound = G_SoundIndex( "sound/weapons/proxmine/wstbtick.wav" );

	// build the proximity trigger
	trigger = G_Spawn ();

	trigger->classname = "proxmine_trigger";

	r = ent->splashRadius;
	VectorSet( trigger->r.mins, -r, -r, -r );
	VectorSet( trigger->r.maxs, r, r, r );

	G_SetOrigin( trigger, ent->s.pos.trBase );

	trigger->parent = ent;
	trigger->r.contents = CONTENTS_TRIGGER;
	trigger->touch = ProximityMine_Trigger;

	trap_LinkEntity (trigger);

	// set pointer to trigger so the entity can be freed when the mine explodes
	ent->activator = trigger;
}

/*
================
ProximityMine_ExplodeOnPlayer
================
*/
static void ProximityMine_ExplodeOnPlayer( gentity_t *mine ) {
	gentity_t	*player;

	player = mine->enemy;
	player->client->ps.eFlags &= ~EF_TICKING;

	if ( player->client->invulnerabilityTime > level.time ) {
		G_Damage( player, mine->parent, mine->parent, vec3_origin, mine->s.origin, 1000, DAMAGE_NO_KNOCKBACK, MOD_JUICED );
		player->client->invulnerabilityTime = 0;
		G_TempEntity( player->client->ps.origin, EV_JUICED );
	}
	else {
		G_SetOrigin( mine, player->s.pos.trBase );
		// make sure the explosion gets to the client
		mine->r.svFlags &= ~SVF_NOCLIENT;
		mine->splashMethodOfDeath = MOD_PROXIMITY_MINE;
		G_ExplodeMissile( mine );
	}
}

/*
================
ProximityMine_Player
================
*/
static void ProximityMine_Player( gentity_t *mine, gentity_t *player ) {
	if( mine->s.eFlags & EF_NODRAW ) {
		return;
	}

	G_AddEvent( mine, EV_PROXIMITY_MINE_STICK, 0 );

	if( player->s.eFlags & EF_TICKING ) {
		player->activator->splashDamage += mine->splashDamage;
		player->activator->splashRadius *= 1.50;
		mine->think = G_FreeEntity;
		mine->nextthink = level.time;
		return;
	}

	player->client->ps.eFlags |= EF_TICKING;
	player->activator = mine;

	mine->s.eFlags |= EF_NODRAW;
	mine->r.svFlags |= SVF_NOCLIENT;
	mine->s.pos.trType = TR_LINEAR;
	VectorClear( mine->s.pos.trDelta );

	mine->enemy = player;
	mine->think = ProximityMine_ExplodeOnPlayer;
	if ( player->client->invulnerabilityTime > level.time ) {
		mine->nextthink = level.time + 2 * 1000;
	}
	else {
		mine->nextthink = level.time + 10 * 1000;
	}
}

/*
 *=================
 *ProximityMine_RemoveAll
 *=================
 */

void ProximityMine_RemoveAll() {
    gentity_t	*mine;

    mine = NULL;

    while ((mine = G_Find (mine, FOFS(classname), "prox mine")) != NULL) {
        mine->think = ProximityMine_Explode;
	mine->nextthink = level.time + 1;
    }
}

/*
================
G_MissileImpact
================
*/
void G_MissileImpact( gentity_t *ent, trace_t *trace ) {
	gentity_t		*other;
	qboolean		hitClient = qfalse;
	vec3_t			forward, impactpoint, bouncedir;
	int				eFlags;
	other = &g_entities[trace->entityNum];

	// check for bounce
	if ( !other->takedamage &&
		( ent->s.eFlags & ( EF_BOUNCE | EF_BOUNCE_HALF ) ) ) {
		G_BounceMissile( ent, trace );
		if (ent->classname == "grenade"){
		G_AddEvent( ent, EV_GRENADE_BOUNCE, 0 );
		}
		return;
	}

	if ( other->takedamage ) {
		if ( ent->s.weapon != WP_PROX_LAUNCHER ) {
			if ( other->client && other->client->invulnerabilityTime > level.time ) {

				//
				VectorCopy( ent->s.pos.trDelta, forward );
				VectorNormalize( forward );
				if (G_InvulnerabilityEffect( other, forward, ent->s.pos.trBase, impactpoint, bouncedir )) {
					VectorCopy( bouncedir, trace->plane.normal );
					eFlags = ent->s.eFlags & EF_BOUNCE_HALF;
					ent->s.eFlags &= ~EF_BOUNCE_HALF;
					G_BounceMissile( ent, trace );
					ent->s.eFlags |= eFlags;
				}
				ent->target_ent = other;
				return;
			}
		}
	}
	// impact damage
	if (other->takedamage) {
		// FIXME: wrong damage direction?
		if ( ent->damage ) {
			vec3_t	velocity;

			if( LogAccuracyHit( other, &g_entities[ent->r.ownerNum] ) ) {
				g_entities[ent->r.ownerNum].client->accuracy_hits++;
				hitClient = qtrue;
                                g_entities[ent->r.ownerNum].client->accuracy[ent->s.weapon][1]++;
			}
			BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, velocity );
			if ( VectorLength( velocity ) == 0 ) {
				velocity[2] = 1;	// stepped on a grenade
			}
			G_Damage (other, ent, &g_entities[ent->r.ownerNum], velocity,
				ent->s.origin, ent->damage,
				0, ent->methodOfDeath);
		}
	}

	if( ent->s.weapon == WP_PROX_LAUNCHER ) {
		if( ent->s.pos.trType != TR_GRAVITY ) {
			return;
		}

		// if it's a player, stick it on to them (flag them and remove this entity)
		if( other->s.eType == ET_PLAYER && other->health > 0 ) {
			ProximityMine_Player( ent, other );
			return;
		}

		SnapVectorTowards( trace->endpos, ent->s.pos.trBase );
		G_SetOrigin( ent, trace->endpos );
		ent->s.pos.trType = TR_STATIONARY;
		VectorClear( ent->s.pos.trDelta );

		G_AddEvent( ent, EV_PROXIMITY_MINE_STICK, trace->surfaceFlags );

		ent->think = ProximityMine_Activate;
		ent->nextthink = level.time + 2000;

		vectoangles( trace->plane.normal, ent->s.angles );
		ent->s.angles[0] += 90;

		// link the prox mine to the other entity
		ent->enemy = other;
		ent->die = ProximityMine_Die;
		VectorCopy(trace->plane.normal, ent->movedir);
		VectorSet(ent->r.mins, -4, -4, -4);
		VectorSet(ent->r.maxs, 4, 4, 4);
		trap_LinkEntity(ent);

		return;
	}

	if (!strcmp(ent->classname, "hook")) {
		gentity_t *nent;
		vec3_t v;

		nent = G_Spawn();
		if ( other->takedamage && other->client ) {

			G_AddEvent( nent, EV_MISSILE_HIT, DirToByte( trace->plane.normal ) );
			nent->s.otherEntityNum = other->s.number;

			ent->enemy = other;

			v[0] = other->r.currentOrigin[0] + (other->r.mins[0] + other->r.maxs[0]) * 0.5;
			v[1] = other->r.currentOrigin[1] + (other->r.mins[1] + other->r.maxs[1]) * 0.5;
			v[2] = other->r.currentOrigin[2] + (other->r.mins[2] + other->r.maxs[2]) * 0.5;

			SnapVectorTowards( v, ent->s.pos.trBase );	// save net bandwidth
		} else {
			VectorCopy(trace->endpos, v);
			G_AddEvent( nent, EV_MISSILE_MISS, DirToByte( trace->plane.normal ) );
			ent->enemy = NULL;
		}

		SnapVectorTowards( v, ent->s.pos.trBase );	// save net bandwidth

		nent->freeAfterEvent = qtrue;
		// change over to a normal entity right at the point of impact
		nent->s.eType = ET_GENERAL;
		ent->s.eType = ET_GRAPPLE;

		G_SetOrigin( ent, v );
		G_SetOrigin( nent, v );

		ent->think = Weapon_HookThink;
		ent->nextthink = level.time + FRAMETIME;

		ent->parent->client->ps.pm_flags |= PMF_GRAPPLE_PULL;
		VectorCopy( ent->r.currentOrigin, ent->parent->client->ps.grapplePoint);

		trap_LinkEntity( ent );
		trap_LinkEntity( nent );

		return;
	}

	// is it cheaper in bandwidth to just remove this ent and create a new
	// one, rather than changing the missile into the explosion?

	if ( other->takedamage && other->client ) {
		G_AddEvent( ent, EV_MISSILE_HIT, DirToByte( trace->plane.normal ) );
		ent->s.otherEntityNum = other->s.number;
	} else if( trace->surfaceFlags & SURF_METALSTEPS ) {
		G_AddEvent( ent, EV_MISSILE_MISS_METAL, DirToByte( trace->plane.normal ) );
	} else {
		G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( trace->plane.normal ) );
	}

	ent->freeAfterEvent = qtrue;

	// change over to a normal entity right at the point of impact
	ent->s.eType = ET_GENERAL;

	SnapVectorTowards( trace->endpos, ent->s.pos.trBase );	// save net bandwidth

	G_SetOrigin( ent, trace->endpos );

	// splash damage (doesn't apply to person directly hit)
	if ( ent->splashDamage ) {
		if( G_RadiusDamage( trace->endpos, ent->parent, ent->splashDamage, ent->splashRadius,
			other, ent->splashMethodOfDeath ) ) {
			if( !hitClient ) {
				g_entities[ent->r.ownerNum].client->accuracy_hits++;
                                g_entities[ent->r.ownerNum].client->accuracy[ent->s.weapon][1]++;
			}
		}
	}

	trap_LinkEntity( ent );
}

/*
================
G_RunMissile
================
*/
void G_RunMissile( gentity_t *ent ) {
	vec3_t		origin;
	trace_t		tr;
	int			passent;

	// get current position
	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );

	// if this missile bounced off an invulnerability sphere
	if ( ent->target_ent ) {
		passent = ent->target_ent->s.number;
	}
	// prox mines that left the owner bbox will attach to anything, even the owner
	else if (ent->s.weapon == WP_PROX_LAUNCHER && ent->count) {
		passent = ENTITYNUM_NONE;
	}
	else {
		// ignore interactions with the missile owner
		passent = ent->r.ownerNum;
	}
	// trace a line from the previous position to the current position
	trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, passent, ent->clipmask );

	if ( tr.startsolid || tr.allsolid ) {
		// make sure the tr.entityNum is set to the entity we're stuck in
		trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, passent, ent->clipmask );
		tr.fraction = 0;
	}
	else {
		VectorCopy( tr.endpos, ent->r.currentOrigin );
	}

	trap_LinkEntity( ent );

	if ( tr.fraction != 1 ) {
		// never explode or bounce on sky
/*		if ( tr.surfaceFlags & SURF_NOIMPACT ) {
			// If grapple, reset owner
			if (ent->parent && ent->parent->client && ent->parent->client->hook == ent) {
				ent->parent->client->hook = NULL;
			}
			G_FreeEntity( ent );
			return;
		}*/
		G_MissileImpact( ent, &tr );
		if ( ent->s.eType != ET_MISSILE ) {
			return;		// exploded
		}
	}
	// if the prox mine wasn't yet outside the player body
	if (ent->s.weapon == WP_PROX_LAUNCHER && !ent->count) {
		// check if the prox mine is outside the owner bbox
		trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, ENTITYNUM_NONE, ent->clipmask );
		if (!tr.startsolid || tr.entityNum != ent->r.ownerNum) {
			ent->count = 1;
		}
	}
	// check think function after bouncing
	G_RunThink( ent );
}


//=============================================================================









/*
================
G_HomingMissile
================
*/

void G_HomingMissile( gentity_t *ent )
{
	gentity_t	*target = NULL;
	gentity_t	*blip = NULL;
	vec3_t      dir, blipdir, temp_dir;

	while (( blip = findradius( blip, ent->r.currentOrigin, 131072 )) != NULL ) {

		if ( blip->client == NULL )
			continue;

		if ( blip == ent->parent )
			continue;

		if ( blip->health<=0 )
			continue;

		if ( blip->client->sess.sessionTeam == TEAM_SPECTATOR )
			continue;

		if (( g_gametype.integer == GT_TEAM || g_gametype.integer == GT_CTF ) &&
			blip->client->sess.sessionTeam == ent->parent->client->sess.sessionTeam )
			continue;

		VectorSubtract( blip->r.currentOrigin, ent->r.currentOrigin, blipdir );
		blipdir[2] += 16;

		if (( target == NULL ) || ( VectorLength( blipdir ) < VectorLength( dir ) )) {

			//if new target is the nearest
			VectorCopy( blipdir, temp_dir );
			VectorNormalize( temp_dir );
			VectorAdd( temp_dir, ent->r.currentAngles, temp_dir );

			//now the longer temp_dir length is the more straight path for the rocket.
			if ( VectorLength( temp_dir ) >0.8 ) {

				//if this 1.6 were smaller,the rocket also get to target the enemy on his back.
				target = blip;
				VectorCopy( blipdir, dir );
			}
		}
	}

	if ( target == NULL ) {
		ent->nextthink = level.time + 10000;
	} else {
		ent->s.pos.trTime=level.time;
		VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );

		//for exact trajectory calculation, set current point to base.
		VectorNormalize( dir );
		VectorScale( dir, 1.8,dir );
		VectorAdd( dir, ent->r.currentAngles, dir );

		// this 0.3 is swing rate.this value is cheap,I think.try 0.8 or 1.5.
		// if you want fastest swing,comment out these 3 lines.
		VectorNormalize( dir );
		VectorCopy( dir, ent->r.currentAngles );

		//locate nozzle to target
		VectorScale ( dir, VectorLength( ent->s.pos.trDelta )*1.1, ent->s.pos.trDelta );

		//trDelta is actual vector for movement.Because the rockets slow down
		// when swing large angle,so accelalate them.
		SnapVector ( ent->s.pos.trDelta ); // save net bandwidth
		ent->nextthink = level.time + 100;	//decrease this value also makes fast swing.
	}

       if ( ent->parent->client->ps.stats[STAT_HEALTH] <= 0 || ent->parent->health <= 0 )
           G_ExplodeMissile( ent );

       if ( level.time > ent->wait ) {
           G_ExplodeMissile( ent );
	}
}





/*
================
Guided_Missile_Think
================
*/
void Guided_Missile_Think( gentity_t *missile )
{
	vec3_t forward, right, up;
	vec3_t muzzle;
	float  dist;

	gentity_t *player = missile->parent;

	// If our owner can't be found, just return
	if ( !player ) {
		G_Printf ("Guided_Missile_Think : missile has no owner!\n");
		return;
	}

	// Get our forward, right, up vector from the view angle of the player
	AngleVectors ( player->client->ps.viewangles, forward, right, up );

	// Calculate the player's eyes origin, and store this origin in muzzle
	CalcMuzzlePoint ( player, forward, right, up, muzzle );

	// Tells the engine that our movement starts at the current missile's origin
	VectorCopy ( missile->r.currentOrigin, missile->s.pos.trBase );

	// Trajectory type setup (linear move - fly)
	missile->s.pos.trType = TR_LINEAR;
	missile->s.pos.trTime = level.time - 50;

	// Get the dir vector between the player's point of view and the rocket
	// and store it into muzzle again
	VectorSubtract( muzzle, missile->r.currentOrigin, muzzle );

	// Add some range to our "line" so we can go behind blocks
	// We could have used a trace function here, but the rocket would
	// have come back if player was aiming on a block while the rocket is behind it
	// as the trace stops on solid blocks

//	dist = VectorLength( muzzle ) + 400;	 //give the range of our muzzle vector + 400 units
//	VectorScale( forward, dist, forward );
		if(missile->classname == "plasma"){
     VectorScale( forward, g_pgspeed.integer, forward );
		}
		if(missile->classname == "grenade"){
     VectorScale( forward, g_glspeed.integer, forward );
		}
		if(missile->classname == "bfg"){
     VectorScale( forward, g_bfgspeed.integer, forward );
		}
		if(missile->classname == "rocket"){
     VectorScale( forward, g_rlspeed.integer, forward );
		}
		if(missile->classname == "nail"){
     VectorScale( forward, g_ngspeed.integer, forward );
		}
		if(missile->classname == "flame"){
     VectorScale( forward, g_ftspeed.integer, forward );
		}
		if(missile->classname == "antimatter"){
     VectorScale( forward, g_amspeed.integer, forward );
		}

	// line straight forward
	VectorAdd( forward, muzzle, muzzle );

	// Normalize the vector so it's 1 unit long, but keep its direction
	VectorNormalize( muzzle );

	// Slow the rocket down a bit, so we can handle it
    // VectorScale( muzzle, 300, forward );

	// Set the rockets's velocity so it'll move toward our new direction
	VectorCopy( forward, missile->s.pos.trDelta );

	// Change the rocket's angle so it'll point toward the new direction
	vectoangles( muzzle, missile->s.angles );

	// This should "save net bandwidth" =D
	SnapVector( missile->s.pos.trDelta );


    missile->nextthink = level.time + 25;

    if ( level.time > missile->wait ) {
        G_ExplodeMissile( missile );
	}
}










/*
=================
fire_plasma

=================
*/
gentity_t *fire_plasma (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "plasma";
if(g_pghoming.integer == 0){
	bolt->nextthink = level.time + g_pgtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_pghoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_pgtimeout.integer;
}
if(g_pghoming.integer == 0){
if(g_pgguided.integer == 0){
	bolt->nextthink = level.time + g_pgtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_pgguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_pgtimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_pgbounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_pgbounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_PLASMAGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_pgdamage.value;
	bolt->splashDamage = g_pgsdamage.value;
	bolt->splashRadius = g_pgsradius.value;
	bolt->methodOfDeath = MOD_PLASMA;
	bolt->splashMethodOfDeath = MOD_PLASMA_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	if (g_pggravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_pgspeed.value, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}


gentity_t *fire_plasma2 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "plasma";
if(g_pghoming.integer == 0){
	bolt->nextthink = level.time + g_pgtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_pghoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_pgtimeout.integer;
}
if(g_pghoming.integer == 0){
if(g_pgguided.integer == 0){
	bolt->nextthink = level.time + g_pgtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_pgguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_pgtimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_pgbounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_pgbounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_PLASMAGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_pgdamage.value;
	bolt->splashDamage = g_pgsdamage.value;
	bolt->splashRadius = g_pgsradius.value;
	bolt->methodOfDeath = MOD_PLASMA;
	bolt->splashMethodOfDeath = MOD_PLASMA_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	if (g_pggravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_pgspeed.value*2, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

gentity_t *fire_plasma3 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "plasma";
	bolt->nextthink = level.time + g_pgtimeout.integer;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_PLASMAGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_pgdamage.value*4;
	bolt->splashDamage = g_pgsdamage.value*4;
	bolt->splashRadius = g_pgsradius.value*1.2;
	bolt->methodOfDeath = MOD_PLASMA;
	bolt->splashMethodOfDeath = MOD_PLASMA_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_pgspeed.value*1.5, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

gentity_t *fire_plasma4 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "plasma";
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_pgtimeout.integer;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_PLASMAGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_pgdamage.value;
	bolt->splashDamage = g_pgsdamage.value;
	bolt->splashRadius = g_pgsradius.value;
	bolt->methodOfDeath = MOD_PLASMA;
	bolt->splashMethodOfDeath = MOD_PLASMA_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, 62, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

//=============================================================================


/*
=================
fire_grenade
=================
*/
gentity_t *fire_grenade (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "grenade";
if(g_glhoming.integer == 0){
	bolt->nextthink = level.time + g_gltimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_glhoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_gltimeout.integer;
}
if(g_glhoming.integer == 0){
if(g_glguided.integer == 0){
	bolt->nextthink = level.time + g_gltimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_glguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_gltimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_glbounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_glbounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_GRENADE_LAUNCHER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_gldamage.value;
	bolt->splashDamage = g_glsdamage.value;
	bolt->splashRadius = g_glsradius.value;
	bolt->methodOfDeath = MOD_GRENADE;
	bolt->splashMethodOfDeath = MOD_GRENADE_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	if (g_glgravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_glspeed.value, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}



gentity_t *fire_grenade2 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "grenade";
	bolt->nextthink = level.time + g_gltimeout.integer * 10;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.eFlags = EF_BOUNCE;
//	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_GRENADE_LAUNCHER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_gldamage.value;
	bolt->splashDamage = g_glsdamage.value;
	bolt->splashRadius = g_glsradius.value;
	bolt->methodOfDeath = MOD_GRENADE;
	bolt->splashMethodOfDeath = MOD_GRENADE_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_glspeed.value*1.3, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}


gentity_t *fire_grenade3 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "grenade";
	bolt->nextthink = level.time + g_gltimeout.integer*1.5;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.eFlags = EF_BOUNCE_HALF;
//	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_GRENADE_LAUNCHER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_gldamage.value*1.5;
	bolt->splashDamage = g_glsdamage.value*1.5;
	bolt->splashRadius = g_glsradius.value*3;
	bolt->methodOfDeath = MOD_GRENADE;
	bolt->splashMethodOfDeath = MOD_GRENADE_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_glspeed.value*2, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}


gentity_t *fire_grenade4 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "grenade";
	bolt->nextthink = level.time + g_gltimeout.integer*1.5;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_GRENADE_LAUNCHER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_gldamage.value*1.2;
	bolt->splashDamage = g_glsdamage.value*1.2;
	bolt->splashRadius = g_glsradius.value*1.2;
	bolt->methodOfDeath = MOD_GRENADE;
	bolt->splashMethodOfDeath = MOD_GRENADE_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_glspeed.value*1.5, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

//=============================================================================


/*
=================
fire_bfg
=================
*/
gentity_t *fire_bfg (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "bfg";
if(g_bfghoming.integer == 0){
	bolt->nextthink = level.time + g_bfgtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_bfghoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_bfgtimeout.integer;
}
if(g_bfghoming.integer == 0){
if(g_bfgguided.integer == 0){
	bolt->nextthink = level.time + g_bfgtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_bfgguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_bfgtimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_bfgbounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_bfgbounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_BFG;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_bfgdamage.value;
	bolt->splashDamage = g_bfgsdamage.value;
	bolt->splashRadius = g_bfgsradius.value;
	bolt->methodOfDeath = MOD_BFG;
	bolt->splashMethodOfDeath = MOD_BFG_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	if (g_bfggravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_bfgspeed.value, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

gentity_t *fire_bfg4 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "bfg";
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + 100000;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_BFG;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_bfgdamage.value;
	bolt->splashDamage = g_bfgsdamage.value;
	bolt->splashRadius = g_bfgsradius.value;
	bolt->methodOfDeath = MOD_BFG;
	bolt->splashMethodOfDeath = MOD_BFG_SPLASH;
	bolt->clipmask = MASK_NOCSHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, 13, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

//=============================================================================


/*
=================
fire_rocket
=================
*/
gentity_t *fire_rocket (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "rocket";
if(g_rlhoming.integer == 0){
	bolt->nextthink = level.time + g_rltimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_rlhoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_rltimeout.integer;
}
if(g_rlhoming.integer == 0){
if(g_rlguided.integer == 0){
	bolt->nextthink = level.time + g_rltimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_rlguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_rltimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_rlbounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_rlbounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_ROCKET_LAUNCHER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_rldamage.value;
	bolt->splashDamage = g_rlsdamage.value;
	bolt->splashRadius = g_rlsradius.value;
	bolt->methodOfDeath = MOD_ROCKET;
	bolt->splashMethodOfDeath = MOD_ROCKET_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	if (g_rlgravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_rlspeed.value, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

gentity_t *fire_rocket2 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "rocket";
	bolt->nextthink = level.time + g_rltimeout.integer;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_ROCKET_LAUNCHER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_rldamage.value*1.2;
	bolt->splashDamage = g_rlsdamage.value*1.2;
	bolt->splashRadius = g_rlsradius.value*1.2;
	bolt->methodOfDeath = MOD_ROCKET;
	bolt->splashMethodOfDeath = MOD_ROCKET_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_rlspeed.value*3, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}


gentity_t *fire_rocket3 (gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "rocket";
	bolt->nextthink = level.time + g_rltimeout.integer;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_ROCKET_LAUNCHER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_rldamage.value;
	bolt->splashDamage = g_rlsdamage.value;
	bolt->splashRadius = g_rlsradius.value;
	bolt->methodOfDeath = MOD_ROCKET;
	bolt->splashMethodOfDeath = MOD_ROCKET_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * 650 * 16;
	r = cos(r) * crandom() * 650 * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );
	
	scale = g_rlspeed.integer * 1.3;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}

gentity_t *fire_rocket4 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "rocket";
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + 30000;
	bolt->s.eType = ET_MISSILE;
	bolt->s.eFlags = EF_BOUNCE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_ROCKET_LAUNCHER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_rldamage.value;
	bolt->splashDamage = g_rlsdamage.value;
	bolt->splashRadius = g_rlsradius.value;
	bolt->methodOfDeath = MOD_ROCKET;
	bolt->splashMethodOfDeath = MOD_ROCKET_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, 50, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

/*
=================
fire_grapple
=================
*/
gentity_t *fire_grapple (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*hook;
//unlagged - grapple
	int hooktime;
//unlagged - grapple

	VectorNormalize (dir);

	hook = G_Spawn();
	hook->classname = "hook";
	hook->nextthink = level.time + g_ghtimeout.integer;
	hook->think = Weapon_HookFree;
	hook->s.eType = ET_MISSILE;
	hook->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	hook->s.weapon = WP_GRAPPLING_HOOK;
	hook->r.ownerNum = self->s.number;
	hook->methodOfDeath = MOD_GRAPPLE;
	hook->clipmask = MASK_SHOT;
	hook->parent = self;
	hook->target_ent = NULL;

//unlagged - grapple
	// we might want this later
	hook->s.otherEntityNum = self->s.number;

	// setting the projectile base time back makes the hook's first
	// step larger

	if ( self->client ) {
		hooktime = self->client->pers.cmd.serverTime + 50;
	}
	else {
		hooktime = level.time - MISSILE_PRESTEP_TIME;
	}

	hook->s.pos.trTime = hooktime;
//unlagged - grapple

	hook->s.pos.trType = TR_LINEAR;
//unlagged - grapple
	//hook->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
//unlagged - grapple
	hook->s.otherEntityNum = self->s.number; // use to match beam in client
	VectorCopy( start, hook->s.pos.trBase );
	VectorScale( dir, g_ghspeed.value, hook->s.pos.trDelta );
	
	SnapVector( hook->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, hook->r.currentOrigin);

	self->client->hook = hook;

	return hook;
}

/*
=================
fire_grapple
=================
*/
gentity_t *fire_grapple4 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*hook;
//unlagged - grapple
	int hooktime;
//unlagged - grapple

	VectorNormalize (dir);

	hook = G_Spawn();
	hook->classname = "hook";
	hook->nextthink = level.time + g_ghtimeout.integer;
	hook->think = Weapon_HookFree;
	hook->s.eType = ET_MISSILE;
	hook->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	hook->s.weapon = WP_GRAPPLING_HOOK;
	hook->r.ownerNum = self->s.number;
	hook->methodOfDeath = MOD_GRAPPLE;
	hook->clipmask = MASK_SHOT;
	hook->parent = self;
	hook->damage = 100;
	hook->target_ent = NULL;

//unlagged - grapple
	// we might want this later
	hook->s.otherEntityNum = self->s.number;

	// setting the projectile base time back makes the hook's first
	// step larger

	if ( self->client ) {
		hooktime = self->client->pers.cmd.serverTime + 50;
	}
	else {
		hooktime = level.time - MISSILE_PRESTEP_TIME;
	}

	hook->s.pos.trTime = hooktime;
//unlagged - grapple

	hook->s.pos.trType = TR_LINEAR;
//unlagged - grapple
	//hook->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
//unlagged - grapple
	hook->s.otherEntityNum = self->s.number; // use to match beam in client
	VectorCopy( start, hook->s.pos.trBase );
	VectorScale( dir, g_ghspeed.value*2, hook->s.pos.trDelta );	
	
	SnapVector( hook->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, hook->r.currentOrigin);

	self->client->hook = hook;

	return hook;
}

/*
=================
fire_nail
=================
*/
#define NAILGUN_SPREAD	g_ngspread.integer

gentity_t *fire_nail( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
if(g_nghoming.integer == 0){
	bolt->nextthink = level.time + g_ngtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_nghoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_ngtimeout.integer;
}
if(g_nghoming.integer == 0){
if(g_ngguided.integer == 0){
	bolt->nextthink = level.time + g_ngtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_ngguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_ngtimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_ngbounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_ngbounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_ngdamage.value;
	bolt->methodOfDeath = MOD_NAIL;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	if (g_nggravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * NAILGUN_SPREAD * 16;
	r = cos(r) * crandom() * NAILGUN_SPREAD * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = g_ngspeed.integer + random() * g_ngrandom.integer;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}

gentity_t *fire_nails( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
if(g_nghoming.integer == 0){
	bolt->nextthink = level.time + g_ngtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_nghoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_ngtimeout.integer;
}
if(g_nghoming.integer == 0){
if(g_ngguided.integer == 0){
	bolt->nextthink = level.time + g_ngtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_ngguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_ngtimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_ngbounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_ngbounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_ngdamage.value;
	bolt->methodOfDeath = MOD_NAIL;
	bolt->clipmask = MASK_NOCSHOT;
	bolt->target_ent = NULL;
	if (g_nggravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * NAILGUN_SPREAD * 16;
	r = cos(r) * crandom() * NAILGUN_SPREAD * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = g_ngspeed.integer + random() * g_ngrandom.integer;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}

gentity_t *fire_nail2( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + 200;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = 60;
	bolt->methodOfDeath = MOD_NAIL;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * 100 * 16;
	r = cos(r) * crandom() * 100 * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = 3000;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}


gentity_t *fire_nail3( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + g_ngtimeout.integer;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.eFlags = EF_BOUNCE_HALF;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_ngdamage.value;
	bolt->methodOfDeath = MOD_NAIL;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * NAILGUN_SPREAD * 16;
	r = cos(r) * crandom() * NAILGUN_SPREAD * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = g_ngspeed.integer * 2 + random() * g_ngrandom.integer;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}


gentity_t *fire_nail4( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + g_ngtimeout.integer;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_ngdamage.value;
	bolt->methodOfDeath = MOD_NAIL;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * NAILGUN_SPREAD * 16;
	r = cos(r) * crandom() * NAILGUN_SPREAD * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = g_ngspeed.integer*4;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}






/*
=================
fire_grapple2
=================
*/

gentity_t *fire_grapple2( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + 15000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_sgdamage.value;
	bolt->methodOfDeath = MOD_SHOTGUN;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * g_sgspread.integer * 16;
	r = cos(r) * crandom() * g_sgspread.integer * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = 999999;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}


gentity_t *fire_shotgun2( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + 15000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_sgdamage.value;
	bolt->methodOfDeath = MOD_SHOTGUN;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * g_sgspread.integer * 0.70 * 16;
	r = cos(r) * crandom() * g_sgspread.integer * 0.70 * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = 999999;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}

gentity_t *fire_shotgun3( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + 15000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.eFlags = EF_BOUNCE;
//	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_sgdamage.value;
	bolt->methodOfDeath = MOD_SHOTGUN;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * g_sgspread.integer * 16;
	r = cos(r) * crandom() * g_sgspread.integer * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = 5000;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}

gentity_t *fire_shotgun4( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + 15000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_sgdamage.value;
	bolt->methodOfDeath = MOD_SHOTGUN;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * g_sgspread.integer * 2 * 16;
	r = cos(r) * crandom() * g_sgspread.integer * 2 * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = 7000;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}


/*
=================
fire_grapple3
=================
*/

gentity_t *fire_grapple3( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + 15000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_sgdamage.value*3;
	bolt->methodOfDeath = MOD_SHOTGUN;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * g_sgspread.integer * 0.2 * 16;
	r = cos(r) * crandom() * g_sgspread.integer * 0.2 * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = 999999;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}

/*
=================
fire_grapple3
=================
*/

gentity_t *fire_machine3( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up ) {
	gentity_t	*bolt;
	vec3_t		dir;
	vec3_t		end;
	float		r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + 15000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_mgdamage.value*8;
	bolt->methodOfDeath = MOD_MACHINEGUN;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( start, bolt->s.pos.trBase );

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * g_sgspread.integer * 0.1 * 16;
	r = cos(r) * crandom() * g_sgspread.integer * 0.1 * 16;
	VectorMA( start, 8192 * 16, forward, end);
	VectorMA (end, r, right, end);
	VectorMA (end, u, up, end);
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	scale = 999999;
	VectorScale( dir, scale, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );

	VectorCopy( start, bolt->r.currentOrigin );

	return bolt;
}


/*
=================
fire_prox
=================
*/
gentity_t *fire_prox( gentity_t *self, vec3_t start, vec3_t dir ) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "prox mine";
	bolt->nextthink = level.time + g_pltimeout.value;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_PROX_LAUNCHER;
	bolt->s.eFlags = 0;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_pldamage.value;
	bolt->splashDamage = g_plsdamage.value;
	bolt->splashRadius = g_plsradius.value;
	bolt->methodOfDeath = MOD_PROXIMITY_MINE;
	bolt->splashMethodOfDeath = MOD_PROXIMITY_MINE;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	// count is used to check if the prox mine left the player bbox
	// if count == 1 then the prox mine left the player bbox and can attack to it
	bolt->count = 0;

	//FIXME: we prolly wanna abuse another field
	bolt->s.generic1 = self->client->sess.sessionTeam;
	if (g_plgravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_plspeed.value, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

/*
=================
fire_flamethrower
=================
*/
gentity_t *fire_flame (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "flame";
if(g_fthoming.integer == 0){
	bolt->nextthink = level.time + g_fttimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_fthoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_fttimeout.integer;
}
if(g_fthoming.integer == 0){
if(g_ftguided.integer == 0){
	bolt->nextthink = level.time + g_fttimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_ftguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_fttimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_ftbounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_ftbounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_FLAMETHROWER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_ftdamage.value;
	bolt->splashDamage = g_ftsdamage.value;
	bolt->splashRadius = g_ftsradius.value;
	bolt->methodOfDeath = MOD_FLAME;
	bolt->splashMethodOfDeath = MOD_FLAME_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	if (g_ftgravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_ftspeed.value, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}


gentity_t *fire_flame2 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "flame";
	bolt->nextthink = level.time + g_fttimeout.integer;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.eFlags = EF_BOUNCE_HALF;
//	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_FLAMETHROWER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_ftdamage.value;
	bolt->splashDamage = g_ftsdamage.value;
	bolt->splashRadius = g_ftsradius.value;
	bolt->methodOfDeath = MOD_FLAME;
	bolt->splashMethodOfDeath = MOD_FLAME_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_ftspeed.value*2.5, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}


gentity_t *fire_flame3 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "flame";
	bolt->nextthink = level.time + g_fttimeout.integer;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_FLAMETHROWER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_ftdamage.value;
	bolt->splashDamage = g_ftsdamage.value;
	bolt->splashRadius = g_ftsradius.value;
	bolt->methodOfDeath = MOD_FLAME;
	bolt->splashMethodOfDeath = MOD_FLAME_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_ftspeed.value*3, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

gentity_t *fire_flame4 (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "flame";
	bolt->nextthink = level.time + g_fttimeout.integer;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_FLAMETHROWER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_ftdamage.value;
	bolt->splashDamage = g_ftsdamage.value;
	bolt->splashRadius = g_ftsradius.value;
	bolt->methodOfDeath = MOD_FLAME;
	bolt->splashMethodOfDeath = MOD_FLAME_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_ftspeed.value*8, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}


/*
=================
fire_antimatter
=================
*/
gentity_t *fire_antimatter (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "antimatter";
if(g_amhoming.integer == 0){
	bolt->nextthink = level.time + g_amtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_amhoming.integer == 1){
	bolt->nextthink = level.time + 10;
	bolt->think = G_HomingMissile;
    bolt->wait = level.time + g_amtimeout.integer;
}
if(g_amhoming.integer == 0){
if(g_amguided.integer == 0){
	bolt->nextthink = level.time + g_amtimeout.integer;
	bolt->think = G_ExplodeMissile;
}
if(g_amguided.integer == 1){
	bolt->nextthink = level.time + 50;
	bolt->think = Guided_Missile_Think;
    bolt->wait = level.time + g_amtimeout.integer;
}
}
	bolt->s.eType = ET_MISSILE;
	if (g_ambounce.integer == 1)
	bolt->s.eFlags = EF_BOUNCE_HALF;
	else
	if (g_ambounce.integer == 2)
	bolt->s.eFlags = EF_BOUNCE;
	else
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_ANTIMATTER;
	bolt->r.ownerNum = self->s.number;
//unlagged - projectile nudge
	// we'll need this for nudging projectiles later
	bolt->s.otherEntityNum = self->s.number;
//unlagged - projectile nudge
	bolt->parent = self;
	bolt->damage = g_amdamage.value;
	bolt->splashDamage = g_amsdamage.value;
	bolt->splashRadius = g_amsradius.value;
	bolt->methodOfDeath = MOD_ANTIMATTER;
	bolt->splashMethodOfDeath = MOD_ANTIMATTER_SPLASH;
	bolt->clipmask = MASK_SHOT;
	bolt->target_ent = NULL;
	if (g_amgravity.integer == 1)
	bolt->s.pos.trType = TR_GRAVITY;
	else
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, g_amspeed.value, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}
