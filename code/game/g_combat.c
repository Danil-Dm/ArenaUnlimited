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
// g_combat.c

#include "g_local.h"
#include "challenges.h"

/*
============
ScorePlum
============
*/
void ScorePlum( gentity_t *ent, vec3_t origin, int score ) {
	gentity_t *plum;

	plum = G_TempEntity( origin, EV_SCOREPLUM );
	// only send this temp entity to a single client
	plum->r.svFlags |= SVF_SINGLECLIENT;
	plum->r.singleClient = ent->s.number;
	//
	plum->s.otherEntityNum = ent->s.number;
	plum->s.time = score;
}

/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, vec3_t origin, int score ) {
        int i;

	if ( !ent->client ) {
		return;
	}
	// no scoring during pre-match warmup
	if ( level.warmupTime ) {
		return;
	}

        //No scoring during intermission
        if ( level.intermissiontime ) {
            return;
        }
	// show score plum
        if( level.numNonSpectatorClients<3 && score < 0 && (g_gametype.integer<GT_TEAM || g_ffa_gt==1)) {
            for ( i = 0 ; i < level.maxclients ; i++ ) {
                if ( level.clients[ i ].pers.connected != CON_CONNECTED )
                    continue; //Client was not connected

                if (level.clients[i].sess.sessionTeam == TEAM_SPECTATOR)
                    continue; //Don't give anything to spectators

                if (g_entities+i == ent)
                    continue; //Don't award dead one

                level.clients[i].ps.persistant[PERS_SCORE] -= score;
                ScorePlum(ent, origin, -score);
            }
        }
        else {
            ScorePlum(ent, origin, score);
            //
            ent->client->ps.persistant[PERS_SCORE] += score;
            if ( g_gametype.integer == GT_TEAM ) {
                int team = ent->client->ps.persistant[PERS_TEAM];
                    level.teamScores[ team ] += score;
                    G_LogPrintf("TeamScore: %i %i: Team %d now has %d points\n",
                        team, level.teamScores[ team ], team, level.teamScores[ team ] );
            }
        }
        G_LogPrintf("PlayerScore: %i %i: %s now has %d points\n",
		ent->s.number, ent->client->ps.persistant[PERS_SCORE], ent->client->pers.netname, ent->client->ps.persistant[PERS_SCORE] );
	CalculateRanks();
}

/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
void TossClientItems( gentity_t *self ) {
	gitem_t		*item;
	int			weapon;
	float		angle;
	int			i;
	gentity_t	*drop;

	// drop the weapon if not a gauntlet or machinegun
	weapon = self->s.weapon;

	//Never drop in elimination or last man standing mode!
	if( g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_LMS)
		return;

	// make a special check to see if they are changing to a new
	// weapon that isn't the mg or gauntlet.  Without this, a client
	// can pick up a weapon, be killed, and not drop the weapon because
	// their weapon change hasn't completed yet and they are still holding the MG.
	if ( weapon == WP_MACHINEGUN || weapon == WP_GRAPPLING_HOOK ) {
		if ( self->client->ps.weaponstate == WEAPON_DROPPING ) {
			weapon = self->client->pers.cmd.weapon;
		}
		if ( !( self->client->ps.stats[STAT_WEAPONS] & ( 1 << weapon ) ) ) {
			weapon = WP_NONE;
		}
	}

	if (g_instantgib.integer || g_rockets.integer || g_gametype.integer == GT_CTF_ELIMINATION || g_elimination_allgametypes.integer){
	//Nothing!
	}
	else
	if ( weapon > WP_MACHINEGUN && weapon != WP_GRAPPLING_HOOK &&
		self->client->ps.ammo[ weapon ] ) {
		// find the item type for this weapon
		item = BG_FindItemForWeapon( weapon );

		// spawn the item
		Drop_Item( self, item, 0 );
	}

	// drop all the powerups if not in teamplay

		angle = 45;
		for ( i = 1 ; i < PW_NUM_POWERUPS ; i++ ) {
			if ( self->client->ps.powerups[ i ] > level.time ) {
				item = BG_FindItemForPowerup( i );
				if ( !item ) {
					continue;
				}
				drop = Drop_Item( self, item, angle );
				// decide how many seconds it has left
				drop->count = ( self->client->ps.powerups[ i ] - level.time ) / 1000;
				if ( drop->count < 1 ) {
					drop->count = 1;
				}
				angle += 45;
			}
		}
	if(self->client->ps.stats[STAT_HOLDABLE_ITEM] == BG_FindItem( "Personal Teleporter" ) - bg_itemlist) {
	item = BG_FindItem( "Personal Teleporter" );
	Drop_Item( self, item, 0 );
	}
	if(self->client->ps.stats[STAT_HOLDABLE_ITEM] == BG_FindItem( "Medkit" ) - bg_itemlist) {
	item = BG_FindItem( "Medkit" );
	Drop_Item( self, item, 0 );
	}
	if(self->client->ps.stats[STAT_HOLDABLE_ITEM] == BG_FindItem( "Kamikaze" ) - bg_itemlist) {
	item = BG_FindItem( "Kamikaze" );
	Drop_Item( self, item, 0 );
	}
	if(self->client->ps.stats[STAT_HOLDABLE_ITEM] == BG_FindItem( "Invulnerability" ) - bg_itemlist) {
	item = BG_FindItem( "Invulnerability" );
	Drop_Item( self, item, 0 );
	}
	if(self->client->ps.stats[STAT_HOLDABLE_ITEM] == BG_FindItem( "Portal" ) - bg_itemlist) {
	item = BG_FindItem( "Portal" );
	Drop_Item( self, item, 0 );
	}

}

/*
=================
TossClientCubes
=================
*/
extern gentity_t	*neutralObelisk;

void TossClientCubes( gentity_t *self ) {
	gitem_t		*item;
	gentity_t	*drop;
	vec3_t		velocity;
	vec3_t		angles;
	vec3_t		origin;

	self->client->ps.generic1 = 0;

	// this should never happen but we should never
	// get the server to crash due to skull being spawned in
	if (!G_EntitiesFree()) {
		return;
	}

	if( self->client->sess.sessionTeam == TEAM_RED ) {
		item = BG_FindItem( "Red Cube" );
	}
	else {
		item = BG_FindItem( "Blue Cube" );
	}

	angles[YAW] = (float)(level.time % 360);
	angles[PITCH] = 0;	// always forward
	angles[ROLL] = 0;

	AngleVectors( angles, velocity, NULL, NULL );
	VectorScale( velocity, 150, velocity );
	velocity[2] += 200 + crandom() * 50;

	if( neutralObelisk ) {
		VectorCopy( neutralObelisk->s.pos.trBase, origin );
		origin[2] += 44;
	} else {
		VectorClear( origin ) ;
	}

	drop = LaunchItem( item, origin, velocity );

	drop->nextthink = level.time + g_cubeTimeout.integer * 1000;
	drop->think = G_FreeEntity;
	drop->spawnflags = self->client->sess.sessionTeam;
}

/*
=================
TossClientPersistantPowerups
=================
*/
void TossClientPersistantPowerups( gentity_t *ent ) {
	gentity_t	*powerup;

	if( !ent->client ) {
		return;
	}

	if( !ent->client->persistantPowerup ) {
		return;
	}

	powerup = ent->client->persistantPowerup;

	powerup->r.svFlags &= ~SVF_NOCLIENT;
	powerup->s.eFlags &= ~EF_NODRAW;
	powerup->r.contents = CONTENTS_TRIGGER;
	trap_LinkEntity( powerup );

	ent->client->ps.stats[STAT_PERSISTANT_POWERUP] = 0;
	ent->client->persistantPowerup = NULL;
}


/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker ) {
	vec3_t		dir;
	//vec3_t		angles;

	if ( attacker && attacker != self ) {
		VectorSubtract (attacker->s.pos.trBase, self->s.pos.trBase, dir);
	} else if ( inflictor && inflictor != self ) {
		VectorSubtract (inflictor->s.pos.trBase, self->s.pos.trBase, dir);
	} else {
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	}

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );

	/*angles[YAW] =*/ vectoyaw ( dir );
	//angles[PITCH] = 0;
	//angles[ROLL] = 0;
}

/*
==================
GibEntity
==================
*/
void GibEntity( gentity_t *self, int killer ) {
	gentity_t *ent;
	int i;

	//if this entity still has kamikaze
	if (self->s.eFlags & EF_KAMIKAZE) {
		// check if there is a kamikaze timer around for this owner
		for (i = 0; i < MAX_GENTITIES; i++) {
			ent = &g_entities[i];
			if (!ent->inuse)
				continue;
			if (ent->activator != self)
				continue;
			if (strcmp(ent->classname, "kamikaze timer"))
				continue;
			G_FreeEntity(ent);
			break;
		}
	}
	G_AddEvent( self, EV_GIB_PLAYER, killer );
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
}

/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	if ( self->health > GIB_HEALTH ) {
		return;
	}
	if ( !g_blood.integer ) {
		self->health = GIB_HEALTH+1;
		return;
	}

	GibEntity( self, 0 );
}


// these are just for logging, the client prints its own messages
char	*modNames[] = {
	"MOD_UNKNOWN",
	"MOD_SHOTGUN",
	"MOD_GAUNTLET",
	"MOD_MACHINEGUN",
	"MOD_GRENADE",
	"MOD_GRENADE_SPLASH",
	"MOD_ROCKET",
	"MOD_ROCKET_SPLASH",
	"MOD_PLASMA",
	"MOD_PLASMA_SPLASH",
	"MOD_RAILGUN",
	"MOD_LIGHTNING",
	"MOD_BFG",
	"MOD_BFG_SPLASH",
	"MOD_FLAME",
	"MOD_FLAME_SPLASH",
	"MOD_ANTIMATTER",
	"MOD_ANTIMATTER_SPLASH",
	"MOD_WATER",
	"MOD_SLIME",
	"MOD_LAVA",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TARGET_LASER",
	"MOD_TRIGGER_HURT",
	"MOD_NAIL",
	"MOD_CHAINGUN",
	"MOD_PROXIMITY_MINE",
	"MOD_KAMIKAZE",
	"MOD_JUICED",
	"MOD_GRAPPLE"
};

/*
==================
Kamikaze_DeathActivate
==================
*/
void Kamikaze_DeathActivate( gentity_t *ent ) {
	G_StartKamikaze(ent);
	G_FreeEntity(ent);
}

/*
==================
Kamikaze_DeathTimer
==================
*/
void Kamikaze_DeathTimer( gentity_t *self ) {
	gentity_t *ent;

	ent = G_Spawn();
	ent->classname = "kamikaze timer";
	VectorCopy(self->s.pos.trBase, ent->s.pos.trBase);
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->think = Kamikaze_DeathActivate;
	ent->nextthink = level.time + 5 * 1000;

	ent->activator = self;
}


/*
==================
CheckAlmostCapture
==================
*/
void CheckAlmostCapture( gentity_t *self, gentity_t *attacker ) {
	gentity_t	*ent;
	vec3_t		dir;
	char		*classname;

	// if this player was carrying a flag
	if ( self->client->ps.powerups[PW_REDFLAG] ||
		self->client->ps.powerups[PW_BLUEFLAG] ||
		self->client->ps.powerups[PW_NEUTRALFLAG] ) {
		// get the goal flag this player should have been going for
		if ( g_gametype.integer == GT_CTF || g_gametype.integer == GT_CTF_ELIMINATION) {
			if ( self->client->sess.sessionTeam == TEAM_BLUE ) {
				classname = "team_CTF_blueflag";
			}
			else {
				classname = "team_CTF_redflag";
			}
		}
		else {
			if ( self->client->sess.sessionTeam == TEAM_BLUE ) {
				classname = "team_CTF_redflag";
			}
			else {
				classname = "team_CTF_blueflag";
			}
		}
		ent = NULL;
		do
		{
			ent = G_Find(ent, FOFS(classname), classname);
		} while (ent && (ent->flags & FL_DROPPED_ITEM));
		// if we found the destination flag and it's not picked up
		if (ent && !(ent->r.svFlags & SVF_NOCLIENT) ) {
			// if the player was *very* close
			VectorSubtract( self->client->ps.origin, ent->s.origin, dir );
			if ( VectorLength(dir) < 200 ) {
				self->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_HOLYSHIT;
				if ( attacker->client ) {
					attacker->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_HOLYSHIT;
				}
			}
		}
	}
}

/*
==================
CheckAlmostScored
==================
*/
void CheckAlmostScored( gentity_t *self, gentity_t *attacker ) {
	gentity_t	*ent;
	vec3_t		dir;
	char		*classname;

	// if the player was carrying cubes
	if ( self->client->ps.generic1 ) {
		if ( self->client->sess.sessionTeam == TEAM_BLUE ) {
			classname = "team_redobelisk";
		}
		else {
			classname = "team_blueobelisk";
		}
		ent = G_Find(NULL, FOFS(classname), classname);
		// if we found the destination obelisk
		if ( ent ) {
			// if the player was *very* close
			VectorSubtract( self->client->ps.origin, ent->s.origin, dir );
			if ( VectorLength(dir) < 200 ) {
				self->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_HOLYSHIT;
				if ( attacker->client ) {
					attacker->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_HOLYSHIT;
				}
			}
		}
	}
}

/*
==================
player_die
==================
*/
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	gentity_t	*ent;
	int			anim;
	int			contents;
	int			killer;
	int			i,counter2;
	char		*killerName, *obit;

	if ( self->client->ps.pm_type == PM_DEAD ) {
		return;
	}

	if ( level.intermissiontime ) {
		return;
	}
	
			
	self->client->pers.oldmoney = self->client->pers.oldmoney;

//unlagged - backward reconciliation #2
	// make sure the body shows up in the client's current position
	G_UnTimeShiftClient( self );
//unlagged - backward reconciliation #2
    //KK-OAX Here is where we run the streak logic.
    G_RunStreakLogic( attacker, self );

	// check for an almost capture
	CheckAlmostCapture( self, attacker );
	// check for a player that almost brought in cubes
	CheckAlmostScored( self, attacker );

	if (self->client && self->client->hook) {
		Weapon_HookFree(self->client->hook);
	}
	if ((self->client->ps.eFlags & EF_TICKING) && self->activator) {
		self->client->ps.eFlags &= ~EF_TICKING;
		self->activator->think = G_FreeEntity;
		self->activator->nextthink = level.time;
	}
	self->client->ps.pm_type = PM_DEAD;

	if ( attacker ) {
		killer = attacker->s.number;
		if ( attacker->client ) {
			killerName = attacker->client->pers.netname;
		} else {
			killerName = "<non-client>";
		}
	} else {
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( killer < 0 || killer >= MAX_CLIENTS ) {
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) ) {
		obit = "<bad obituary>";
	} else {
		obit = modNames[meansOfDeath];
	}

	G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n",
		killer, self->s.number, meansOfDeath, killerName,
		self->client->pers.netname, obit );

	// broadcast the death event to everyone
	ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
	ent->s.eventParm = meansOfDeath;
	ent->s.otherEntityNum = self->s.number;
	ent->s.otherEntityNum2 = killer;
        //Sago: Hmmm... generic? Can I transmit anything I like? Like if it is a team kill? Let's try
        ent->s.generic1 = OnSameTeam (self, attacker);
        if( !((g_gametype.integer==GT_ELIMINATION || g_gametype.integer==GT_CTF_ELIMINATION) && level.time < level.roundStartTime) )
            ent->r.svFlags = SVF_BROADCAST;	// send to everyone (if not an elimination gametype during active warmup)
        else
            ent->r.svFlags = SVF_NOCLIENT;

	self->enemy = attacker;

	self->client->ps.persistant[PERS_KILLED]++;

	if (attacker) {
		attacker->client->lastkilled_client = self->s.number;

		if ( attacker == self || OnSameTeam (self, attacker ) ) {
			if(g_gametype.integer!=GT_LMS && !((g_gametype.integer==GT_ELIMINATION || g_gametype.integer==GT_CTF_ELIMINATION) && level.time < level.roundStartTime))
                            if( (g_gametype.integer <GT_TEAM && g_ffa_gt!=1 && self->client->ps.persistant[PERS_SCORE]>0) || level.numNonSpectatorClients<3) //Cannot get negative scores by suicide
                                AddScore( attacker, self->r.currentOrigin, -1 );
		} else {
			if(g_gametype.integer!=GT_LMS){
				AddScore( attacker, self->r.currentOrigin, 1 );
				attacker->client->pers.oldmoney += 1;
				G_Printf( "Codepart_21\n");
			}

			if( meansOfDeath == MOD_GAUNTLET ) {

				// Attack gets a challenge complete:
				if(!(attacker->r.svFlags & SVF_BOT) && !(self->r.svFlags & SVF_BOT))
				ChallengeMessage(attacker,WEAPON_GAUNTLET_KILLS);
				attacker->client->pers.oldmoney += 1;

				// play humiliation on player
				attacker->client->ps.persistant[PERS_GAUNTLET_FRAG_COUNT]++;
                                G_LogPrintf( "Award: %i %i: %s gained the %s award!\n", attacker->client->ps.clientNum, 0, attacker->client->pers.netname, "GAUNTLET" );

				// add the sprite over the player's head
				attacker->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
				attacker->client->ps.eFlags |= EF_AWARD_GAUNTLET;
				attacker->client->rewardTime = level.time + REWARD_SPRITE_TIME;

				// also play humiliation on target
				self->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_GAUNTLETREWARD;
			}

                        //If neither attacker or taget is bots and not the same
                        if(self!=attacker)
                        {
                            switch(meansOfDeath)
                            {
                                case MOD_GAUNTLET:
                                    ChallengeMessage(attacker,WEAPON_GAUNTLET_KILLS);
                                    break;
                                case MOD_MACHINEGUN:
                                    ChallengeMessage(attacker,WEAPON_MACHINEGUN_KILLS);
                                    break;
                                case MOD_SHOTGUN:
                                    ChallengeMessage(attacker,WEAPON_SHOTGUN_KILLS);
                                    break;
                                case MOD_GRENADE:
                                case MOD_GRENADE_SPLASH:
                                    ChallengeMessage(attacker,WEAPON_GRANADE_KILLS);
                                    break;
                                case MOD_ROCKET:
                                case MOD_ROCKET_SPLASH:
                                    ChallengeMessage(attacker,WEAPON_ROCKET_KILLS);
                                    break;
                                case MOD_LIGHTNING:
                                    ChallengeMessage(attacker,WEAPON_LIGHTNING_KILLS);
                                    break;
                                case MOD_PLASMA:
                                case MOD_PLASMA_SPLASH:
                                    ChallengeMessage(attacker,WEAPON_PLASMA_KILLS);
                                    break;
                                case MOD_RAILGUN:
                                    if(g_instantgib.integer)
                                        ChallengeMessage(attacker,WEAPON_INSTANT_RAIL_KILLS);
                                    else
                                        ChallengeMessage(attacker,WEAPON_RAIL_KILLS);
                                    break;
                                case MOD_BFG:
                                case MOD_BFG_SPLASH:
                                    ChallengeMessage(attacker,WEAPON_BFG_KILLS);
                                    break;
																case MOD_FLAME:
		                            case MOD_FLAME_SPLASH:
		                                ChallengeMessage(attacker,WEAPON_FLAMETHROWER_KILLS);
		                                break;
																case MOD_ANTIMATTER:
				                        case MOD_ANTIMATTER_SPLASH:
				                            ChallengeMessage(attacker,WEAPON_DARKFLARE_KILLS);
				                            break;
                                case MOD_NAIL:
                                    ChallengeMessage(attacker,WEAPON_NAILGUN_KILLS);
                                    break;
                                case MOD_CHAINGUN:
                                    ChallengeMessage(attacker,WEAPON_CHAINGUN_KILLS);
                                    break;
                                case MOD_PROXIMITY_MINE:
                                    ChallengeMessage(attacker,WEAPON_MINE_KILLS);
                                    break;
                                case MOD_GRAPPLE:
                                    ChallengeMessage(attacker,WEAPON_GRAPPLE_KILLS);
                                    break;
                                case MOD_LAVA:
                                case MOD_SLIME:
                                case MOD_TRIGGER_HURT:
                                case MOD_FALLING:
                                    ChallengeMessage(attacker,WEAPON_PUSH_KILLS);
                                    break;
                                case MOD_CRUSH:
                                    ChallengeMessage(attacker,WEAPON_CRUSH_KILLS);
                                    break;
                                case MOD_TELEFRAG:
                                    ChallengeMessage(attacker,WEAPON_TELEFRAG_KILLS);
                                    break;
                            };
                            ChallengeMessage(attacker,GENERAL_MONEY);
                            ChallengeMessage(attacker,GENERAL_TOTALKILLS);
                            ChallengeMessage(self,GENERAL_TOTALDEATHS);

                            //Lets count number of powerups:
                            i = 0;
                            counter2 = 0;

                            if(attacker->client->ps.powerups[PW_QUAD]) {
                                ChallengeMessage(attacker,POWERUP_QUAD_KILL);
                                counter2++;
                            }
                            if(self->client->ps.powerups[PW_QUAD]) {
                                ChallengeMessage(attacker,POWERUP_COUNTER_QUAD);
                                i++;
                            }

                            if(attacker->client->ps.powerups[PW_HASTE]) {
                                ChallengeMessage(attacker,POWERUP_SPEED_KILL);
                                counter2++;
                            }
                            if(self->client->ps.powerups[PW_HASTE]) {
                                ChallengeMessage(attacker,POWERUP_COUNTER_SPEED);
                                i++;
                            }

                            if(attacker->client->ps.powerups[PW_INVIS]) {
                                ChallengeMessage(attacker,POWERUP_INVIS_KILL);
                                counter2++;
                            }
                            if(self->client->ps.powerups[PW_INVIS]) {
                                ChallengeMessage(attacker,POWERUP_COUNTER_INVIS);
                                i++;
                            }

                            if(attacker->client->ps.powerups[PW_FLIGHT]) {
                                ChallengeMessage(attacker,POWERUP_FLIGHT_KILL);
                                counter2++;
                            }
                            if(self->client->ps.powerups[PW_FLIGHT]) {
                                ChallengeMessage(attacker,POWERUP_COUNTER_FLIGHT);
                                i++;
                            }

                            if(self->client->ps.powerups[PW_BATTLESUIT]) {
                                ChallengeMessage(attacker,POWERUP_COUNTER_ENVIR);
                                i++;
                            }

                            if(self->client->ps.powerups[PW_REGEN]) {
                                ChallengeMessage(attacker,POWERUP_COUNTER_REGEN);
                                i++;
                            }

                            if(i>1) //The target had more than one powerup
                                ChallengeMessage(attacker,POWERUP_COUNTER_MULTI);
                            if(counter2>1) //The attacker has more than one powerup
                                ChallengeMessage(attacker,POWERUP_MULTI_KILL);
                        }

			// check for two kills in a short amount of time
			// if this is close enough to the last kill, give a reward sound
			if ( level.time - attacker->client->lastKillTime < CARNAGE_REWARD_TIME ) {
				// KK-OAX
				// Check if Multikills are enabled
				if( g_altExcellent.integer ) {
				    attacker->client->pers.multiKillCount++;
				    G_checkForMultiKill( attacker );
				} // play excellent on player
				attacker->client->ps.persistant[PERS_EXCELLENT_COUNT]++;
                                G_LogPrintf( "Award: %i %i: %s gained the %s award!\n", attacker->client->ps.clientNum, 1, attacker->client->pers.netname, "EXCELLENT" );
                                if(!level.hadBots) //There has not been any bots
                                    ChallengeMessage(attacker,AWARD_EXCELLENT);
									attacker->client->pers.oldmoney += 1;
				// add the sprite over the player's head
				attacker->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
				attacker->client->ps.eFlags |= EF_AWARD_EXCELLENT;
				attacker->client->rewardTime = level.time + REWARD_SPRITE_TIME;
			} else {
			    //KK-OAX Clear multikill count
			    //Must be 1 so the correct number of kills are displayed to the clients.
			    attacker->client->pers.multiKillCount = 1;
			}
			attacker->client->lastKillTime = level.time;
		}
	} else {
		if(g_gametype.integer!=GT_LMS && !((g_gametype.integer==GT_ELIMINATION || g_gametype.integer==GT_CTF_ELIMINATION) && level.time < level.roundStartTime))
                    if(self->client->ps.persistant[PERS_SCORE]>0 || level.numNonSpectatorClients<3) //Cannot get negative scores by suicide
			AddScore( self, self->r.currentOrigin, -1 );
	}

	// Add team bonuses
	Team_FragBonuses(self, inflictor, attacker);

	// if I committed suicide, the flag does not fall, it returns.
	if (meansOfDeath == MOD_SUICIDE) {
		if ( self->client->ps.powerups[PW_NEUTRALFLAG] ) {		// only happens in One Flag CTF
			Team_ReturnFlag( TEAM_FREE );
			self->client->ps.powerups[PW_NEUTRALFLAG] = 0;
		}
		else if ( self->client->ps.powerups[PW_REDFLAG] ) {		// only happens in standard CTF
			Team_ReturnFlag( TEAM_RED );
			self->client->ps.powerups[PW_REDFLAG] = 0;
		}
		else if ( self->client->ps.powerups[PW_BLUEFLAG] ) {	// only happens in standard CTF
			Team_ReturnFlag( TEAM_BLUE );
			self->client->ps.powerups[PW_BLUEFLAG] = 0;
		}
	}
        TossClientPersistantPowerups( self );
        if( g_gametype.integer == GT_HARVESTER ) {
                TossClientCubes( self );
        }
	// if client is in a nodrop area, don't drop anything (but return CTF flags!)
	TossClientItems( self );
//#endif

	Cmd_Score_f( self );		// show scores
	// send updated scores to any clients that are following this one,
	// or they would get stale scoreboards
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		gclient_t	*client;

		client = &level.clients[i];
		if ( client->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			continue;
		}
		if ( client->sess.spectatorClient == self->s.number ) {
			Cmd_Score_f( g_entities + i );
		}
	}

	self->takedamage = qtrue;	// can still be gibbed

	self->s.weapon = WP_NONE;
	self->s.powerups = 0;
	self->r.contents = CONTENTS_CORPSE;

	self->s.angles[0] = 0;
	self->s.angles[2] = 0;
	LookAtKiller (self, inflictor, attacker);

	VectorCopy( self->s.angles, self->client->ps.viewangles );

	self->s.loopSound = 0;

	self->r.maxs[2] = -8;

	// don't allow respawn until the death anim is done
	// g_forcerespawn may force spawning at some later time
	self->client->respawnTime = level.time + g_respawnwait.integer +i;
		if(self->client->sess.sessionTeam == TEAM_BLUE){
		self->client->respawnTime = level.time + g_teamblue_respawnwait.integer +i;
		}
		if(self->client->sess.sessionTeam == TEAM_RED){
		self->client->respawnTime = level.time + g_teamred_respawnwait.integer +i;
		}
        if(g_respawntime.integer>0) {
            for(i=0; self->client->respawnTime > i*g_respawntime.integer*1000;i++);

            self->client->respawnTime = i*g_respawntime.integer*1000;
        }
        //For testing:
        //G_Printf("Respawntime: %i\n",self->client->respawnTime);
	//However during warm up, we should respawn quicker!
	if(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS)
		if(level.time<=level.roundStartTime && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer)
			self->client->respawnTime = level.time + rand()%800;

        RespawnTimeMessage(self,self->client->respawnTime);


	// remove powerups
	memset( self->client->ps.powerups, 0, sizeof(self->client->ps.powerups) );

	// never gib in a nodrop
	contents = trap_PointContents( self->r.currentOrigin, -1 );

	if ( (self->health <= GIB_HEALTH && !(contents & CONTENTS_NODROP) && g_blood.integer) || meansOfDeath == MOD_SUICIDE) {
		// gib death
		GibEntity( self, killer );
	} else {
		// normal death
		static int i;

		switch ( i ) {
		case 0:
			anim = BOTH_DEATH1;
			break;
		case 1:
			anim = BOTH_DEATH2;
			break;
		case 2:
		default:
			anim = BOTH_DEATH3;
			break;
		}

		// for the no-blood option, we need to prevent the health
		// from going to gib level
		if ( self->health <= GIB_HEALTH ) {
			self->health = GIB_HEALTH+1;
		}

		self->client->ps.legsAnim =
			( ( self->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
		self->client->ps.torsoAnim =
			( ( self->client->ps.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

		G_AddEvent( self, EV_DEATH1 + i, killer );

		// the body can still be gibbed
		self->die = body_die;

		// globally cycle through the different death animations
		i = ( i + 1 ) % 3;

		if (self->s.eFlags & EF_KAMIKAZE) {
			Kamikaze_DeathTimer( self );
		}
	}

	trap_LinkEntity (self);

}


/*
================
CheckArmor
================
*/
int CheckArmor (gentity_t *ent, int damage, int dflags)
{
	gclient_t	*client;
	int			save;
	int			count;

	if (!damage)
		return 0;

	client = ent->client;

	if (!client)
		return 0;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	// armor
	count = client->ps.stats[STAT_ARMOR];
	save = ceil( damage * g_armorprotect.value );
	if (save >= count)
		save = count;

	if (!save)
		return 0;

	client->ps.stats[STAT_ARMOR] -= save;

	return save;
}

/*
================
RaySphereIntersections
================
*/
int RaySphereIntersections( vec3_t origin, float radius, vec3_t point, vec3_t dir, vec3_t intersections[2] ) {
	float b, c, d, t;

	//	| origin - (point + t * dir) | = radius
	//	a = dir[0]^2 + dir[1]^2 + dir[2]^2;
	//	b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
	//	c = (point[0] - origin[0])^2 + (point[1] - origin[1])^2 + (point[2] - origin[2])^2 - radius^2;

	// normalize dir so a = 1
	VectorNormalize(dir);
	b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
	c = (point[0] - origin[0]) * (point[0] - origin[0]) +
		(point[1] - origin[1]) * (point[1] - origin[1]) +
		(point[2] - origin[2]) * (point[2] - origin[2]) -
		radius * radius;

	d = b * b - 4 * c;
	if (d > 0) {
		t = (- b + sqrt(d)) / 2;
		VectorMA(point, t, dir, intersections[0]);
		t = (- b - sqrt(d)) / 2;
		VectorMA(point, t, dir, intersections[1]);
		return 2;
	}
	else if (d == 0) {
		t = (- b ) / 2;
		VectorMA(point, t, dir, intersections[0]);
		return 1;
	}
	return 0;
}

/*
================
G_InvulnerabilityEffect
================
*/
int G_InvulnerabilityEffect( gentity_t *targ, vec3_t dir, vec3_t point, vec3_t impactpoint, vec3_t bouncedir ) {
	gentity_t	*impact;
	vec3_t		intersections[2], vec;
	int			n;

	if ( !targ->client ) {
		return qfalse;
	}
	VectorCopy(dir, vec);
	VectorInverse(vec);
	// sphere model radius = 42 units
	n = RaySphereIntersections( targ->client->ps.origin, 42, point, vec, intersections);
	if (n > 0) {
		impact = G_TempEntity( targ->client->ps.origin, EV_INVUL_IMPACT );
		VectorSubtract(intersections[0], targ->client->ps.origin, vec);
		vectoangles(vec, impact->s.angles);
		impact->s.angles[0] += 90;
		if (impact->s.angles[0] > 360)
			impact->s.angles[0] -= 360;
		if ( impactpoint ) {
			VectorCopy( intersections[0], impactpoint );
		}
		if ( bouncedir ) {
			VectorCopy( vec, bouncedir );
			VectorNormalize( bouncedir );
		}
		return qtrue;
	}
	else {
		return qfalse;
	}
}

/*
catchup_damage
*/
static int catchup_damage(int damage, int attacker_points, int target_points) {
    int newdamage;
    if(g_catchup.integer <= 0 )
        return damage;
    //Reduce damage
    if(attacker_points<=target_points+5)
        return damage; //Never reduce damage if only 5 points ahead.

    newdamage=damage-((attacker_points-target_points-5) * (g_catchup.integer*damage))/100;
    if(newdamage<damage/2)
        return damage/2;
    return newdamage;
}

/*
============
G_LocationDamage
============
*/
int G_LocationDamage(vec3_t point, gentity_t* targ, gentity_t* attacker, int take) {
	vec3_t bulletPath;
	vec3_t bulletAngle;

	int clientHeight;
	int clientFeetZ;
	int clientRotation;
	int bulletHeight;
	int bulletRotation;	// Degrees rotation around client.
				// used to check Back of head vs. Face
	int impactRotation;


	// First things first.  If we're not damaging them, why are we here?
	if (!take)
		return 0;

	// Point[2] is the REAL world Z. We want Z relative to the clients feet

	// Where the feet are at [real Z]
	clientFeetZ  = targ->r.currentOrigin[2] + targ->r.mins[2];
	// How tall the client is [Relative Z]
	clientHeight = targ->r.maxs[2] - targ->r.mins[2];
	// Where the bullet struck [Relative Z]
	bulletHeight = point[2] - clientFeetZ;

	// Get a vector aiming from the client to the bullet hit
	VectorSubtract(targ->r.currentOrigin, point, bulletPath);
	// Convert it into PITCH, ROLL, YAW
	vectoangles(bulletPath, bulletAngle);

	clientRotation = targ->client->ps.viewangles[YAW];
	bulletRotation = bulletAngle[YAW];

	impactRotation = abs(clientRotation-bulletRotation);

	impactRotation += 45; // just to make it easier to work with
	impactRotation = impactRotation % 360; // Keep it in the 0-359 range

	if (impactRotation < 90)
		targ->client->lasthurt_location = LOCATION_BACK;
	else if (impactRotation < 180)
		targ->client->lasthurt_location = LOCATION_RIGHT;
	else if (impactRotation < 270)
		targ->client->lasthurt_location = LOCATION_FRONT;
	else if (impactRotation < 360)
		targ->client->lasthurt_location = LOCATION_LEFT;
	else
		targ->client->lasthurt_location = LOCATION_NONE;

	// The upper body never changes height, just distance from the feet
		if (bulletHeight > clientHeight - 2)
			targ->client->lasthurt_location |= LOCATION_HEAD;
		else if (bulletHeight > clientHeight - 8)
			targ->client->lasthurt_location |= LOCATION_FACE;
		else if (bulletHeight > clientHeight - 10)
			targ->client->lasthurt_location |= LOCATION_SHOULDER;
		else if (bulletHeight > clientHeight - 16)
			targ->client->lasthurt_location |= LOCATION_CHEST;
		else if (bulletHeight > clientHeight - 26)
			targ->client->lasthurt_location |= LOCATION_STOMACH;
		else if (bulletHeight > clientHeight - 29)
			targ->client->lasthurt_location |= LOCATION_GROIN;
		else if (bulletHeight < 4)
			targ->client->lasthurt_location |= LOCATION_FOOT;
		else
			// The leg is the only thing that changes size when you duck,
			// so we check for every other parts RELATIVE location, and
			// whats left over must be the leg.
			targ->client->lasthurt_location |= LOCATION_LEG;



		// Check the location ignoring the rotation info
		switch ( targ->client->lasthurt_location &
				~(LOCATION_BACK | LOCATION_LEFT | LOCATION_RIGHT | LOCATION_FRONT) )
		{
		case LOCATION_HEAD:
			take *= 3.8;
			break;
		case LOCATION_FACE:
			if (targ->client->lasthurt_location & LOCATION_FRONT)
				take *= 7.0; // Faceshots REALLY suck
			else
				take *= 3.8;
			break;
		case LOCATION_SHOULDER:
			if (targ->client->lasthurt_location & (LOCATION_FRONT | LOCATION_BACK))
				take *= 1.4; // Throat or nape of neck
			else
				take *= 1.1; // Shoulders
			break;
		case LOCATION_CHEST:
			if (targ->client->lasthurt_location & (LOCATION_FRONT | LOCATION_BACK))
				take *= 1.3; // Belly or back
			else
				take *= 0.8; // Arms
			break;
		case LOCATION_STOMACH:
			take *= 1.2;
			break;
		case LOCATION_GROIN:
			if (targ->client->lasthurt_location & LOCATION_FRONT)
				take *= 1.3; // Groin shot
			break;
		case LOCATION_LEG:
			take *= 0.7;
			break;
		case LOCATION_FOOT:
			take *= 0.5;
			break;

		}
	return take;

}


/*
============
T_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/

void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
			   vec3_t dir, vec3_t point, int damage, int dflags, int mod ) {
	gclient_t	*client;
	int			take;
	//int			save;
	int			asave;
	int			knockback;
	int			max;
	int			i;
	vec3_t	kvels;
	vec3_t	snaporigin;
	vec3_t	snapcorigin;
	vec3_t	snaptrbase;
	vec3_t	addd;
	vec3_t	bddd;
	char	*gtarget;
	char	*gtargetname;
	char	*gclassname;
	char	*gmodifier;
	vec3_t		bouncedir, impactpoint;
	vec3_t		anglesis;

if(g_building.integer == 2){
	//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
if(!attacker->gmodtool){
//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	if (!targ->takedamage) {
		return;
	}

	// the intermission has allready been qualified for, so don't
	// allow any extra scoring
	if ( level.intermissionQueued ) {
		return;
	}
	if ( targ->client && mod != MOD_JUICED) {
		if ( targ->client->invulnerabilityTime > level.time) {
			if ( dir && point ) {
				G_InvulnerabilityEffect( targ, dir, point, impactpoint, bouncedir );
			}
			return;
		}
	}
        //Sago: This was moved up
        client = targ->client;

        //Sago: See if the client was sent flying
        //Check if damage is by somebody who is not a player!
        if( (!attacker || attacker->s.eType != ET_PLAYER) && client && client->lastSentFlying>-1 && ( mod==MOD_FALLING || mod==MOD_LAVA || mod==MOD_SLIME || mod==MOD_TRIGGER_HURT || mod==MOD_SUICIDE) )  {
            if( client->lastSentFlyingTime+5000<level.time) {
                client->lastSentFlying = -1; //More than 5 seconds, not a kill!
            } else {
                //G_Printf("LastSentFlying %i\n",client->lastSentFlying);
                attacker = &g_entities[client->lastSentFlying];
            }
        }

	if ( !inflictor ) {
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}
	if ( !attacker ) {
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

		// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER) {
		if (strcmp(targ->classname, "func_breakable") && targ->use && (targ->moverState == MOVER_POS1 || targ->moverState == ROTATOR_POS1)) {
			targ->use(targ, inflictor, attacker);
			}
		if (!strcmp(targ->classname, "func_train") && targ->health > 0) {
			G_UseTargets(targ, attacker);
		} else {	// entity is a func_breakable
			if ( (targ->spawnflags & 1024) && attacker == attacker )
			return;
			if ( (targ->spawnflags & 2048) && IsBot(attacker) )
				return;
			if ( (targ->spawnflags & 4096) && strstr(attacker->classname, "shooter_") )
				return;
			
		}
	}
	if( g_gametype.integer == GT_OBELISK && CheckObeliskAttack( targ, attacker ) ) {
		return;
	}
	// reduce damage by the attacker's handicap value
	// unless they are rocket jumping
	if ( attacker->client && attacker != targ ) {
		max = attacker->client->ps.stats[STAT_MAX_HEALTH];
		if( bg_itemlist[attacker->client->ps.stats[STAT_PERSISTANT_POWERUP]].giTag == PW_GUARD ) {
			max /= 2;
		}
		damage = damage * max / 100;
	}

        //Sago: I have moved this up
	//client = targ->client;

	if ( client ) {
		if ( client->noclip ) {
			return;
		}
	}

	if ( !dir ) {
		dflags |= DAMAGE_NO_KNOCKBACK;
	} else {
		VectorNormalize(dir);
	}

	knockback = damage;
if(!attacker->weaponpack){
	if ( mod == MOD_GAUNTLET )
	knockback *= g_gknockback.value*25;
	if ( mod == MOD_MACHINEGUN )
	knockback *= g_mgknockback.value;
	if ( mod == MOD_SHOTGUN )
	knockback *= g_sgknockback.value;
	if ( mod == MOD_GRENADE )
	knockback *= g_glknockback.value;
	if ( mod == MOD_GRENADE_SPLASH )
	knockback *= g_glknockback.value;
	if ( mod == MOD_ROCKET )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_ROCKET_SPLASH )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_PLASMA )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_PLASMA_SPLASH )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_RAILGUN )
	knockback *= g_rgknockback.value;
	if ( mod == MOD_LIGHTNING )
	knockback *= g_lgknockback.value;
	if ( mod == MOD_BFG )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_BFG_SPLASH )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_NAIL )
	knockback *= g_ngknockback.value;
	if ( mod == MOD_CHAINGUN )
	knockback *= g_cgknockback.value;
	if ( mod == MOD_PROXIMITY_MINE )
	knockback *= g_plknockback.value;
	if ( mod == MOD_FLAME )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_FLAME_SPLASH )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_ANTIMATTER )
	knockback *= g_amknockback.value;
	if ( mod == MOD_ANTIMATTER_SPLASH )
	knockback *= g_amknockback.value;
}
if(attacker->weaponpack == 1){
	if ( mod == MOD_GAUNTLET )
	knockback *= g_gknockback.value*25;
	if ( mod == MOD_MACHINEGUN )
	knockback *= g_mgknockback.value;
	if ( mod == MOD_SHOTGUN )
	knockback *= g_sgknockback.value;
	if ( mod == MOD_GRENADE )
	knockback *= g_glknockback.value;
	if ( mod == MOD_GRENADE_SPLASH )
	knockback *= g_glknockback.value;
	if ( mod == MOD_ROCKET )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_ROCKET_SPLASH )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_PLASMA )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_PLASMA_SPLASH )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_RAILGUN )
	knockback *= g_rgknockback.value;
	if ( mod == MOD_LIGHTNING )
	knockback *= g_lgknockback.value;
	if ( mod == MOD_BFG )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_BFG_SPLASH )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_NAIL )
	knockback *= g_ngknockback.value;
	if ( mod == MOD_CHAINGUN )
	knockback *= g_cgknockback.value;
	if ( mod == MOD_PROXIMITY_MINE )
	knockback *= g_plknockback.value;
	if ( mod == MOD_FLAME )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_FLAME_SPLASH )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_ANTIMATTER )
	knockback *= g_amknockback.value;
	if ( mod == MOD_ANTIMATTER_SPLASH )
	knockback *= g_amknockback.value;
}
if(attacker->weaponpack == 2){
	if ( mod == MOD_GAUNTLET )
	knockback *= g_gknockback.value;
	if ( mod == MOD_MACHINEGUN )
	knockback *= g_mgknockback.value;
	if ( mod == MOD_SHOTGUN )
	knockback *= g_sgknockback.value;
	if ( mod == MOD_GRENADE )
	knockback *= g_glknockback.value;
	if ( mod == MOD_GRENADE_SPLASH )
	knockback *= g_glknockback.value;
	if ( mod == MOD_ROCKET )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_ROCKET_SPLASH )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_PLASMA )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_PLASMA_SPLASH )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_RAILGUN )
	knockback *= g_rgknockback.value;
	if ( mod == MOD_LIGHTNING )
	knockback *= g_lgknockback.value;
	if ( mod == MOD_BFG )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_BFG_SPLASH )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_NAIL )
	knockback *= g_ngknockback.value;
	if ( mod == MOD_CHAINGUN )
	knockback *= g_cgknockback.value;
	if ( mod == MOD_PROXIMITY_MINE )
	knockback *= g_plknockback.value;
	if ( mod == MOD_FLAME )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_FLAME_SPLASH )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_ANTIMATTER )
	knockback *= g_amknockback.value;
	if ( mod == MOD_ANTIMATTER_SPLASH )
	knockback *= g_amknockback.value;
}
if(attacker->weaponpack == 3){
	if ( mod == MOD_GAUNTLET )
	knockback *= g_gknockback.value;
	if ( mod == MOD_MACHINEGUN )
	knockback *= g_mgknockback.value;
	if ( mod == MOD_SHOTGUN )
	knockback *= g_sgknockback.value;
	if ( mod == MOD_GRENADE )
	knockback *= g_glknockback.value;
	if ( mod == MOD_GRENADE_SPLASH )
	knockback *= g_glknockback.value;
	if ( mod == MOD_ROCKET )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_ROCKET_SPLASH )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_PLASMA )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_PLASMA_SPLASH )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_RAILGUN )
	knockback *= g_rgknockback.value;
	if ( mod == MOD_LIGHTNING )
	knockback *= g_lgknockback.value;
	if ( mod == MOD_BFG )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_BFG_SPLASH )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_NAIL )
	knockback *= g_ngknockback.value;
	if ( mod == MOD_CHAINGUN )
	knockback *= g_cgknockback.value;
	if ( mod == MOD_PROXIMITY_MINE )
	knockback *= g_plknockback.value;
	if ( mod == MOD_FLAME )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_FLAME_SPLASH )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_ANTIMATTER )
	knockback *= g_amknockback.value;
	if ( mod == MOD_ANTIMATTER_SPLASH )
	knockback *= g_amknockback.value;
}
if(attacker->weaponpack == 4){
	if ( mod == MOD_GRAPPLE )
	knockback = -6000;
	if ( mod == MOD_GAUNTLET )
	knockback *= g_gknockback.value;
	if ( mod == MOD_MACHINEGUN )
	knockback *= g_mgknockback.value;
	if ( mod == MOD_SHOTGUN )
	knockback *= g_sgknockback.value;
	if ( mod == MOD_GRENADE )
	knockback *= g_glknockback.value;
	if ( mod == MOD_GRENADE_SPLASH )
	knockback *= g_glknockback.value;
	if ( mod == MOD_ROCKET )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_ROCKET_SPLASH )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_PLASMA )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_PLASMA_SPLASH )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_RAILGUN )
	knockback *= g_rgknockback.value;
	if ( mod == MOD_LIGHTNING )
	knockback *= g_lgknockback.value;
	if ( mod == MOD_BFG )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_BFG_SPLASH )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_NAIL )
	knockback *= g_ngknockback.value;
	if ( mod == MOD_CHAINGUN )
	knockback *= g_cgknockback.value;
	if ( mod == MOD_PROXIMITY_MINE )
	knockback *= g_plknockback.value;
	if ( mod == MOD_FLAME )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_FLAME_SPLASH )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_ANTIMATTER )
	knockback *= g_amknockback.value;
	if ( mod == MOD_ANTIMATTER_SPLASH )
	knockback *= g_amknockback.value;
}

	if ( targ->flags & FL_NO_KNOCKBACK ) {
		knockback = 0;
	}
	if ( dflags & DAMAGE_NO_KNOCKBACK ) {
		knockback = 0;
	}

	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client ) {
		vec3_t	kvel;
		float	mass;

		mass = 200;

		VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
		VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

		// set the timer so that the other client can't cancel
		// out the movement immediately
		if ( !targ->client->ps.pm_time ) {
			int		t;

			t = knockback * 2;
			if ( t < 50 ) {
				t = 50;
			}
			if ( t > 200 ) {
				t = 200;
			}
			targ->client->ps.pm_time = t;
			targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		}
                //Remeber the last person to hurt the player
                if( !g_awardpushing.integer || targ==attacker || OnSameTeam (targ, attacker)) {
                    targ->client->lastSentFlying = -1;
                } else {
	/*if ( pm->waterlevel <= 1 ) {
		if ( pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK) ) {
			// if getting knocked back, no friction
			if ( ! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) ) {
				control = speed < pm_stopspeed ? pm_stopspeed : speed;
				drop += control*pm_friction*pml.frametime;
			}
		}
	}*/
                    targ->client->lastSentFlying = attacker->s.number;
                    targ->client->lastSentFlyingTime = level.time;
                }
	}

	// check for completely getting out of the damage
	if ( !(dflags & DAMAGE_NO_PROTECTION) ) {

		// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
		// if the attacker was on the same team
		if ( mod != MOD_JUICED && mod != MOD_CRUSH && targ != attacker && !(dflags & DAMAGE_NO_TEAM_PROTECTION) && OnSameTeam (targ, attacker)  ) {
			if ( ( !g_friendlyFire.integer && g_gametype.integer != GT_ELIMINATION && g_gametype.integer != GT_CTF_ELIMINATION ) || ( g_elimination_selfdamage.integer<2 &&	(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION) ) ) {
				return;
			}
		}
		if (mod == MOD_PROXIMITY_MINE) {
			if (inflictor && inflictor->parent && OnSameTeam(targ, inflictor->parent)) {
				return;
			}
			if (targ == attacker) {
				return;
			}
		}

		// check for godmode
		if ( targ->flags & FL_GODMODE ) {
			return;
		}

                if(targ->client && targ->client->spawnprotected) {
                   if(level.time>targ->client->respawnTime+g_spawnprotect.integer)
                       targ->client->spawnprotected = qfalse;
                   else
                       if( (mod > MOD_UNKNOWN && mod < MOD_WATER) || mod == MOD_TELEFRAG || mod>MOD_TRIGGER_HURT)
                       return;
                }
	}

	// battlesuit protects from all radius damage (but takes knockback)
	// and protects 50% against all damage
	if ( client && client->ps.powerups[PW_BATTLESUIT] ) {
		G_AddEvent( targ, EV_POWERUP_BATTLESUIT, 0 );
		if ( ( dflags & DAMAGE_RADIUS ) || ( mod == MOD_FALLING ) ) {
			return;
		}
		damage *= 0.5;
	}

	// add to the attacker's hit counter (if the target isn't a general entity like a prox mine)
	if ( attacker->client && client
			&& targ != attacker && targ->health > 0
			&& targ->s.eType != ET_MISSILE
			&& targ->s.eType != ET_GENERAL) {
		if ( OnSameTeam( targ, attacker ) ) {
			attacker->client->ps.persistant[PERS_HITS]--;
		} else {
			attacker->client->ps.persistant[PERS_HITS]++;
		}
		attacker->client->ps.persistant[PERS_ATTACKEE_ARMOR] = (targ->health<<8)|(client->ps.stats[STAT_ARMOR]);
	}

	// always give half damage if hurting self
	// calculated after knockback, so rocket jumping works
	if ( targ == attacker) {
		damage *= 0.5;
	}

        if(targ && targ->client && attacker->client )
            damage = catchup_damage(damage, attacker->client->ps.persistant[PERS_SCORE], targ->client->ps.persistant[PERS_SCORE]);

        if(g_damageModifier.value > 0.01) {
            damage *= g_damageModifier.value;
        }

	if ( damage < 1 ) {
		damage = 1;
	}

        if(targ == attacker && (g_dmflags.integer & DF_NO_SELF_DAMAGE) )
            damage = 0;

	if ((g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS || g_elimination_allgametypes.integer)
				&& g_elimination_selfdamage.integer<1 && ( targ == attacker ||  mod == MOD_FALLING )) {
		damage = 0;
	}


//So people can be telefragged!
	if ((g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS) && level.time < level.roundStartTime && ((mod == MOD_LAVA) || (mod == MOD_SLIME)) ) {
		damage = 1000;
	}


	take = damage;
	//save = 0;

	// save some from armor
	asave = CheckArmor (targ, take, dflags);
	take -= asave;

	if ( g_debugDamage.integer ) {
		G_Printf( "%i: client:%i health:%i damage:%i armor:%i\n", level.time, targ->s.number,
			targ->health, take, asave );
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( client ) {
		if ( attacker ) {
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
                } else if(client->lastSentFlying) {
                        client->ps.persistant[PERS_ATTACKER] = client->lastSentFlying;
                } else {
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}
		client->damage_armor += asave;
		client->damage_blood += take;
		client->damage_knockback += knockback;
		if ( dir ) {
			VectorCopy ( dir, client->damage_from );
			client->damage_fromWorld = qfalse;
		} else {
			VectorCopy ( targ->r.currentOrigin, client->damage_from );
			client->damage_fromWorld = qtrue;
		}
	}

	// See if it's the player hurting the emeny flag carrier
	if( g_gametype.integer == GT_CTF || g_gametype.integer == GT_1FCTF || g_gametype.integer == GT_CTF_ELIMINATION) {
		Team_CheckHurtCarrier(targ, attacker);
	}

	if (targ->client) {
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod = mod;

		if(g_locationdamage.integer == 1){
		// Modify the damage for location damage
		if (point && targ && targ->health > 0 && attacker && take)
		take = G_LocationDamage(point, targ, attacker, take);
		else
		targ->client->lasthurt_location = LOCATION_NONE;
		}
	}

	//If vampire is enabled, gain health but not from self or teammate, cannot steal more than targ has
	if( g_vampire.value>0.0 && (targ != attacker) && take > 0 &&
                !(OnSameTeam(targ, attacker)) && attacker->health > 0 && targ->health > 0 )
	{
		if(take<targ->health)
			attacker->health += (int)(((float)take)*g_vampire.value);
		else
			attacker->health += (int)(((float)targ->health)*g_vampire.value);
		if(attacker->health>g_vampireMaxHealth.integer)
			attacker->health = g_vampireMaxHealth.integer;
	}

	// oatmeal begin


	// Vampire Weapons
	if(mod==MOD_MACHINEGUN){
		if(g_mgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_SHOTGUN){
		if(g_sgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_RAILGUN){
		if(g_rgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_LIGHTNING){
if(g_portallight.integer == 1){
PortalTouches( targ );
}
		if(g_lgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_NAIL){
		if(g_ngvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_CHAINGUN){
		if(g_cgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_PROXIMITY_MINE){
		if(g_plvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_GRENADE)||(mod==MOD_GRENADE_SPLASH)){
		if(g_glvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_ROCKET)||(mod==MOD_ROCKET_SPLASH)){
		if(g_rlvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_PLASMA)||(mod==MOD_PLASMA_SPLASH)){
		if(g_pgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_BFG)||(mod==MOD_BFG_SPLASH)){
		if(g_bfgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_FLAME)||(mod==MOD_FLAME_SPLASH)){
		if(g_ftvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_ANTIMATTER)||(mod==MOD_ANTIMATTER_SPLASH)){
		if(g_amvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}


	// oatmeal end


	// do the damage
	if (take) {
		targ->health = targ->health - take;
		if ( targ->client ) {
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
		}
		
		if ( !strcmp(targ->classname, "func_breakable") ) {
			targ->health -= damage;
			if ( targ->health <= 0 ){
			    Break_Breakable(targ, attacker);
			}
		}

		if ( targ->health <= 0 ) {
			if ( client )
				targ->flags |= FL_NO_KNOCKBACK;

			if (targ->health < -999)
				targ->health = -999;

			targ->enemy = attacker;
			targ->die (targ, inflictor, attacker, take, mod);
			return;
		} else if ( targ->pain ) {
			targ->pain (targ, attacker, take);
		}
	}
}
//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii	
if(mod == MOD_GAUNTLET){
if(targ->client){
if(attacker->gmodtool != 141){
if(attacker->gmodtool != 140){
if(attacker->gmodtool != 139){
if(attacker->gmodtool != 135){
if(attacker->gmodtool != 134){
if(attacker->gmodtool != 133){
if(attacker->gmodtool != 68){
if(attacker->gmodtool != 61){
if(attacker->gmodtool != 6){
return;
}
}
}
}
}
}
}
}
}
}
if(!attacker->client){
return;
}
if(targ->owner != attacker->s.clientNum + 1){
if(targ->owner != 0){
trap_SendServerCommand( attacker->s.clientNum, va( "cp Owned_by_%s\n", targ->ownername ));
return;
}	
}
if(attacker->gmodtool == 1){
	G_FreeEntity(targ);
}
if(attacker->gmodtool == 2){
	targ->sandphys = 1;
	targ->sandbounce = attacker->gmodmodifier;
	targ->physicsObject = qtrue;
	targ->physicsBounce = attacker->gmodmodifier;
	targ->s.pos.trType = TR_GRAVITY;
	targ->s.pos.trTime = level.time;
}
if(attacker->gmodtool == 3){
	targ->sandphys = 2;
	targ->physicsObject = qfalse;
	targ->s.pos.trType = TR_STATIONARY;
	targ->s.pos.trTime = level.time;
}
if(attacker->gmodtool == 4){
	targ->sandphys = 3;
	targ->physicsObject = qfalse;
	targ->s.pos.trType = TR_LINEAR;
	targ->s.pos.trTime = level.time;
}
if(attacker->gmodtool == 5){
	targ->wait = attacker->gmodmodifier;
}
if(attacker->gmodtool == 6){
	targ->model = "12345";
	targ->s.modelindex = G_ModelIndex( attacker->gmodmodifiers );
	targ->modelnumber = attacker->gmodmodifier;
	targ->sandboxindex = G_ModelIndex( attacker->gmodmodifiers );
}
if(attacker->gmodtool == 7){
	targ->spawnflags = attacker->gmodmodifier;
}
if(attacker->gmodtool == 8){
	targ->speed = attacker->gmodmodifier;
}
if(attacker->gmodtool == 9){
	targ->count = attacker->gmodmodifier;
}
if(attacker->gmodtool == 10){
	targ->health = attacker->gmodmodifier;
}
if(attacker->gmodtool == 11){
	targ->damage = attacker->gmodmodifier;
}
if(attacker->gmodtool == 12){
if(attacker->gmodmodifier == 1){
	targ->target = "gtarget1";
}
if(attacker->gmodmodifier == 2){
	targ->target = "gtarget2";
}
if(attacker->gmodmodifier == 3){
	targ->target = "gtarget3";
}
if(attacker->gmodmodifier == 4){
	targ->target = "gtarget4";
}
if(attacker->gmodmodifier == 5){
	targ->target = "gtarget5";
}
if(attacker->gmodmodifier == 6){
	targ->target = "gtarget6";
}
if(attacker->gmodmodifier == 7){
	targ->target = "gtarget7";
}
if(attacker->gmodmodifier == 8){
	targ->target = "gtarget8";
}
if(attacker->gmodmodifier == 9){
	targ->target = "gtarget9";
}
if(attacker->gmodmodifier == 10){
	targ->target = "gtarget10";
}
if(attacker->gmodmodifier == 11){
	targ->target = "gtarget11";
}
if(attacker->gmodmodifier == 12){
	targ->target = "gtarget12";
}
if(attacker->gmodmodifier == 13){
	targ->target = "gtarget13";
}
if(attacker->gmodmodifier == 14){
	targ->target = "gtarget14";
}
if(attacker->gmodmodifier == 15){
	targ->target = "gtarget15";
}
if(attacker->gmodmodifier == 16){
	targ->target = "gtarget16";
}
if(attacker->gmodmodifier == 17){
	targ->target = "gtarget17";
}
if(attacker->gmodmodifier == 18){
	targ->target = "gtarget18";
}
if(attacker->gmodmodifier == 19){
	targ->target = "gtarget19";
}
if(attacker->gmodmodifier == 20){
	targ->target = "gtarget20";
}
if(attacker->gmodmodifier == 21){
	targ->target = "gtarget21";
}
if(attacker->gmodmodifier == 22){
	targ->target = "gtarget22";
}
if(attacker->gmodmodifier == 23){
	targ->target = "gtarget23";
}
if(attacker->gmodmodifier == 24){
	targ->target = "gtarget24";
}
if(attacker->gmodmodifier == 25){
	targ->target = "gtarget25";
}
if(attacker->gmodmodifier == 26){
	targ->target = "gtarget26";
}
if(attacker->gmodmodifier == 27){
	targ->target = "gtarget27";
}
if(attacker->gmodmodifier == 28){
	targ->target = "gtarget28";
}
if(attacker->gmodmodifier == 29){
	targ->target = "gtarget29";
}
if(attacker->gmodmodifier == 30){
	targ->target = "gtarget30";
}
if(attacker->gmodmodifier == 31){
	targ->target = "gtarget31";
}
if(attacker->gmodmodifier == 32){
	targ->target = "gtarget32";
}
if(attacker->gmodmodifier == 33){
	targ->target = "gtarget33";
}
if(attacker->gmodmodifier == 34){
	targ->target = "gtarget34";
}
if(attacker->gmodmodifier == 35){
	targ->target = "gtarget35";
}
if(attacker->gmodmodifier == 36){
	targ->target = "gtarget36";
}
if(attacker->gmodmodifier == 37){
	targ->target = "gtarget37";
}
if(attacker->gmodmodifier == 38){
	targ->target = "gtarget38";
}
if(attacker->gmodmodifier == 39){
	targ->target = "gtarget39";
}
if(attacker->gmodmodifier == 40){
	targ->target = "gtarget40";
}
if(attacker->gmodmodifier == 41){
	targ->target = "gtarget41";
}
if(attacker->gmodmodifier == 42){
	targ->target = "gtarget42";
}
if(attacker->gmodmodifier == 43){
	targ->target = "gtarget43";
}
if(attacker->gmodmodifier == 44){
	targ->target = "gtarget44";
}
if(attacker->gmodmodifier == 45){
	targ->target = "gtarget45";
}
if(attacker->gmodmodifier == 46){
	targ->target = "gtarget46";
}
if(attacker->gmodmodifier == 47){
	targ->target = "gtarget47";
}
if(attacker->gmodmodifier == 48){
	targ->target = "gtarget48";
}
if(attacker->gmodmodifier == 49){
	targ->target = "gtarget49";
}
if(attacker->gmodmodifier == 50){
	targ->target = "gtarget50";
}
if(attacker->gmodmodifier == 51){
	targ->target = "gtarget51";
}
if(attacker->gmodmodifier == 52){
	targ->target = "gtarget52";
}
if(attacker->gmodmodifier == 53){
	targ->target = "gtarget53";
}
if(attacker->gmodmodifier == 54){
	targ->target = "gtarget54";
}
if(attacker->gmodmodifier == 55){
	targ->target = "gtarget55";
}
if(attacker->gmodmodifier == 56){
	targ->target = "gtarget56";
}
if(attacker->gmodmodifier == 57){
	targ->target = "gtarget57";
}
if(attacker->gmodmodifier == 58){
	targ->target = "gtarget58";
}
if(attacker->gmodmodifier == 59){
	targ->target = "gtarget59";
}
if(attacker->gmodmodifier == 60){
	targ->target = "gtarget60";
}
if(attacker->gmodmodifier == 61){
	targ->target = "gtarget61";
}
if(attacker->gmodmodifier == 62){
	targ->target = "gtarget62";
}
if(attacker->gmodmodifier == 63){
	targ->target = "gtarget63";
}
if(attacker->gmodmodifier == 64){
	targ->target = "gtarget64";
}
if(attacker->gmodmodifier == 65){
	targ->target = "gtarget65";
}
if(attacker->gmodmodifier == 66){
	targ->target = "gtarget66";
}
if(attacker->gmodmodifier == 67){
	targ->target = "gtarget67";
}
if(attacker->gmodmodifier == 68){
	targ->target = "gtarget68";
}
if(attacker->gmodmodifier == 69){
	targ->target = "gtarget69";
}
if(attacker->gmodmodifier == 70){
	targ->target = "gtarget70";
}
if(attacker->gmodmodifier == 71){
	targ->target = "gtarget71";
}
if(attacker->gmodmodifier == 72){
	targ->target = "gtarget72";
}
if(attacker->gmodmodifier == 73){
	targ->target = "gtarget73";
}
if(attacker->gmodmodifier == 74){
	targ->target = "gtarget74";
}
if(attacker->gmodmodifier == 75){
	targ->target = "gtarget75";
}
if(attacker->gmodmodifier == 76){
	targ->target = "gtarget76";
}
if(attacker->gmodmodifier == 77){
	targ->target = "gtarget77";
}
if(attacker->gmodmodifier == 78){
	targ->target = "gtarget78";
}
if(attacker->gmodmodifier == 79){
	targ->target = "gtarget79";
}
if(attacker->gmodmodifier == 80){
	targ->target = "gtarget80";
}
if(attacker->gmodmodifier == 81){
	targ->target = "gtarget81";
}
if(attacker->gmodmodifier == 82){
	targ->target = "gtarget82";
}
if(attacker->gmodmodifier == 83){
	targ->target = "gtarget83";
}
if(attacker->gmodmodifier == 84){
	targ->target = "gtarget84";
}
if(attacker->gmodmodifier == 85){
	targ->target = "gtarget85";
}
if(attacker->gmodmodifier == 86){
	targ->target = "gtarget86";
}
if(attacker->gmodmodifier == 87){
	targ->target = "gtarget87";
}
if(attacker->gmodmodifier == 88){
	targ->target = "gtarget88";
}
if(attacker->gmodmodifier == 89){
	targ->target = "gtarget89";
}
if(attacker->gmodmodifier == 90){
	targ->target = "gtarget90";
}
if(attacker->gmodmodifier == 91){
	targ->target = "gtarget91";
}
if(attacker->gmodmodifier == 92){
	targ->target = "gtarget92";
}
if(attacker->gmodmodifier == 93){
	targ->target = "gtarget93";
}
if(attacker->gmodmodifier == 94){
	targ->target = "gtarget94";
}
if(attacker->gmodmodifier == 95){
	targ->target = "gtarget95";
}
if(attacker->gmodmodifier == 96){
	targ->target = "gtarget96";
}
if(attacker->gmodmodifier == 97){
	targ->target = "gtarget97";
}
if(attacker->gmodmodifier == 98){
	targ->target = "gtarget98";
}
if(attacker->gmodmodifier == 99){
	targ->target = "gtarget99";
}
if(attacker->gmodmodifier == 100){
	targ->target = "gtarget100";
}
}
if(attacker->gmodtool == 13){
if(attacker->gmodmodifier == 1){
	targ->targetname = "gtarget1";
}
if(attacker->gmodmodifier == 2){
	targ->targetname = "gtarget2";
}
if(attacker->gmodmodifier == 3){
	targ->targetname = "gtarget3";
}
if(attacker->gmodmodifier == 4){
	targ->targetname = "gtarget4";
}
if(attacker->gmodmodifier == 5){
	targ->targetname = "gtarget5";
}
if(attacker->gmodmodifier == 6){
	targ->targetname = "gtarget6";
}
if(attacker->gmodmodifier == 7){
	targ->targetname = "gtarget7";
}
if(attacker->gmodmodifier == 8){
	targ->targetname = "gtarget8";
}
if(attacker->gmodmodifier == 9){
	targ->targetname = "gtarget9";
}
if(attacker->gmodmodifier == 10){
	targ->targetname = "gtarget10";
}
if(attacker->gmodmodifier == 11){
	targ->targetname = "gtarget11";
}
if(attacker->gmodmodifier == 12){
	targ->targetname = "gtarget12";
}
if(attacker->gmodmodifier == 13){
	targ->targetname = "gtarget13";
}
if(attacker->gmodmodifier == 14){
	targ->targetname = "gtarget14";
}
if(attacker->gmodmodifier == 15){
	targ->targetname = "gtarget15";
}
if(attacker->gmodmodifier == 16){
	targ->targetname = "gtarget16";
}
if(attacker->gmodmodifier == 17){
	targ->targetname = "gtarget17";
}
if(attacker->gmodmodifier == 18){
	targ->targetname = "gtarget18";
}
if(attacker->gmodmodifier == 19){
	targ->targetname = "gtarget19";
}
if(attacker->gmodmodifier == 20){
	targ->targetname = "gtarget20";
}
if(attacker->gmodmodifier == 21){
	targ->targetname = "gtarget21";
}
if(attacker->gmodmodifier == 22){
	targ->targetname = "gtarget22";
}
if(attacker->gmodmodifier == 23){
	targ->targetname = "gtarget23";
}
if(attacker->gmodmodifier == 24){
	targ->targetname = "gtarget24";
}
if(attacker->gmodmodifier == 25){
	targ->targetname = "gtarget25";
}
if(attacker->gmodmodifier == 26){
	targ->targetname = "gtarget26";
}
if(attacker->gmodmodifier == 27){
	targ->targetname = "gtarget27";
}
if(attacker->gmodmodifier == 28){
	targ->targetname = "gtarget28";
}
if(attacker->gmodmodifier == 29){
	targ->targetname = "gtarget29";
}
if(attacker->gmodmodifier == 30){
	targ->targetname = "gtarget30";
}
if(attacker->gmodmodifier == 31){
	targ->targetname = "gtarget31";
}
if(attacker->gmodmodifier == 32){
	targ->targetname = "gtarget32";
}
if(attacker->gmodmodifier == 33){
	targ->targetname = "gtarget33";
}
if(attacker->gmodmodifier == 34){
	targ->targetname = "gtarget34";
}
if(attacker->gmodmodifier == 35){
	targ->targetname = "gtarget35";
}
if(attacker->gmodmodifier == 36){
	targ->targetname = "gtarget36";
}
if(attacker->gmodmodifier == 37){
	targ->targetname = "gtarget37";
}
if(attacker->gmodmodifier == 38){
	targ->targetname = "gtarget38";
}
if(attacker->gmodmodifier == 39){
	targ->targetname = "gtarget39";
}
if(attacker->gmodmodifier == 40){
	targ->targetname = "gtarget40";
}
if(attacker->gmodmodifier == 41){
	targ->targetname = "gtarget41";
}
if(attacker->gmodmodifier == 42){
	targ->targetname = "gtarget42";
}
if(attacker->gmodmodifier == 43){
	targ->targetname = "gtarget43";
}
if(attacker->gmodmodifier == 44){
	targ->targetname = "gtarget44";
}
if(attacker->gmodmodifier == 45){
	targ->targetname = "gtarget45";
}
if(attacker->gmodmodifier == 46){
	targ->targetname = "gtarget46";
}
if(attacker->gmodmodifier == 47){
	targ->targetname = "gtarget47";
}
if(attacker->gmodmodifier == 48){
	targ->targetname = "gtarget48";
}
if(attacker->gmodmodifier == 49){
	targ->targetname = "gtarget49";
}
if(attacker->gmodmodifier == 50){
	targ->targetname = "gtarget50";
}
if(attacker->gmodmodifier == 51){
	targ->targetname = "gtarget51";
}
if(attacker->gmodmodifier == 52){
	targ->targetname = "gtarget52";
}
if(attacker->gmodmodifier == 53){
	targ->targetname = "gtarget53";
}
if(attacker->gmodmodifier == 54){
	targ->targetname = "gtarget54";
}
if(attacker->gmodmodifier == 55){
	targ->targetname = "gtarget55";
}
if(attacker->gmodmodifier == 56){
	targ->targetname = "gtarget56";
}
if(attacker->gmodmodifier == 57){
	targ->targetname = "gtarget57";
}
if(attacker->gmodmodifier == 58){
	targ->targetname = "gtarget58";
}
if(attacker->gmodmodifier == 59){
	targ->targetname = "gtarget59";
}
if(attacker->gmodmodifier == 60){
	targ->targetname = "gtarget60";
}
if(attacker->gmodmodifier == 61){
	targ->targetname = "gtarget61";
}
if(attacker->gmodmodifier == 62){
	targ->targetname = "gtarget62";
}
if(attacker->gmodmodifier == 63){
	targ->targetname = "gtarget63";
}
if(attacker->gmodmodifier == 64){
	targ->targetname = "gtarget64";
}
if(attacker->gmodmodifier == 65){
	targ->targetname = "gtarget65";
}
if(attacker->gmodmodifier == 66){
	targ->targetname = "gtarget66";
}
if(attacker->gmodmodifier == 67){
	targ->targetname = "gtarget67";
}
if(attacker->gmodmodifier == 68){
	targ->targetname = "gtarget68";
}
if(attacker->gmodmodifier == 69){
	targ->targetname = "gtarget69";
}
if(attacker->gmodmodifier == 70){
	targ->targetname = "gtarget70";
}
if(attacker->gmodmodifier == 71){
	targ->targetname = "gtarget71";
}
if(attacker->gmodmodifier == 72){
	targ->targetname = "gtarget72";
}
if(attacker->gmodmodifier == 73){
	targ->targetname = "gtarget73";
}
if(attacker->gmodmodifier == 74){
	targ->targetname = "gtarget74";
}
if(attacker->gmodmodifier == 75){
	targ->targetname = "gtarget75";
}
if(attacker->gmodmodifier == 76){
	targ->targetname = "gtarget76";
}
if(attacker->gmodmodifier == 77){
	targ->targetname = "gtarget77";
}
if(attacker->gmodmodifier == 78){
	targ->targetname = "gtarget78";
}
if(attacker->gmodmodifier == 79){
	targ->targetname = "gtarget79";
}
if(attacker->gmodmodifier == 80){
	targ->targetname = "gtarget80";
}
if(attacker->gmodmodifier == 81){
	targ->targetname = "gtarget81";
}
if(attacker->gmodmodifier == 82){
	targ->targetname = "gtarget82";
}
if(attacker->gmodmodifier == 83){
	targ->targetname = "gtarget83";
}
if(attacker->gmodmodifier == 84){
	targ->targetname = "gtarget84";
}
if(attacker->gmodmodifier == 85){
	targ->targetname = "gtarget85";
}
if(attacker->gmodmodifier == 86){
	targ->targetname = "gtarget86";
}
if(attacker->gmodmodifier == 87){
	targ->targetname = "gtarget87";
}
if(attacker->gmodmodifier == 88){
	targ->targetname = "gtarget88";
}
if(attacker->gmodmodifier == 89){
	targ->targetname = "gtarget89";
}
if(attacker->gmodmodifier == 90){
	targ->targetname = "gtarget90";
}
if(attacker->gmodmodifier == 91){
	targ->targetname = "gtarget91";
}
if(attacker->gmodmodifier == 92){
	targ->targetname = "gtarget92";
}
if(attacker->gmodmodifier == 93){
	targ->targetname = "gtarget93";
}
if(attacker->gmodmodifier == 94){
	targ->targetname = "gtarget94";
}
if(attacker->gmodmodifier == 95){
	targ->targetname = "gtarget95";
}
if(attacker->gmodmodifier == 96){
	targ->targetname = "gtarget96";
}
if(attacker->gmodmodifier == 97){
	targ->targetname = "gtarget97";
}
if(attacker->gmodmodifier == 98){
	targ->targetname = "gtarget98";
}
if(attacker->gmodmodifier == 99){
	targ->targetname = "gtarget99";
}
if(attacker->gmodmodifier == 100){
	targ->targetname = "gtarget100";
}
}
if(attacker->gmodtool == 14){
if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
	if(!targ->model){
	targ->model = "12345";
	}
}
	targ->classname = "func_door";
  SP_func_door (targ);
  targ->s.modelindex = targ->sandboxindex;
}
if(attacker->gmodtool == 15){
	if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
		if(!targ->model){
		targ->model = "12345";
		}
	}
	targ->classname = "func_button";
  SP_func_button (targ);
 targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
targ->r.contents = CONTENTS_SOLID | CONTENTS_BODY;
}
if(attacker->gmodtool == 16){
	if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
		if(!targ->model){
		targ->model = "12345";
		}
	}
	targ->classname = "func_pendulum";
  SP_func_pendulum (targ);
 targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
targ->r.contents = CONTENTS_SOLID | CONTENTS_BODY;
}
if(attacker->gmodtool == 17){
	if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
		if(!targ->model){
		targ->model = "12345";
		}
	}
	targ->classname = "func_bobbing";
  SP_func_bobbing (targ);
 targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
targ->r.contents = CONTENTS_SOLID | CONTENTS_BODY;
}
if(attacker->gmodtool == 18){
	if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
		if(!targ->model){
		targ->model = "12345";
		}
	}
	targ->classname = "func_rotating";
  SP_func_rotating (targ);
 targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
targ->r.contents = CONTENTS_SOLID | CONTENTS_BODY;
}
if(attacker->gmodtool == 19){
	if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
		if(!targ->model){
		targ->model = "12345";
		}
	}
	targ->classname = "func_static";
  SP_func_static (targ);
 targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
targ->r.contents = CONTENTS_SOLID | CONTENTS_BODY;
}
if(attacker->gmodtool == 20){
	if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
		if(!targ->model){
		targ->model = "12345";
		}
	}
	targ->classname = "func_plat";
  SP_func_plat (targ);
 targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
targ->r.contents = CONTENTS_SOLID | CONTENTS_BODY;
}
if(attacker->gmodtool == 21){
	targ->height = attacker->gmodmodifier;
}
if(attacker->gmodtool == 22){
	targ->random = attacker->gmodmodifier;
}
if(attacker->gmodtool == 23){
  VectorScale (dir, attacker->gmodmodifier, kvels);
  VectorAdd (targ->s.pos.trDelta, kvels, targ->s.pos.trDelta);
}
if(attacker->gmodtool == 24){
	targ->s.eFlags = 0;
}
if(attacker->gmodtool == 25){
	targ->s.eFlags = EF_BOUNCE_HALF;
}
if(attacker->gmodtool == 26){
	targ->s.eFlags = EF_BOUNCE;
}
if(attacker->gmodtool == 27){
	targ->classname = "shooter_rocket";
  SP_shooter_rocket (targ);
}
if(attacker->gmodtool == 28){
	targ->classname = "shooter_grenade";
  SP_shooter_grenade (targ);
}
if(attacker->gmodtool == 29){
	targ->classname = "shooter_plasma";
  SP_shooter_plasma (targ);
}
if(attacker->gmodtool == 30){
	targ->classname = "target_remove_powerups";
  SP_target_remove_powerups (targ);
}
if(attacker->gmodtool == 31){
	targ->classname = "target_score";
  SP_target_score (targ);
}
if(attacker->gmodtool == 32){
	targ->classname = "target_sound";
  SP_target_sound (targ);
}
if(attacker->gmodtool == 33){
	targ->classname = "target_music";
  SP_target_music (targ);
}
if(attacker->gmodtool == 34){
	targ->classname = "target_cmd";
  SP_target_cmd (targ);
}
if(attacker->gmodtool == 35){
	targ->classname = "target_model";
  SP_target_model (targ);
}
if(attacker->gmodtool == 36){
	targ->classname = "target_legs";
  SP_target_legs (targ);
}
if(attacker->gmodtool == 37){
	targ->classname = "target_head";
  SP_target_head (targ);
}
if(attacker->gmodtool == 38){
	targ->classname = "target_teleporter";
  SP_target_teleporter (targ);
}
if(attacker->gmodtool == 39){
	targ->classname = "target_kill";
  SP_target_kill (targ);
}
if(attacker->gmodtool == 40){
	targ->classname = "target_push";
  SP_target_push (targ);
}
if(attacker->gmodtool == 41){
	targ->classname = "target_give";
  SP_target_give (targ);
}
if(attacker->gmodtool == 42){
	targ->classname = "target_print";
  SP_target_print (targ);
}
if(attacker->gmodtool == 43){
	targ->classname = "misc_teleporter_dest";
  SP_misc_teleporter_dest (targ);
}
if(attacker->gmodtool == 44){
	targ->classname = "info_player_deathmatch";
  SP_info_player_deathmatch (targ);
}
if(attacker->gmodtool == 45){
	targ->classname = "team_CTF_redspawn";
  SP_team_CTF_redspawn (targ);
}
if(attacker->gmodtool == 46){
	targ->classname = "team_CTF_bluespawn";
  SP_team_CTF_bluespawn (targ);
}
if(attacker->gmodtool == 47){
	targ->classname = "team_redobelisk";
  SP_team_redobelisk (targ);
}
if(attacker->gmodtool == 48){
	targ->classname = "team_blueobelisk";
  SP_team_blueobelisk (targ);
}
if(attacker->gmodtool == 49){
	targ->classname = "team_neutralobelisk";
  SP_team_neutralobelisk (targ);
}
if(attacker->gmodtool == 50){
	targ->classname = "domination_point";
  SP_domination_point (targ);
}
if(attacker->gmodtool == 51){
	targ->message = attacker->gmodmodifiers;
}
if(attacker->gmodtool == 52){
	targ->sandcol = 1;
	targ->r.contents = CONTENTS_SOLID;
//	G_CallSpawn( targ );
	targ->s.modelindex = targ->sandboxindex;
	trap_LinkEntity( targ );
}
if(attacker->gmodtool == 53){
	targ->sandcol = 2;
	targ->r.contents = CONTENTS_WATER;
//	G_CallSpawn( targ );
	targ->s.modelindex = targ->sandboxindex;
	trap_LinkEntity( targ );
}
if(attacker->gmodtool == 54){
	G_CallSpawn( targ );
	targ->s.modelindex = targ->sandboxindex;
}
if(attacker->gmodtool == 55){
	VectorCopy( targ->s.origin, snaporigin );
	VectorCopy( targ->r.currentOrigin, snapcorigin );
	VectorCopy( targ->s.pos.trBase, snaptrbase );
	SnapVector( snaporigin );
	SnapVector( snapcorigin );
	SnapVector( snaptrbase );
	VectorSet( addd, attacker->gmodmodifier, 0, 0 );
	VectorAdd (snaporigin, addd, snaporigin);
	VectorAdd (snapcorigin, addd, snapcorigin);
	VectorAdd (snaptrbase, addd, snaptrbase);
	VectorCopy( snaporigin, targ->s.origin );
	VectorCopy( snapcorigin, targ->r.currentOrigin );
	VectorCopy( snaptrbase, targ->s.pos.trBase );
	G_SetOrigin( targ, snaptrbase );
	G_CallSpawn( targ );
	targ->s.modelindex = targ->sandboxindex;
		trap_LinkEntity( targ );
}
if(attacker->gmodtool == 56){
	VectorCopy( targ->s.origin, snaporigin );
	VectorCopy( targ->r.currentOrigin, snapcorigin );
	VectorCopy( targ->s.pos.trBase, snaptrbase );
	SnapVector( snaporigin );
	SnapVector( snapcorigin );
	SnapVector( snaptrbase );
	VectorSet( addd, 0, attacker->gmodmodifier, 0 );
	VectorAdd (snaporigin, addd, snaporigin);
	VectorAdd (snapcorigin, addd, snapcorigin);
	VectorAdd (snaptrbase, addd, snaptrbase);
	VectorCopy( snaporigin, targ->s.origin );
	VectorCopy( snapcorigin, targ->r.currentOrigin );
	VectorCopy( snaptrbase, targ->s.pos.trBase );
	G_SetOrigin( targ, snaptrbase );
	G_CallSpawn( targ );
	targ->s.modelindex = targ->sandboxindex;
		trap_LinkEntity( targ );
}
if(attacker->gmodtool == 57){
	VectorCopy( targ->s.origin, snaporigin );
	VectorCopy( targ->r.currentOrigin, snapcorigin );
	VectorCopy( targ->s.pos.trBase, snaptrbase );
	SnapVector( snaporigin );
	SnapVector( snapcorigin );
	SnapVector( snaptrbase );
	VectorSet( addd, 0, 0, attacker->gmodmodifier );
	VectorAdd (snaporigin, addd, snaporigin);
	VectorAdd (snapcorigin, addd, snapcorigin);
	VectorAdd (snaptrbase, addd, snaptrbase);
	VectorCopy( snaporigin, targ->s.origin );
	VectorCopy( snapcorigin, targ->r.currentOrigin );
	VectorCopy( snaptrbase, targ->s.pos.trBase );
	G_SetOrigin( targ, snaptrbase );
	G_CallSpawn( targ );
	targ->s.modelindex = targ->sandboxindex;
		trap_LinkEntity( targ );
}
if(attacker->gmodtool == 58){
targ->model = "1";
targ->classname = attacker->gmodmodifiers;
G_CallSpawn( targ );
	trap_LinkEntity( targ );
}
if(attacker->gmodtool == 59){
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", targ->classname ));
}
if(attacker->gmodtool == 60){
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", targ->target ));
}
if(attacker->gmodtool == 61){
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", targ->targetname ));
}
if(attacker->gmodtool == 62){
targ->classname = "target_position";
SP_target_position (targ);
}
if(attacker->gmodtool == 63){
	targ->model = "1";
if(attacker->gmodmodifier == 1){
	targ->model = "1";
}
if(attacker->gmodmodifier == 2){
	targ->model = "2";
}
if(attacker->gmodmodifier == 3){
	targ->model = "3";
}
if(attacker->gmodmodifier == 4){
	targ->model = "4";
}
if(attacker->gmodmodifier == 5){
	targ->model = "5";
}
if(attacker->gmodmodifier == 6){
	targ->model = "6";
}
if(attacker->gmodmodifier == 7){
	targ->model = "7";
}
if(attacker->gmodmodifier == 8){
	targ->model = "8";
}
if(attacker->gmodmodifier == 9){
	targ->model = "9";
}
targ->classname = "trigger_multiple";
SP_trigger_multiple (targ);
targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
}
if(attacker->gmodtool == 64){
targ->model = "1";
if(attacker->gmodmodifier == 1){
	targ->model = "1";
}
if(attacker->gmodmodifier == 2){
	targ->model = "2";
}
if(attacker->gmodmodifier == 3){
	targ->model = "3";
}
if(attacker->gmodmodifier == 4){
	targ->model = "4";
}
if(attacker->gmodmodifier == 5){
	targ->model = "5";
}
if(attacker->gmodmodifier == 6){
	targ->model = "6";
}
if(attacker->gmodmodifier == 7){
	targ->model = "7";
}
if(attacker->gmodmodifier == 8){
	targ->model = "8";
}
if(attacker->gmodmodifier == 9){
	targ->model = "9";
}
targ->classname = "trigger_push";
SP_trigger_push (targ);
targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
}
if(attacker->gmodtool == 65){
targ->model = "1";
if(attacker->gmodmodifier == 1){
	targ->model = "1";
}
if(attacker->gmodmodifier == 2){
	targ->model = "2";
}
if(attacker->gmodmodifier == 3){
	targ->model = "3";
}
if(attacker->gmodmodifier == 4){
	targ->model = "4";
}
if(attacker->gmodmodifier == 5){
	targ->model = "5";
}
if(attacker->gmodmodifier == 6){
	targ->model = "6";
}
if(attacker->gmodmodifier == 7){
	targ->model = "7";
}
if(attacker->gmodmodifier == 8){
	targ->model = "8";
}
if(attacker->gmodmodifier == 9){
	targ->model = "9";
}
targ->classname = "trigger_teleport";
SP_trigger_teleport (targ);
targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
}
if(attacker->gmodtool == 66){
targ->model = "1";
if(attacker->gmodmodifier == 1){
	targ->model = "1";
}
if(attacker->gmodmodifier == 2){
	targ->model = "2";
}
if(attacker->gmodmodifier == 3){
	targ->model = "3";
}
if(attacker->gmodmodifier == 4){
	targ->model = "4";
}
if(attacker->gmodmodifier == 5){
	targ->model = "5";
}
if(attacker->gmodmodifier == 6){
	targ->model = "6";
}
if(attacker->gmodmodifier == 7){
	targ->model = "7";
}
if(attacker->gmodmodifier == 8){
	targ->model = "8";
}
if(attacker->gmodmodifier == 9){
	targ->model = "9";
}
targ->classname = "trigger_hurt";
SP_trigger_hurt (targ);
targ->s.modelindex = targ->sandboxindex;
VectorSet( targ->r.mins, -25, -25, -25);
VectorSet( targ->r.maxs, 25, 25, 25 );
}
if(attacker->gmodtool == 67){
if(attacker->gmodmodifier == 1){
	VectorSet( targ->r.mins, -5, -5, -5);
	VectorSet( targ->r.maxs, 5, 5, 5 );
}
if(attacker->gmodmodifier == 2){
	VectorSet( targ->r.mins, -10, -10, -10);
	VectorSet( targ->r.maxs, 10, 10, 10 );
}
if(attacker->gmodmodifier == 3){
	VectorSet( targ->r.mins, -15, -15, -15);
	VectorSet( targ->r.maxs, 15, 15, 15 );
}
if(attacker->gmodmodifier == 4){
	VectorSet( targ->r.mins, -20, -20, -20);
	VectorSet( targ->r.maxs, 20, 20, 20 );
}
if(attacker->gmodmodifier == 5){
	VectorSet( targ->r.mins, -25, -25, -25);
	VectorSet( targ->r.maxs, 25, 25, 25 );
}
if(attacker->gmodmodifier == 6){
	VectorSet( targ->r.mins, -50, -50, -50);
	VectorSet( targ->r.maxs, 50, 50, 50 );
}
if(attacker->gmodmodifier == 7){
	VectorSet( targ->r.mins, -75, -75, -75);
	VectorSet( targ->r.maxs, 75, 75, 75 );
}
if(attacker->gmodmodifier == 8){
	VectorSet( targ->r.mins, -100, -100, -100);
	VectorSet( targ->r.maxs, 100, 100, 100 );
}
if(attacker->gmodmodifier == 9){
	VectorSet( targ->r.mins, -200, -200, -200);
	VectorSet( targ->r.maxs, 200, 200, 200 );
}
if(attacker->gmodmodifier == 10){
	VectorSet( targ->r.mins, -250, -250, -250);
	VectorSet( targ->r.maxs, 250, 250, 250 );
}
if(attacker->gmodmodifier == 11){
	VectorSet( targ->r.mins, -500, -500, -500);
	VectorSet( targ->r.maxs, 500, 500, 500 );
}
}
if(attacker->gmodtool == 68){
if(attacker->gmodmodifier == 1){
	targ->targetname = "gtarget1";
}
if(attacker->gmodmodifier == 2){
	targ->targetname = "gtarget2";
}
if(attacker->gmodmodifier == 3){
	targ->targetname = "gtarget3";
}
if(attacker->gmodmodifier == 4){
	targ->targetname = "gtarget4";
}
if(attacker->gmodmodifier == 5){
	targ->targetname = "gtarget5";
}
if(attacker->gmodmodifier == 6){
	targ->targetname = "gtarget6";
}
if(attacker->gmodmodifier == 7){
	targ->targetname = "gtarget7";
}
if(attacker->gmodmodifier == 8){
	targ->targetname = "gtarget8";
}
if(attacker->gmodmodifier == 9){
	targ->targetname = "gtarget9";
}
if(attacker->gmodmodifier == 10){
	targ->targetname = "gtarget10";
}
if(attacker->gmodmodifier == 11){
	targ->targetname = "gtarget11";
}
if(attacker->gmodmodifier == 12){
	targ->targetname = "gtarget12";
}
if(attacker->gmodmodifier == 13){
	targ->targetname = "gtarget13";
}
if(attacker->gmodmodifier == 14){
	targ->targetname = "gtarget14";
}
if(attacker->gmodmodifier == 15){
	targ->targetname = "gtarget15";
}
if(attacker->gmodmodifier == 16){
	targ->targetname = "gtarget16";
}
if(attacker->gmodmodifier == 17){
	targ->targetname = "gtarget17";
}
if(attacker->gmodmodifier == 18){
	targ->targetname = "gtarget18";
}
if(attacker->gmodmodifier == 19){
	targ->targetname = "gtarget19";
}
if(attacker->gmodmodifier == 20){
	targ->targetname = "gtarget20";
}
if(attacker->gmodmodifier == 21){
	targ->targetname = "gtarget21";
}
if(attacker->gmodmodifier == 22){
	targ->targetname = "gtarget22";
}
if(attacker->gmodmodifier == 23){
	targ->targetname = "gtarget23";
}
if(attacker->gmodmodifier == 24){
	targ->targetname = "gtarget24";
}
if(attacker->gmodmodifier == 25){
	targ->targetname = "gtarget25";
}
if(attacker->gmodmodifier == 26){
	targ->targetname = "gtarget26";
}
if(attacker->gmodmodifier == 27){
	targ->targetname = "gtarget27";
}
if(attacker->gmodmodifier == 28){
	targ->targetname = "gtarget28";
}
if(attacker->gmodmodifier == 29){
	targ->targetname = "gtarget29";
}
if(attacker->gmodmodifier == 30){
	targ->targetname = "gtarget30";
}
if(attacker->gmodmodifier == 31){
	targ->targetname = "gtarget31";
}
if(attacker->gmodmodifier == 32){
	targ->targetname = "gtarget32";
}
if(attacker->gmodmodifier == 33){
	targ->targetname = "gtarget33";
}
if(attacker->gmodmodifier == 34){
	targ->targetname = "gtarget34";
}
if(attacker->gmodmodifier == 35){
	targ->targetname = "gtarget35";
}
if(attacker->gmodmodifier == 36){
	targ->targetname = "gtarget36";
}
if(attacker->gmodmodifier == 37){
	targ->targetname = "gtarget37";
}
if(attacker->gmodmodifier == 38){
	targ->targetname = "gtarget38";
}
if(attacker->gmodmodifier == 39){
	targ->targetname = "gtarget39";
}
if(attacker->gmodmodifier == 40){
	targ->targetname = "gtarget40";
}
if(attacker->gmodmodifier == 41){
	targ->targetname = "gtarget41";
}
if(attacker->gmodmodifier == 42){
	targ->targetname = "gtarget42";
}
if(attacker->gmodmodifier == 43){
	targ->targetname = "gtarget43";
}
if(attacker->gmodmodifier == 44){
	targ->targetname = "gtarget44";
}
if(attacker->gmodmodifier == 45){
	targ->targetname = "gtarget45";
}
if(attacker->gmodmodifier == 46){
	targ->targetname = "gtarget46";
}
if(attacker->gmodmodifier == 47){
	targ->targetname = "gtarget47";
}
if(attacker->gmodmodifier == 48){
	targ->targetname = "gtarget48";
}
if(attacker->gmodmodifier == 49){
	targ->targetname = "gtarget49";
}
if(attacker->gmodmodifier == 50){
	targ->targetname = "gtarget50";
}
if(attacker->gmodmodifier == 51){
	targ->targetname = "gtarget51";
}
if(attacker->gmodmodifier == 52){
	targ->targetname = "gtarget52";
}
if(attacker->gmodmodifier == 53){
	targ->targetname = "gtarget53";
}
if(attacker->gmodmodifier == 54){
	targ->targetname = "gtarget54";
}
if(attacker->gmodmodifier == 55){
	targ->targetname = "gtarget55";
}
if(attacker->gmodmodifier == 56){
	targ->targetname = "gtarget56";
}
if(attacker->gmodmodifier == 57){
	targ->targetname = "gtarget57";
}
if(attacker->gmodmodifier == 58){
	targ->targetname = "gtarget58";
}
if(attacker->gmodmodifier == 59){
	targ->targetname = "gtarget59";
}
if(attacker->gmodmodifier == 60){
	targ->targetname = "gtarget60";
}
if(attacker->gmodmodifier == 61){
	targ->targetname = "gtarget61";
}
if(attacker->gmodmodifier == 62){
	targ->targetname = "gtarget62";
}
if(attacker->gmodmodifier == 63){
	targ->targetname = "gtarget63";
}
if(attacker->gmodmodifier == 64){
	targ->targetname = "gtarget64";
}
if(attacker->gmodmodifier == 65){
	targ->targetname = "gtarget65";
}
if(attacker->gmodmodifier == 66){
	targ->targetname = "gtarget66";
}
if(attacker->gmodmodifier == 67){
	targ->targetname = "gtarget67";
}
if(attacker->gmodmodifier == 68){
	targ->targetname = "gtarget68";
}
if(attacker->gmodmodifier == 69){
	targ->targetname = "gtarget69";
}
if(attacker->gmodmodifier == 70){
	targ->targetname = "gtarget70";
}
if(attacker->gmodmodifier == 71){
	targ->targetname = "gtarget71";
}
if(attacker->gmodmodifier == 72){
	targ->targetname = "gtarget72";
}
if(attacker->gmodmodifier == 73){
	targ->targetname = "gtarget73";
}
if(attacker->gmodmodifier == 74){
	targ->targetname = "gtarget74";
}
if(attacker->gmodmodifier == 75){
	targ->targetname = "gtarget75";
}
if(attacker->gmodmodifier == 76){
	targ->targetname = "gtarget76";
}
if(attacker->gmodmodifier == 77){
	targ->targetname = "gtarget77";
}
if(attacker->gmodmodifier == 78){
	targ->targetname = "gtarget78";
}
if(attacker->gmodmodifier == 79){
	targ->targetname = "gtarget79";
}
if(attacker->gmodmodifier == 80){
	targ->targetname = "gtarget80";
}
if(attacker->gmodmodifier == 81){
	targ->targetname = "gtarget81";
}
if(attacker->gmodmodifier == 82){
	targ->targetname = "gtarget82";
}
if(attacker->gmodmodifier == 83){
	targ->targetname = "gtarget83";
}
if(attacker->gmodmodifier == 84){
	targ->targetname = "gtarget84";
}
if(attacker->gmodmodifier == 85){
	targ->targetname = "gtarget85";
}
if(attacker->gmodmodifier == 86){
	targ->targetname = "gtarget86";
}
if(attacker->gmodmodifier == 87){
	targ->targetname = "gtarget87";
}
if(attacker->gmodmodifier == 88){
	targ->targetname = "gtarget88";
}
if(attacker->gmodmodifier == 89){
	targ->targetname = "gtarget89";
}
if(attacker->gmodmodifier == 90){
	targ->targetname = "gtarget90";
}
if(attacker->gmodmodifier == 91){
	targ->targetname = "gtarget91";
}
if(attacker->gmodmodifier == 92){
	targ->targetname = "gtarget92";
}
if(attacker->gmodmodifier == 93){
	targ->targetname = "gtarget93";
}
if(attacker->gmodmodifier == 94){
	targ->targetname = "gtarget94";
}
if(attacker->gmodmodifier == 95){
	targ->targetname = "gtarget95";
}
if(attacker->gmodmodifier == 96){
	targ->targetname = "gtarget96";
}
if(attacker->gmodmodifier == 97){
	targ->targetname = "gtarget97";
}
if(attacker->gmodmodifier == 98){
	targ->targetname = "gtarget98";
}
if(attacker->gmodmodifier == 99){
	targ->targetname = "gtarget99";
}
if(attacker->gmodmodifier == 100){
	targ->targetname = "gtarget100";
}
}
if(attacker->gmodtool == 70){
targ->s.generic1 = attacker->gmodmodifier;
}
if(attacker->gmodtool == 71){
targ->classname = "item_armor_shard";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 72){
targ->classname = "item_armor_combat";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 73){
targ->classname = "item_armor_body";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 74){
targ->classname = "item_health_small";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 75){
targ->classname = "item_health";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 76){
targ->classname = "item_health_large";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 77){
targ->classname = "item_health_mega";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 78){
targ->classname = "weapon_shotgun";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 79){
targ->classname = "weapon_machinegun";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 80){
targ->classname = "weapon_grenadelauncher";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 81){
targ->classname = "weapon_rocketlauncher";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 82){
targ->classname = "weapon_lightning";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 83){
targ->classname = "weapon_railgun";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 84){
targ->classname = "weapon_plasmagun";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 85){
targ->classname = "weapon_bfg";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 86){
targ->classname = "weapon_grapplinghook";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 87){
targ->classname = "ammo_shells";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 88){
targ->classname = "ammo_bullets";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 89){
targ->classname = "ammo_grenades";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 90){
targ->classname = "ammo_cells";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 91){
targ->classname = "ammo_lightning";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 92){
targ->classname = "ammo_rockets";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 93){
targ->classname = "ammo_slugs";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 94){
targ->classname = "ammo_bfg";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 95){
targ->classname = "holdable_teleporter";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 96){
targ->classname = "holdable_medkit";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 97){
targ->classname = "item_quad";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 98){
targ->classname = "item_enviro";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 99){
targ->classname = "item_haste";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 100){
targ->classname = "item_invis";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 101){
targ->classname = "item_regen";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 102){
targ->classname = "item_flight";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 103){
targ->classname = "team_CTF_redflag";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 104){
targ->classname = "team_CTF_blueflag";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 105){
targ->classname = "holdable_kamikaze";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 106){
targ->classname = "holdable_portal";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 107){
targ->classname = "holdable_invulnerability";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 108){
targ->classname = "ammo_nails";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 109){
targ->classname = "ammo_mines";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 110){
targ->classname = "ammo_belt";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 111){
targ->classname = "item_scout";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 112){
targ->classname = "item_guard";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 113){
targ->classname = "item_doubler";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 114){
targ->classname = "item_ammoregen";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 115){
targ->classname = "team_CTF_neutralflag";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 116){
targ->classname = "weapon_nailgun";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 117){
targ->classname = "weapon_prox_launcher";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 118){
targ->classname = "weapon_chaingun";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 119){
targ->classname = "weapon_flamethrower";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 120){
targ->classname = "ammo_flame";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 121){
targ->classname = "weapon_antimatter";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 122){
targ->classname = "team_DD_pointAwhite";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 123){
targ->classname = "team_DD_pointBwhite";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 124){
targ->classname = "team_dom_pointWhite";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 125){
targ->s.loopSound = G_SoundIndex(attacker->gmodmodifiers);
}
if(attacker->gmodtool == 126){
targ->s.modelindex = G_ModelIndex( targ->sandmodel );
}
if(attacker->gmodtool == 127){
	if(!targ->owner){
targ->owner = attacker->s.clientNum + 1;
targ->ownername = attacker->client->pers.netname;
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Private" ));
return;
	}
	if(targ->owner){
targ->owner = 0;
targ->ownername = 0;
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Public" ));
return;
	}
}
if(attacker->gmodtool == 128){
	if(targ->owner){
trap_SendServerCommand( attacker->s.clientNum, va( "cp Owned_by_%s\n", targ->ownername ));
	}
	if(!targ->owner){
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Not_owned" ));
	}
}
if(attacker->gmodtool == 129){
	if(!targ->locked){
targ->locked = 1;
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Prop_locked" ));
return;
	}
	if(targ->locked){
targ->locked = 0;
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Prop_unlocked" ));
return;
	}
}
if(attacker->gmodtool == 130){
	VectorCopy( attacker->s.pos.trBase, attacker->home1 );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point1_set" ));
}
if(attacker->gmodtool == 131){
	VectorCopy( attacker->s.pos.trBase, attacker->home2 );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point2_set" ));
}
if(attacker->gmodtool == 132){
	VectorCopy( attacker->s.pos.trBase, attacker->home3 );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point3_set" ));
}
if(attacker->gmodtool == 133){
	anglesis[0] = 0;
	anglesis[1] = 0;
	anglesis[2] = 0;
	
	if( !targ->client ) {
		return;
	}
	
	TeleportPlayer( targ, attacker->home1, anglesis );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point1_teleport" ));
}
if(attacker->gmodtool == 134){
	anglesis[0] = 0;
	anglesis[1] = 0;
	anglesis[2] = 0;
	
	if( targ->health <= 0 ) {
		return;
	}
	if( !targ->client ) {
		return;
	}
	
	TeleportPlayer( targ, attacker->home2, anglesis );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point2_teleport" ));
}
if(attacker->gmodtool == 135){
	anglesis[0] = 0;
	anglesis[1] = 0;
	anglesis[2] = 0;
	
	if( targ->health <= 0 ) {
		return;
	}
	if( !targ->client ) {
		return;
	}
	
	TeleportPlayer( targ, attacker->home3, anglesis );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point3_teleport" ));
}
if(attacker->gmodtool == 136){
	anglesis[0] = 0;
	anglesis[1] = 0;
	anglesis[2] = 0;
	
	TeleportPlayer( attacker, attacker->home1, anglesis );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point1_teleport" ));
}
if(attacker->gmodtool == 137){
	anglesis[0] = 0;
	anglesis[1] = 0;
	anglesis[2] = 0;
	
	TeleportPlayer( attacker, attacker->home2, anglesis );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point2_teleport" ));
}
if(attacker->gmodtool == 138){
	anglesis[0] = 0;
	anglesis[1] = 0;
	anglesis[2] = 0;
	
	TeleportPlayer( attacker, attacker->home3, anglesis );
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Point3_teleport" ));
}
if(attacker->gmodtool == 139){
	if( !targ->client ) {
		return;
	}
	if(targ->client->ps.stats[STAT_HOLDABLE_ITEM] != 0){
	   attacker->client->ps.stats[STAT_HOLDABLE_ITEM] = targ->client->ps.stats[STAT_HOLDABLE_ITEM];
	}
		targ->client->ps.stats[STAT_HOLDABLE_ITEM] = 0;
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Item_grabed" ));
}
if(attacker->gmodtool == 140){
	if( !targ->client ) {
		return;
	}
	if(targ->client->ps.stats[STAT_HOLDABLE_ITEM] != 0){
	   attacker->client->ps.stats[STAT_HOLDABLE_ITEM] = targ->client->ps.stats[STAT_HOLDABLE_ITEM];
	}
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_MACHINEGUN );
		targ->client->ps.ammo[WP_MACHINEGUN] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_SHOTGUN );
		targ->client->ps.ammo[WP_SHOTGUN] = 0;	
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_GRENADE_LAUNCHER );
		targ->client->ps.ammo[WP_GRENADE_LAUNCHER] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_ROCKET_LAUNCHER );
		targ->client->ps.ammo[WP_ROCKET_LAUNCHER] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_LIGHTNING );
		targ->client->ps.ammo[WP_LIGHTNING] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_RAILGUN );
		targ->client->ps.ammo[WP_RAILGUN] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_PLASMAGUN );
		targ->client->ps.ammo[WP_PLASMAGUN] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_BFG );
		targ->client->ps.ammo[WP_BFG] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_GRAPPLING_HOOK );
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_NAILGUN );
		targ->client->ps.ammo[WP_NAILGUN] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_PROX_LAUNCHER );
		targ->client->ps.ammo[WP_PROX_LAUNCHER] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_CHAINGUN );
		targ->client->ps.ammo[WP_CHAINGUN] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_FLAMETHROWER );
		targ->client->ps.ammo[WP_FLAMETHROWER] = 0;
		targ->client->ps.stats[STAT_WEAPONS] &= ~( 1 << WP_ANTIMATTER );
		targ->client->ps.ammo[WP_ANTIMATTER] = 0;
		targ->client->ps.stats[STAT_HOLDABLE_ITEM] = 0;
	trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Items_grabed" ));
}
if(attacker->gmodtool == 141){
trap_SendServerCommand( attacker->s.clientNum, va( "cp %s\n", "Target_set" ));
if(attacker->gmodmodifier == 1){
	attacker->targetname = "gtarget1";
}
if(attacker->gmodmodifier == 2){
	attacker->targetname = "gtarget2";
}
if(attacker->gmodmodifier == 3){
	attacker->targetname = "gtarget3";
}
if(attacker->gmodmodifier == 4){
	attacker->targetname = "gtarget4";
}
if(attacker->gmodmodifier == 5){
	attacker->targetname = "gtarget5";
}
if(attacker->gmodmodifier == 6){
	attacker->targetname = "gtarget6";
}
if(attacker->gmodmodifier == 7){
	attacker->targetname = "gtarget7";
}
if(attacker->gmodmodifier == 8){
	attacker->targetname = "gtarget8";
}
if(attacker->gmodmodifier == 9){
	attacker->targetname = "gtarget9";
}
if(attacker->gmodmodifier == 10){
	attacker->targetname = "gtarget10";
}
if(attacker->gmodmodifier == 11){
	attacker->targetname = "gtarget11";
}
if(attacker->gmodmodifier == 12){
	attacker->targetname = "gtarget12";
}
if(attacker->gmodmodifier == 13){
	attacker->targetname = "gtarget13";
}
if(attacker->gmodmodifier == 14){
	attacker->targetname = "gtarget14";
}
if(attacker->gmodmodifier == 15){
	attacker->targetname = "gtarget15";
}
if(attacker->gmodmodifier == 16){
	attacker->targetname = "gtarget16";
}
if(attacker->gmodmodifier == 17){
	attacker->targetname = "gtarget17";
}
if(attacker->gmodmodifier == 18){
	attacker->targetname = "gtarget18";
}
if(attacker->gmodmodifier == 19){
	attacker->targetname = "gtarget19";
}
if(attacker->gmodmodifier == 20){
	attacker->targetname = "gtarget20";
}
}
if(attacker->gmodtool == 142){
if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
	if(!targ->model){
	targ->model = "12345";
	}
}
	targ->classname = "func_door_rotating";
  SP_func_door_rotating (targ);
  targ->s.modelindex = targ->sandboxindex;
}
if(attacker->gmodtool == 143){
if(Q_stricmp (targ->classname, "func_door") || Q_stricmp (targ->classname, "func_button") || Q_stricmp (targ->classname, "func_pendulum") || Q_stricmp (targ->classname, "func_bobbing") || Q_stricmp (targ->classname, "func_rotating") || Q_stricmp (targ->classname, "func_static") || Q_stricmp (targ->classname, "func_plat") || Q_stricmp (targ->classname, "func_door_rotating") || Q_stricmp (targ->classname, "func_breakable")){
	if(!targ->model){
	targ->model = "12345";
	}
}
	targ->classname = "func_breakable";
  SP_func_breakable (targ);
  targ->s.modelindex = targ->sandboxindex;
}
if(attacker->gmodtool == 144){
	targ->classname = "target_unlink";
  SP_target_unlink (targ);
}
if(attacker->gmodtool == 145){
	targ->classname = "target_logic";
  SP_target_logic (targ);
}
if(attacker->gmodtool == 146){
targ->classname = "item_armor_vest";
G_CallSpawn( targ );
}
if(attacker->gmodtool == 147){
targ->classname = "item_armor_full";
G_CallSpawn( targ );
}
	trap_LinkEntity( targ );
}
if(mod != MOD_GAUNTLET){
		if (!targ->takedamage) {
			return;
		}

		// the intermission has allready been qualified for, so don't
		// allow any extra scoring
		if ( level.intermissionQueued ) {
			return;
		}
		if ( targ->client && mod != MOD_JUICED) {
			if ( targ->client->invulnerabilityTime > level.time) {
				if ( dir && point ) {
					G_InvulnerabilityEffect( targ, dir, point, impactpoint, bouncedir );
				}
				return;
			}
		}
	        //Sago: This was moved up
	        client = targ->client;

	        //Sago: See if the client was sent flying
	        //Check if damage is by somebody who is not a player!
	        if( (!attacker || attacker->s.eType != ET_PLAYER) && client && client->lastSentFlying>-1 && ( mod==MOD_FALLING || mod==MOD_LAVA || mod==MOD_SLIME || mod==MOD_TRIGGER_HURT || mod==MOD_SUICIDE) )  {
	            if( client->lastSentFlyingTime+5000<level.time) {
	                client->lastSentFlying = -1; //More than 5 seconds, not a kill!
	            } else {
	                //G_Printf("LastSentFlying %i\n",client->lastSentFlying);
	                attacker = &g_entities[client->lastSentFlying];
	            }
	        }

		if ( !inflictor ) {
			inflictor = &g_entities[ENTITYNUM_WORLD];
		}
		if ( !attacker ) {
			attacker = &g_entities[ENTITYNUM_WORLD];
		}


		// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER) {
		if (strcmp(targ->classname, "func_breakable") && targ->use && (targ->moverState == MOVER_POS1 || targ->moverState == ROTATOR_POS1)) {
			targ->use(targ, inflictor, attacker);
			}
		if (!strcmp(targ->classname, "func_train") && targ->health > 0) {
			G_UseTargets(targ, attacker);
		} else {	// entity is a func_breakable
			if ( (targ->spawnflags & 1024) && attacker == attacker )
			return;
			if ( (targ->spawnflags & 2048) && IsBot(attacker) )
				return;
			if ( (targ->spawnflags & 4096) && strstr(attacker->classname, "shooter_") )
				return;
			
				}
	}
		if( g_gametype.integer == GT_OBELISK && CheckObeliskAttack( targ, attacker ) ) {
			return;
		}
		// reduce damage by the attacker's handicap value
		// unless they are rocket jumping
		if ( attacker->client && attacker != targ ) {
			max = attacker->client->ps.stats[STAT_MAX_HEALTH];
			if( bg_itemlist[attacker->client->ps.stats[STAT_PERSISTANT_POWERUP]].giTag == PW_GUARD ) {
				max /= 2;
			}
			damage = damage * max / 100;
		}

	        //Sago: I have moved this up
		//client = targ->client;

		if ( client ) {
			if ( client->noclip ) {
				return;
			}
		}

		if ( !dir ) {
			dflags |= DAMAGE_NO_KNOCKBACK;
		} else {
			VectorNormalize(dir);
		}

		knockback = damage;

		if ( mod == MOD_GAUNTLET )
		knockback *= g_gknockback.value;
		if ( mod == MOD_MACHINEGUN )
		knockback *= g_mgknockback.value;
		if ( mod == MOD_SHOTGUN )
		knockback *= g_sgknockback.value;
		if ( mod == MOD_GRENADE )
		knockback *= g_glknockback.value;
		if ( mod == MOD_GRENADE_SPLASH )
		knockback *= g_glknockback.value;
		if ( mod == MOD_ROCKET )
		knockback *= g_rlknockback.value;
		if ( mod == MOD_ROCKET_SPLASH )
		knockback *= g_rlknockback.value;
		if ( mod == MOD_PLASMA )
		knockback *= g_pgknockback.value;
		if ( mod == MOD_PLASMA_SPLASH )
		knockback *= g_pgknockback.value;
		if ( mod == MOD_RAILGUN )
		knockback *= g_rgknockback.value;
		if ( mod == MOD_LIGHTNING )
		knockback *= g_lgknockback.value;
		if ( mod == MOD_BFG )
		knockback *= g_bfgknockback.value;
		if ( mod == MOD_BFG_SPLASH )
		knockback *= g_bfgknockback.value;
		if ( mod == MOD_NAIL )
		knockback *= g_ngknockback.value;
		if ( mod == MOD_CHAINGUN )
		knockback *= g_cgknockback.value;
		if ( mod == MOD_PROXIMITY_MINE )
		knockback *= g_plknockback.value;
		if ( mod == MOD_FLAME )
		knockback *= g_ftknockback.value;
		if ( mod == MOD_FLAME_SPLASH )
		knockback *= g_ftknockback.value;
		if ( mod == MOD_ANTIMATTER )
		knockback *= g_amknockback.value;
		if ( mod == MOD_ANTIMATTER_SPLASH )
		knockback *= g_amknockback.value;

		if ( targ->flags & FL_NO_KNOCKBACK ) {
			knockback = 0;
		}
		if ( dflags & DAMAGE_NO_KNOCKBACK ) {
			knockback = 0;
		}

		// figure momentum add, even if the damage won't be taken
		if ( knockback && targ->physicsObject ) {
			vec3_t	kvel;
			vec3_t	svel;
			float	mass;

			mass = 200;

			VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
			if(targ->client){
			VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);
			}
			if(!targ->client){
			VectorSet( svel, 0, 0, 200 );
			VectorAdd (targ->s.pos.trDelta, svel, targ->s.pos.trDelta);
			VectorAdd (targ->s.pos.trDelta, kvel, targ->s.pos.trDelta);
			}

			// set the timer so that the other client can't cancel
			// out the movement immediately
			if ( !targ->client->ps.pm_time ) {
				int		t;

				t = knockback * 2;
				if ( t < 50 ) {
					t = 50;
				}
				if ( t > 200 ) {
					t = 200;
				}
				targ->client->ps.pm_time = t;
				targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
			}
	                //Remeber the last person to hurt the player
	                if( !g_awardpushing.integer || targ==attacker || OnSameTeam (targ, attacker)) {
	                    targ->client->lastSentFlying = -1;
	                } else {
		/*if ( pm->waterlevel <= 1 ) {
			if ( pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK) ) {
				// if getting knocked back, no friction
				if ( ! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) ) {
					control = speed < pm_stopspeed ? pm_stopspeed : speed;
					drop += control*pm_friction*pml.frametime;
				}
			}
		}*/
	                    targ->client->lastSentFlying = attacker->s.number;
	                    targ->client->lastSentFlyingTime = level.time;
	                }
		}
		
		if ( knockback && targ->client ) {
			vec3_t	kvel;
			vec3_t	svel;
			float	mass;

			mass = 200;

			VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
			if(targ->client){
			VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);
			}
			if(!targ->client){
			VectorSet( svel, 0, 0, 200 );
			VectorAdd (targ->s.pos.trDelta, svel, targ->s.pos.trDelta);
			VectorAdd (targ->s.pos.trDelta, kvel, targ->s.pos.trDelta);
			}

			// set the timer so that the other client can't cancel
			// out the movement immediately
			if ( !targ->client->ps.pm_time ) {
				int		t;

				t = knockback * 2;
				if ( t < 50 ) {
					t = 50;
				}
				if ( t > 200 ) {
					t = 200;
				}
				targ->client->ps.pm_time = t;
				targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
			}
	                //Remeber the last person to hurt the player
	                if( !g_awardpushing.integer || targ==attacker || OnSameTeam (targ, attacker)) {
	                    targ->client->lastSentFlying = -1;
	                } else {
		/*if ( pm->waterlevel <= 1 ) {
			if ( pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK) ) {
				// if getting knocked back, no friction
				if ( ! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) ) {
					control = speed < pm_stopspeed ? pm_stopspeed : speed;
					drop += control*pm_friction*pml.frametime;
				}
			}
		}*/
	                    targ->client->lastSentFlying = attacker->s.number;
	                    targ->client->lastSentFlyingTime = level.time;
	                }
		}

		// check for completely getting out of the damage
		if ( !(dflags & DAMAGE_NO_PROTECTION) ) {

			// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
			// if the attacker was on the same team
			if ( mod != MOD_JUICED && mod != MOD_CRUSH && targ != attacker && !(dflags & DAMAGE_NO_TEAM_PROTECTION) && OnSameTeam (targ, attacker)  ) {
				if ( ( !g_friendlyFire.integer && g_gametype.integer != GT_ELIMINATION && g_gametype.integer != GT_CTF_ELIMINATION ) || ( g_elimination_selfdamage.integer<2 &&	(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION) ) ) {
					return;
				}
			}
			if (mod == MOD_PROXIMITY_MINE) {
				if (inflictor && inflictor->parent && OnSameTeam(targ, inflictor->parent)) {
					return;
				}
				if (targ == attacker) {
					return;
				}
			}

			// check for godmode
			if ( targ->flags & FL_GODMODE ) {
				return;
			}

	                if(targ->client && targ->client->spawnprotected) {
	                   if(level.time>targ->client->respawnTime+g_spawnprotect.integer)
	                       targ->client->spawnprotected = qfalse;
	                   else
	                       if( (mod > MOD_UNKNOWN && mod < MOD_WATER) || mod == MOD_TELEFRAG || mod>MOD_TRIGGER_HURT)
	                       return;
	                }
		}

		// battlesuit protects from all radius damage (but takes knockback)
		// and protects 50% against all damage
		if ( client && client->ps.powerups[PW_BATTLESUIT] ) {
			G_AddEvent( targ, EV_POWERUP_BATTLESUIT, 0 );
			if ( ( dflags & DAMAGE_RADIUS ) || ( mod == MOD_FALLING ) ) {
				return;
			}
			damage *= 0.5;
		}

		// add to the attacker's hit counter (if the target isn't a general entity like a prox mine)
		if ( attacker->client && client
				&& targ != attacker && targ->health > 0
				&& targ->s.eType != ET_MISSILE
				&& targ->s.eType != ET_GENERAL) {
			if ( OnSameTeam( targ, attacker ) ) {
				attacker->client->ps.persistant[PERS_HITS]--;
			} else {
				attacker->client->ps.persistant[PERS_HITS]++;
			}
			attacker->client->ps.persistant[PERS_ATTACKEE_ARMOR] = (targ->health<<8)|(client->ps.stats[STAT_ARMOR]);
		}

		// always give half damage if hurting self
		// calculated after knockback, so rocket jumping works
		if ( targ == attacker) {
			damage *= 0.5;
		}

	        if(targ && targ->client && attacker->client )
	            damage = catchup_damage(damage, attacker->client->ps.persistant[PERS_SCORE], targ->client->ps.persistant[PERS_SCORE]);

	        if(g_damageModifier.value > 0.01) {
	            damage *= g_damageModifier.value;
	        }

		if ( damage < 1 ) {
			damage = 1;
		}

	        if(targ == attacker && (g_dmflags.integer & DF_NO_SELF_DAMAGE) )
	            damage = 0;

		if ((g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS || g_elimination_allgametypes.integer)
					&& g_elimination_selfdamage.integer<1 && ( targ == attacker ||  mod == MOD_FALLING )) {
			damage = 0;
		}


	//So people can be telefragged!
		if ((g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS) && level.time < level.roundStartTime && ((mod == MOD_LAVA) || (mod == MOD_SLIME)) ) {
			damage = 1000;
		}


		take = damage;
		//save = 0;

		// save some from armor
		asave = CheckArmor (targ, take, dflags);
		take -= asave;

		if ( g_debugDamage.integer ) {
			G_Printf( "%i: client:%i health:%i damage:%i armor:%i\n", level.time, targ->s.number,
				targ->health, take, asave );
		}

		// add to the damage inflicted on a player this frame
		// the total will be turned into screen blends and view angle kicks
		// at the end of the frame
		if ( client ) {
			if ( attacker ) {
				client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
	                } else if(client->lastSentFlying) {
	                        client->ps.persistant[PERS_ATTACKER] = client->lastSentFlying;
	                } else {
				client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
			}
			client->damage_armor += asave;
			client->damage_blood += take;
			client->damage_knockback += knockback;
			if ( dir ) {
				VectorCopy ( dir, client->damage_from );
				client->damage_fromWorld = qfalse;
			} else {
				VectorCopy ( targ->r.currentOrigin, client->damage_from );
				client->damage_fromWorld = qtrue;
			}
		}

		// See if it's the player hurting the emeny flag carrier
		if( g_gametype.integer == GT_CTF || g_gametype.integer == GT_1FCTF || g_gametype.integer == GT_CTF_ELIMINATION) {
			Team_CheckHurtCarrier(targ, attacker);
		}

		if (targ->client) {
			// set the last client who damaged the target
			targ->client->lasthurt_client = attacker->s.number;
			targ->client->lasthurt_mod = mod;

			if(g_locationdamage.integer == 1){
			// Modify the damage for location damage
			if (point && targ && targ->health > 0 && attacker && take)
			take = G_LocationDamage(point, targ, attacker, take);
			else
			targ->client->lasthurt_location = LOCATION_NONE;
			}
		}

		//If vampire is enabled, gain health but not from self or teammate, cannot steal more than targ has
		if( g_vampire.value>0.0 && (targ != attacker) && take > 0 &&
	                !(OnSameTeam(targ, attacker)) && attacker->health > 0 && targ->health > 0 )
		{
			if(take<targ->health)
				attacker->health += (int)(((float)take)*g_vampire.value);
			else
				attacker->health += (int)(((float)targ->health)*g_vampire.value);
			if(attacker->health>g_vampireMaxHealth.integer)
				attacker->health = g_vampireMaxHealth.integer;
		}

		// oatmeal begin


		// Vampire Weapons
		if(mod==MOD_MACHINEGUN){
			if(g_mgvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if(mod==MOD_SHOTGUN){
			if(g_sgvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if(mod==MOD_RAILGUN){
			if(g_rgvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if(mod==MOD_LIGHTNING){
	if(g_portallight.integer == 1){
	PortalTouches( targ );
	}
			if(g_lgvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if(mod==MOD_NAIL){
			if(g_ngvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if(mod==MOD_CHAINGUN){
			if(g_cgvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if(mod==MOD_PROXIMITY_MINE){
			if(g_plvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if((mod==MOD_GRENADE)||(mod==MOD_GRENADE_SPLASH)){
			if(g_glvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if((mod==MOD_ROCKET)||(mod==MOD_ROCKET_SPLASH)){
			if(g_rlvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if((mod==MOD_PLASMA)||(mod==MOD_PLASMA_SPLASH)){
			if(g_pgvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if((mod==MOD_BFG)||(mod==MOD_BFG_SPLASH)){
			if(g_bfgvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if((mod==MOD_FLAME)||(mod==MOD_FLAME_SPLASH)){
			if(g_ftvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}

		if((mod==MOD_ANTIMATTER)||(mod==MOD_ANTIMATTER_SPLASH)){
			if(g_amvampire.integer==1){
				if(attacker->health<mod_vampire_max_health){
					attacker->health += take;
					if(attacker->health>mod_vampire_max_health){
						attacker->health = mod_vampire_max_health;
					}
				}
			}
		}


		// oatmeal end


		// do the damage
		if (take) {
			targ->health = targ->health - take;
			if ( targ->client ) {
				targ->client->ps.stats[STAT_HEALTH] = targ->health;
			}
			
		if ( !strcmp(targ->classname, "func_breakable") ) {
			targ->health -= damage;
			if ( targ->health <= 0 ){
			    Break_Breakable(targ, attacker);
			}
		}

			if ( targ->health <= 0 ) {
				if ( client )
					targ->flags |= FL_NO_KNOCKBACK;

				if (targ->health < -999)
					targ->health = -999;

				targ->enemy = attacker;
				targ->die (targ, inflictor, attacker, take, mod);
				return;
			} else if ( targ->pain ) {
				targ->pain (targ, attacker, take);
			}
		}
	}
}
if(g_building.integer != 2){
//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
	if (!targ->takedamage) {
		return;
	}

	// the intermission has allready been qualified for, so don't
	// allow any extra scoring
	if ( level.intermissionQueued ) {
		return;
	}
	if ( targ->client && mod != MOD_JUICED) {
		if ( targ->client->invulnerabilityTime > level.time) {
			if ( dir && point ) {
				G_InvulnerabilityEffect( targ, dir, point, impactpoint, bouncedir );
			}
			return;
		}
	}
        //Sago: This was moved up
        client = targ->client;

        //Sago: See if the client was sent flying
        //Check if damage is by somebody who is not a player!
        if( (!attacker || attacker->s.eType != ET_PLAYER) && client && client->lastSentFlying>-1 && ( mod==MOD_FALLING || mod==MOD_LAVA || mod==MOD_SLIME || mod==MOD_TRIGGER_HURT || mod==MOD_SUICIDE) )  {
            if( client->lastSentFlyingTime+5000<level.time) {
                client->lastSentFlying = -1; //More than 5 seconds, not a kill!
            } else {
                //G_Printf("LastSentFlying %i\n",client->lastSentFlying);
                attacker = &g_entities[client->lastSentFlying];
            }
        }

	if ( !inflictor ) {
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}
	if ( !attacker ) {
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

		// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER) {
		if (strcmp(targ->classname, "func_breakable") && targ->use && (targ->moverState == MOVER_POS1 || targ->moverState == ROTATOR_POS1)) {
			targ->use(targ, inflictor, attacker);
			}
		if (!strcmp(targ->classname, "func_train") && targ->health > 0) {
			G_UseTargets(targ, attacker);
		} else {	// entity is a func_breakable
			if ( (targ->spawnflags & 1024) && attacker == attacker )
			return;
			if ( (targ->spawnflags & 2048) && IsBot(attacker) )
				return;
			if ( (targ->spawnflags & 4096) && strstr(attacker->classname, "shooter_") )
				return;
			
				}
	}
	if( g_gametype.integer == GT_OBELISK && CheckObeliskAttack( targ, attacker ) ) {
		return;
	}
	// reduce damage by the attacker's handicap value
	// unless they are rocket jumping
	if ( attacker->client && attacker != targ ) {
		max = attacker->client->ps.stats[STAT_MAX_HEALTH];
		if( bg_itemlist[attacker->client->ps.stats[STAT_PERSISTANT_POWERUP]].giTag == PW_GUARD ) {
			max /= 2;
		}
		damage = damage * max / 100;
	}

        //Sago: I have moved this up
	//client = targ->client;

	if ( client ) {
		if ( client->noclip ) {
			return;
		}
	}

	if ( !dir ) {
		dflags |= DAMAGE_NO_KNOCKBACK;
	} else {
		VectorNormalize(dir);
	}

	knockback = damage;

	if ( mod == MOD_GAUNTLET )
	knockback *= g_gknockback.value;
	if ( mod == MOD_MACHINEGUN )
	knockback *= g_mgknockback.value;
	if ( mod == MOD_SHOTGUN )
	knockback *= g_sgknockback.value;
	if ( mod == MOD_GRENADE )
	knockback *= g_glknockback.value;
	if ( mod == MOD_GRENADE_SPLASH )
	knockback *= g_glknockback.value;
	if ( mod == MOD_ROCKET )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_ROCKET_SPLASH )
	knockback *= g_rlknockback.value;
	if ( mod == MOD_PLASMA )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_PLASMA_SPLASH )
	knockback *= g_pgknockback.value;
	if ( mod == MOD_RAILGUN )
	knockback *= g_rgknockback.value;
	if ( mod == MOD_LIGHTNING )
	knockback *= g_lgknockback.value;
	if ( mod == MOD_BFG )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_BFG_SPLASH )
	knockback *= g_bfgknockback.value;
	if ( mod == MOD_NAIL )
	knockback *= g_ngknockback.value;
	if ( mod == MOD_CHAINGUN )
	knockback *= g_cgknockback.value;
	if ( mod == MOD_PROXIMITY_MINE )
	knockback *= g_plknockback.value;
	if ( mod == MOD_FLAME )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_FLAME_SPLASH )
	knockback *= g_ftknockback.value;
	if ( mod == MOD_ANTIMATTER )
	knockback *= g_amknockback.value;
	if ( mod == MOD_ANTIMATTER_SPLASH )
	knockback *= g_amknockback.value;

	if ( targ->flags & FL_NO_KNOCKBACK ) {
		knockback = 0;
	}
	if ( dflags & DAMAGE_NO_KNOCKBACK ) {
		knockback = 0;
	}

	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client ) {
		vec3_t	kvel;
		float	mass;

		mass = 200;

		VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
		VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

		// set the timer so that the other client can't cancel
		// out the movement immediately
		if ( !targ->client->ps.pm_time ) {
			int		t;

			t = knockback * 2;
			if ( t < 50 ) {
				t = 50;
			}
			if ( t > 200 ) {
				t = 200;
			}
			targ->client->ps.pm_time = t;
			targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		}
                //Remeber the last person to hurt the player
                if( !g_awardpushing.integer || targ==attacker || OnSameTeam (targ, attacker)) {
                    targ->client->lastSentFlying = -1;
                } else {
	/*if ( pm->waterlevel <= 1 ) {
		if ( pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK) ) {
			// if getting knocked back, no friction
			if ( ! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) ) {
				control = speed < pm_stopspeed ? pm_stopspeed : speed;
				drop += control*pm_friction*pml.frametime;
			}
		}
	}*/
                    targ->client->lastSentFlying = attacker->s.number;
                    targ->client->lastSentFlyingTime = level.time;
                }
	}

	// check for completely getting out of the damage
	if ( !(dflags & DAMAGE_NO_PROTECTION) ) {

		// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
		// if the attacker was on the same team
		if ( mod != MOD_JUICED && mod != MOD_CRUSH && targ != attacker && !(dflags & DAMAGE_NO_TEAM_PROTECTION) && OnSameTeam (targ, attacker)  ) {
			if ( ( !g_friendlyFire.integer && g_gametype.integer != GT_ELIMINATION && g_gametype.integer != GT_CTF_ELIMINATION ) || ( g_elimination_selfdamage.integer<2 &&	(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION) ) ) {
				return;
			}
		}
		if (mod == MOD_PROXIMITY_MINE) {
			if (inflictor && inflictor->parent && OnSameTeam(targ, inflictor->parent)) {
				return;
			}
			if (targ == attacker) {
				return;
			}
		}

		// check for godmode
		if ( targ->flags & FL_GODMODE ) {
			return;
		}

                if(targ->client && targ->client->spawnprotected) {
                   if(level.time>targ->client->respawnTime+g_spawnprotect.integer)
                       targ->client->spawnprotected = qfalse;
                   else
                       if( (mod > MOD_UNKNOWN && mod < MOD_WATER) || mod == MOD_TELEFRAG || mod>MOD_TRIGGER_HURT)
                       return;
                }
	}

	// battlesuit protects from all radius damage (but takes knockback)
	// and protects 50% against all damage
	if ( client && client->ps.powerups[PW_BATTLESUIT] ) {
		G_AddEvent( targ, EV_POWERUP_BATTLESUIT, 0 );
		if ( ( dflags & DAMAGE_RADIUS ) || ( mod == MOD_FALLING ) ) {
			return;
		}
		damage *= 0.5;
	}

	// add to the attacker's hit counter (if the target isn't a general entity like a prox mine)
	if ( attacker->client && client
			&& targ != attacker && targ->health > 0
			&& targ->s.eType != ET_MISSILE
			&& targ->s.eType != ET_GENERAL) {
		if ( OnSameTeam( targ, attacker ) ) {
			attacker->client->ps.persistant[PERS_HITS]--;
		} else {
			attacker->client->ps.persistant[PERS_HITS]++;
		}
		attacker->client->ps.persistant[PERS_ATTACKEE_ARMOR] = (targ->health<<8)|(client->ps.stats[STAT_ARMOR]);
	}

	// always give half damage if hurting self
	// calculated after knockback, so rocket jumping works
	if ( targ == attacker) {
		damage *= 0.5;
	}

        if(targ && targ->client && attacker->client )
            damage = catchup_damage(damage, attacker->client->ps.persistant[PERS_SCORE], targ->client->ps.persistant[PERS_SCORE]);

        if(g_damageModifier.value > 0.01) {
            damage *= g_damageModifier.value;
        }

	if ( damage < 1 ) {
		damage = 1;
	}

        if(targ == attacker && (g_dmflags.integer & DF_NO_SELF_DAMAGE) )
            damage = 0;

	if ((g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS || g_elimination_allgametypes.integer)
				&& g_elimination_selfdamage.integer<1 && ( targ == attacker ||  mod == MOD_FALLING )) {
		damage = 0;
	}


//So people can be telefragged!
	if ((g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS) && level.time < level.roundStartTime && ((mod == MOD_LAVA) || (mod == MOD_SLIME)) ) {
		damage = 1000;
	}


	take = damage;
	//save = 0;

	// save some from armor
	asave = CheckArmor (targ, take, dflags);
	take -= asave;

	if ( g_debugDamage.integer ) {
		G_Printf( "%i: client:%i health:%i damage:%i armor:%i\n", level.time, targ->s.number,
			targ->health, take, asave );
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( client ) {
		if ( attacker ) {
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
                } else if(client->lastSentFlying) {
                        client->ps.persistant[PERS_ATTACKER] = client->lastSentFlying;
                } else {
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}
		client->damage_armor += asave;
		client->damage_blood += take;
		client->damage_knockback += knockback;
		if ( dir ) {
			VectorCopy ( dir, client->damage_from );
			client->damage_fromWorld = qfalse;
		} else {
			VectorCopy ( targ->r.currentOrigin, client->damage_from );
			client->damage_fromWorld = qtrue;
		}
	}

	// See if it's the player hurting the emeny flag carrier
	if( g_gametype.integer == GT_CTF || g_gametype.integer == GT_1FCTF || g_gametype.integer == GT_CTF_ELIMINATION) {
		Team_CheckHurtCarrier(targ, attacker);
	}

	if (targ->client) {
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod = mod;

		if(g_locationdamage.integer == 1){
		// Modify the damage for location damage
		if (point && targ && targ->health > 0 && attacker && take)
		take = G_LocationDamage(point, targ, attacker, take);
		else
		targ->client->lasthurt_location = LOCATION_NONE;
		}
	}

	//If vampire is enabled, gain health but not from self or teammate, cannot steal more than targ has
	if( g_vampire.value>0.0 && (targ != attacker) && take > 0 &&
                !(OnSameTeam(targ, attacker)) && attacker->health > 0 && targ->health > 0 )
	{
		if(take<targ->health)
			attacker->health += (int)(((float)take)*g_vampire.value);
		else
			attacker->health += (int)(((float)targ->health)*g_vampire.value);
		if(attacker->health>g_vampireMaxHealth.integer)
			attacker->health = g_vampireMaxHealth.integer;
	}

	// oatmeal begin


	// Vampire Weapons
	if(mod==MOD_MACHINEGUN){
		if(g_mgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_SHOTGUN){
		if(g_sgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_RAILGUN){
		if(g_rgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_LIGHTNING){
if(g_portallight.integer == 1){
PortalTouches( targ );
}
		if(g_lgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_NAIL){
		if(g_ngvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_CHAINGUN){
		if(g_cgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if(mod==MOD_PROXIMITY_MINE){
		if(g_plvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_GRENADE)||(mod==MOD_GRENADE_SPLASH)){
		if(g_glvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_ROCKET)||(mod==MOD_ROCKET_SPLASH)){
		if(g_rlvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_PLASMA)||(mod==MOD_PLASMA_SPLASH)){
		if(g_pgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_BFG)||(mod==MOD_BFG_SPLASH)){
		if(g_bfgvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_FLAME)||(mod==MOD_FLAME_SPLASH)){
		if(g_ftvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}

	if((mod==MOD_ANTIMATTER)||(mod==MOD_ANTIMATTER_SPLASH)){
		if(g_amvampire.integer==1){
			if(attacker->health<mod_vampire_max_health){
				attacker->health += take;
				if(attacker->health>mod_vampire_max_health){
					attacker->health = mod_vampire_max_health;
				}
			}
		}
	}


	// oatmeal end


	// do the damage
	if (take) {
		targ->health = targ->health - take;
		if ( targ->client ) {
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
		}
		
		if ( !strcmp(targ->classname, "func_breakable") ) {
			targ->health -= damage;
			if ( targ->health <= 0 ){
			    Break_Breakable(targ, attacker);
			}
		}

		if ( targ->health <= 0 ) {
			if ( client )
				targ->flags |= FL_NO_KNOCKBACK;

			if (targ->health < -999)
				targ->health = -999;

			targ->enemy = attacker;
			targ->die (targ, inflictor, attacker, take, mod);
			return;
		} else if ( targ->pain ) {
			targ->pain (targ, attacker, take);
		}
	}
}
//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii

}


/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
qboolean CanDamage (gentity_t *targ, vec3_t origin) {
	vec3_t	dest;
	trace_t	tr;
	vec3_t	midpoint;

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
	VectorScale (midpoint, 0.5, midpoint);

	VectorCopy (midpoint, dest);
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0 || tr.entityNum == targ->s.number)
		return qtrue;

	// this should probably check in the plane of projection,
	// rather than in world coordinate, and also include Z
	VectorCopy (midpoint, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;


	return qfalse;
}


/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage ( vec3_t origin, gentity_t *attacker, float damage, float radius,
					 gentity_t *ignore, int mod) {
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;

	if ( radius < 1 ) {
		radius = 1;
	}

	for ( i = 0 ; i < 3 ; i++ ) {
		mins[i] = origin[i] - radius;
		maxs[i] = origin[i] + radius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( e = 0 ; e < numListedEntities ; e++ ) {
		ent = &g_entities[entityList[ e ]];

		if (ent == ignore)
			continue;
		if (!ent->takedamage)
			continue;

		// find the distance from the edge of the bounding box
		for ( i = 0 ; i < 3 ; i++ ) {
			if ( origin[i] < ent->r.absmin[i] ) {
				v[i] = ent->r.absmin[i] - origin[i];
			} else if ( origin[i] > ent->r.absmax[i] ) {
				v[i] = origin[i] - ent->r.absmax[i];
			} else {
				v[i] = 0;
			}
		}

		dist = VectorLength( v );
		if ( dist >= radius ) {
			continue;
		}

		points = damage * ( 1.0 - dist / radius );

		if( CanDamage (ent, origin) ) {
			if( LogAccuracyHit( ent, attacker ) ) {
				hitClient = qtrue;
			}
			VectorSubtract (ent->r.currentOrigin, origin, dir);
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;
			G_Damage (ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS, mod);
		}
	}

	return hitClient;
}
