// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

GAME OPTIONS MENU

=======================================================================
*/





#include "ui_local.h"


#define ART_FRAMEL				"menu/art/frame2_l"
#define ART_FRAMER				"menu/art/frame1_r"
#define ART_BACK0				"menu/art/back_0"
#define ART_BACK1				"menu/art/back_1"

#define PREFERENCES_X_POS		360
#define PREFERENCES_X_LEFT		(160 + 5*SMALLCHAR_WIDTH)
#define PREFERENCES_X_RIGHT		(480 + 5*SMALLCHAR_WIDTH)

#define LINE_GAP		(BIGCHAR_HEIGHT + 2)


#define CONTROL_X (SCREEN_WIDTH/2 + 40)
#define SMALLCHAR_LINEHEIGHT (SMALLCHAR_HEIGHT + 2)


#define ID_HUD					50
#define ID_RENDER				51
#define ID_OBJECTS				52
#define ID_MISC					53

#define ID_CROSSHAIR			127
#define ID_SIMPLEITEMS			128
#define ID_HIGHQUALITYSKY		129
#define ID_EJECTINGBRASS		130
#define ID_WALLMARKS			131
#define ID_DYNAMICLIGHTS		132
#define ID_IDENTIFYTARGET		133
#define ID_SYNCEVERYFRAME		134
#define ID_FORCEMODEL			135
#define ID_DRAWTEAMOVERLAY		136
#define ID_ALLOWDOWNLOAD			137
#define ID_BACK					138
#define ID_BOTMENU				139

#define ID_GIBS				140
#define ID_BLOOD			141
#define ID_DEFERPLAYER		142
#define ID_DRAWFPS			143
#define ID_DRAWTIMER		144
#define ID_LAGOMETER		145
#define ID_PREDICTITEMS		146
#define ID_SMOOTHCLIENTS	147
#define ID_SHADOWS			148
#define ID_TEAMCHATHEIGHT	149
#define ID_TEAMCHATTIME		150
#define ID_NEWESCAPEMENU	151
#define ID_FOV				152
#define ID_ZOOMFOV			153
#define ID_AMMOWARNING		154
#define ID_OVERBRIGHT		155
#define ID_DRAWGUN			156

#define ID_OLDRAIL			157
#define ID_OLDPLASMA		158
#define ID_OLDROCKET		159
#define ID_TRUELIGHTNING	160

#define	NUM_CROSSHAIRS			10


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menutext_s			hud;
	menutext_s			render;
	menutext_s			objects;
    menutext_s			misc;

	menulist_s			crosshair;
	menuradiobutton_s	simpleitems;
	menuradiobutton_s	brass;
	menuradiobutton_s	wallmarks;
	menuradiobutton_s	dynamiclights;
	menuradiobutton_s	identifytarget;
	menuradiobutton_s	highqualitysky;
	menuradiobutton_s	synceveryframe;
	menuradiobutton_s	forcemodel;
	menulist_s			drawteamoverlay;
	menuradiobutton_s	allowdownload;
	menuradiobutton_s	botmenu;

	menuradiobutton_s	gibs;
	menuradiobutton_s	blood;
	menuradiobutton_s	deferplayer;
	menuradiobutton_s	drawfps;
	menuradiobutton_s	drawtimer;
	menuradiobutton_s	drawlagometer;
	menuradiobutton_s	drawgun;
	menuradiobutton_s	predictitems;
	menuradiobutton_s	smoothclient;
	menulist_s			shadows;
	menuradiobutton_s	newESCmenu;

	menufield_s			teamchatheight;
	menufield_s			teamchattime;
	menufield_s			fov;
	menufield_s			zoomfov;

	menuradiobutton_s	drawAmmoWarning;
	menulist_s			mapOverBright;	

	menuradiobutton_s	oldrail;
	menuradiobutton_s	oldplasma;
	menuradiobutton_s	oldrocket;
	menuradiobutton_s	truelightning;


	menubitmap_s		back;

	qhandle_t			crosshairShader[NUM_CROSSHAIRS];
	int section;
	int max_control_text;
} preferences_t;

static preferences_t s_preferences;


static const char *overbright_values[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", 0
};

static const char *teamoverlay_names[] =
{
	"off",
	"upper right",
	"lower right",
	"lower left",
	0
};




static const char *shadow_types[] =
{
	"off",
	"disc",
	"stencil",
	"simple stencil",
	0
};








static menucommon_s* g_hud_controls[] = {
	(menucommon_s*) &s_preferences.crosshair,
	(menucommon_s*) &s_preferences.identifytarget,
	(menucommon_s*) &s_preferences.drawteamoverlay,
	(menucommon_s*) &s_preferences.teamchatheight,
	(menucommon_s*) &s_preferences.teamchattime,
	(menucommon_s*) &s_preferences.drawfps,
	(menucommon_s*) &s_preferences.drawtimer,
	(menucommon_s*) &s_preferences.drawlagometer,
	(menucommon_s*) &s_preferences.drawgun,
	(menucommon_s*) &s_preferences.drawAmmoWarning,
	NULL
};


static menucommon_s* g_render_controls[] = {
	(menucommon_s*) &s_preferences.shadows,
	(menucommon_s*) &s_preferences.highqualitysky,
	(menucommon_s*) &s_preferences.mapOverBright,
	(menucommon_s*) &s_preferences.blood,
	(menucommon_s*) &s_preferences.fov,
	(menucommon_s*) &s_preferences.zoomfov,
	(menucommon_s*) &s_preferences.synceveryframe,
	NULL
};

static menucommon_s* g_object_controls[] = {
	(menucommon_s*) &s_preferences.simpleitems,
	(menucommon_s*) &s_preferences.wallmarks,
	(menucommon_s*) &s_preferences.brass,
	(menucommon_s*) &s_preferences.dynamiclights,
	(menucommon_s*) &s_preferences.gibs,
	(menucommon_s*) &s_preferences.forcemodel,
	(menucommon_s*) &s_preferences.deferplayer,
	(menucommon_s*) &s_preferences.oldrail,
	(menucommon_s*) &s_preferences.oldplasma,
	(menucommon_s*) &s_preferences.oldrocket,
	(menucommon_s*) &s_preferences.truelightning,
	NULL
};

static menucommon_s* g_prefmisc_controls[] = {
	(menucommon_s*) &s_preferences.predictitems,
	(menucommon_s*) &s_preferences.smoothclient,
	(menucommon_s*) &s_preferences.allowdownload,
	(menucommon_s*) &s_preferences.botmenu,
	(menucommon_s*) &s_preferences.newESCmenu,
	NULL
};


#define C_HUD		0
#define C_RENDER	1
#define C_OBJECTS	2
#define C_MISC		3

#define C_MAX 		4



static menucommon_s** g_control_list[] = {
	g_hud_controls,
	g_render_controls,
	g_object_controls,
	g_prefmisc_controls
};



static void Preferences_SetMenuItems( void ) {
	s_preferences.crosshair.curvalue		= (int)trap_Cvar_VariableValue( "cg_drawCrosshair" ) % NUM_CROSSHAIRS;
	s_preferences.simpleitems.curvalue		= trap_Cvar_VariableValue( "cg_simpleItems" ) != 0;
	s_preferences.brass.curvalue			= trap_Cvar_VariableValue( "cg_brassTime" ) != 0;
	s_preferences.wallmarks.curvalue		= trap_Cvar_VariableValue( "cg_marks" ) != 0;
	s_preferences.identifytarget.curvalue	= trap_Cvar_VariableValue( "cg_drawCrosshairNames" ) != 0;
	s_preferences.dynamiclights.curvalue	= trap_Cvar_VariableValue( "r_dynamiclight" ) != 0;
	s_preferences.highqualitysky.curvalue	= trap_Cvar_VariableValue ( "r_fastsky" ) == 0;
	s_preferences.synceveryframe.curvalue	= trap_Cvar_VariableValue( "r_finish" ) != 0;
	s_preferences.forcemodel.curvalue		= trap_Cvar_VariableValue( "cg_forcemodel" ) != 0;
	s_preferences.drawteamoverlay.curvalue	= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_drawTeamOverlay" ) );
	s_preferences.allowdownload.curvalue	= trap_Cvar_VariableValue( "cl_allowDownload" ) != 0;
	s_preferences.botmenu.curvalue			= trap_Cvar_VariableValue( "uie_autoclosebotmenu" ) != 0;

	s_preferences.gibs.curvalue				= trap_Cvar_VariableValue( "cg_gibs" ) != 0;
	s_preferences.blood.curvalue			= trap_Cvar_VariableValue( "com_blood" ) != 0;
	s_preferences.deferplayer.curvalue		= trap_Cvar_VariableValue( "cg_deferPlayers" ) != 0;
	s_preferences.drawfps.curvalue			= trap_Cvar_VariableValue( "cg_drawFPS" ) != 0;
	s_preferences.drawtimer.curvalue		= trap_Cvar_VariableValue( "cg_drawTimer" ) != 0;
	s_preferences.drawlagometer.curvalue	= trap_Cvar_VariableValue( "cg_lagometer" ) != 0;
	s_preferences.drawgun.curvalue			= trap_Cvar_VariableValue( "cg_drawgun" ) != 0;
	s_preferences.predictitems.curvalue		= trap_Cvar_VariableValue( "cg_predictItems" ) != 0;
	s_preferences.smoothclient.curvalue		= trap_Cvar_VariableValue( "cg_smoothClients" ) != 0;
	s_preferences.shadows.curvalue			= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_shadows" ) );
	s_preferences.newESCmenu.curvalue		= trap_Cvar_VariableValue( "uie_ingame_dynamicmenu" ) != 0;
	s_preferences.drawAmmoWarning.curvalue		= trap_Cvar_VariableValue( "cg_drawAmmoWarning" ) != 0;

	s_preferences.mapOverBright.curvalue		= Com_Clamp( 0, 7, trap_Cvar_VariableValue( "r_mapOverBrightBits" ));

	s_preferences.oldrail.curvalue			= trap_Cvar_VariableValue( "cg_oldRail" ) != 0;
	s_preferences.oldplasma.curvalue		= trap_Cvar_VariableValue( "cg_oldPlasma" ) != 0;
	s_preferences.oldrocket.curvalue		= trap_Cvar_VariableValue( "cg_oldRocket" ) != 0;
	s_preferences.truelightning.curvalue	= trap_Cvar_VariableValue( "cg_truelightning" ) != 0;


	trap_Cvar_VariableStringBuffer("cg_teamChatTime", s_preferences.teamchattime.field.buffer, 6);
	trap_Cvar_VariableStringBuffer("cg_teamChatHeight", s_preferences.teamchatheight.field.buffer, 3);
	trap_Cvar_VariableStringBuffer("cg_fov", s_preferences.fov.field.buffer, 4);
	trap_Cvar_VariableStringBuffer("cg_zoomfov", s_preferences.zoomfov.field.buffer, 4);
}



/*
=================
Control_Update
=================
*/
static void Control_Update( void )
{
	int i, j;
	menucommon_s** list;
	menucommon_s*	control;
	int	count, y;

	// hide all controls
	for (i = 0; i < C_MAX; i++)
	{
		list = g_control_list[i];
		for ( j = 0; (control = list[j]); j++)
		{
			control->flags |= (QMF_HIDDEN|QMF_INACTIVE);
		}
	}

	// count controls in current selection
	list = g_control_list[s_preferences.section];
	for (count = 0; list[count]; count++) {
		list[count]->flags &= ~ (QMF_HIDDEN|QMF_INACTIVE|QMF_GRAYED);
	}

	// show current controls, centering vertically
	y = (SCREEN_HEIGHT - (count * SMALLCHAR_LINEHEIGHT)) / 2;
	for (j = 0; (control = list[j]); j++, y += (SMALLCHAR_LINEHEIGHT) )
	{
		control->x = CONTROL_X;
		control->y = y;
		control->left   = CONTROL_X - s_preferences.max_control_text * SMALLCHAR_WIDTH;
		control->right  = CONTROL_X + 21*SMALLCHAR_WIDTH;
		control->top    = y;
		control->bottom = y + SMALLCHAR_HEIGHT;
	}

	// set correct flags on tab controls
	s_preferences.hud.generic.flags |= (QMF_PULSEIFFOCUS);
	s_preferences.render.generic.flags |= (QMF_PULSEIFFOCUS);
	s_preferences.objects.generic.flags |= (QMF_PULSEIFFOCUS);
	s_preferences.misc.generic.flags |= (QMF_PULSEIFFOCUS);

	s_preferences.hud.generic.flags &= ~ (QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
	s_preferences.render.generic.flags &= ~ (QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
	s_preferences.objects.generic.flags &= ~ (QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
	s_preferences.misc.generic.flags &= ~ (QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);

	switch (s_preferences.section) {
	case C_HUD:
		s_preferences.hud.generic.flags &= ~(QMF_PULSEIFFOCUS);
		s_preferences.hud.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;

	case C_RENDER:
		s_preferences.render.generic.flags &= ~(QMF_PULSEIFFOCUS);
		s_preferences.render.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;

	case C_OBJECTS:
		s_preferences.objects.generic.flags &= ~(QMF_PULSEIFFOCUS);
		s_preferences.objects.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;

	case C_MISC:
		s_preferences.misc.generic.flags &= ~(QMF_PULSEIFFOCUS);
		s_preferences.misc.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;
	}
}



/*
=================
Control_Init
=================
*/
static void Control_Init( void )
{
	int i, j, len, max;
	menucommon_s **list;
	menucommon_s *control;

	// count longest string title and save it
	max = 0;
	for (i = 0; i < C_MAX; i++)
	{
		list = g_control_list[i];
		for (j = 0; (control = list[j]); j++)
		{
			if (control->name) {
				len = strlen(control->name) + 1;
				if (len > max)
					max = len;
			}
		}
	}

	s_preferences.max_control_text = max + 1;

	// prepare current tab
	s_preferences.section = C_HUD;
	Control_Update();
}



static void Preferences_InputEvent( void* ptr, int notification ) {
	if( notification != QM_LOSTFOCUS ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_TEAMCHATTIME:
		if (atoi(s_preferences.teamchattime.field.buffer) <= 0) {
			Q_strncpyz(s_preferences.teamchattime.field.buffer, "5000", 5);
		}
		trap_Cvar_Set( "cg_teamChatTime", s_preferences.teamchattime.field.buffer);
		break;

	case ID_TEAMCHATHEIGHT:
		if (atoi(s_preferences.teamchattime.field.buffer) <= 2) {
			Q_strncpyz(s_preferences.teamchattime.field.buffer, "2", 2);
		}
		trap_Cvar_Set( "cg_teamChatHeight", s_preferences.teamchatheight.field.buffer);
		break;

	case ID_FOV:
		if (atoi(s_preferences.fov.field.buffer) <= 0) {
			Q_strncpyz(s_preferences.fov.field.buffer, "90", 3);
		}
		trap_Cvar_Set( "cg_fov", s_preferences.fov.field.buffer);
		break;

	case ID_ZOOMFOV:
		if (atoi(s_preferences.zoomfov.field.buffer) <= 0) {
			Q_strncpyz(s_preferences.zoomfov.field.buffer, "22", 3);
		}
		trap_Cvar_Set( "cg_zoomfov", s_preferences.zoomfov.field.buffer);
		break;
	}

}




static void Preferences_Event( void* ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_HUD:
		s_preferences.section = C_HUD;
		Control_Update();
		break;

	case ID_RENDER:
		s_preferences.section = C_RENDER;
		Control_Update();
		break;

	case ID_OBJECTS:
		s_preferences.section = C_OBJECTS;
		Control_Update();
		break;

	case ID_MISC:
		s_preferences.section = C_MISC;
		Control_Update();
		break;

	case ID_CROSSHAIR:
		s_preferences.crosshair.curvalue++;
		if( s_preferences.crosshair.curvalue == NUM_CROSSHAIRS ) {
			s_preferences.crosshair.curvalue = 0;
		}
		trap_Cvar_SetValue( "cg_drawCrosshair", s_preferences.crosshair.curvalue );
		break;

	case ID_SIMPLEITEMS:
		trap_Cvar_SetValue( "cg_simpleItems", s_preferences.simpleitems.curvalue );
		break;

	case ID_HIGHQUALITYSKY:
		trap_Cvar_SetValue( "r_fastsky", !s_preferences.highqualitysky.curvalue );
		break;

	case ID_EJECTINGBRASS:
		if ( s_preferences.brass.curvalue )
			trap_Cvar_Reset( "cg_brassTime" );
		else
			trap_Cvar_SetValue( "cg_brassTime", 0 );
		break;

	case ID_WALLMARKS:
		trap_Cvar_SetValue( "cg_marks", s_preferences.wallmarks.curvalue );
		break;

	case ID_DYNAMICLIGHTS:
		trap_Cvar_SetValue( "r_dynamiclight", s_preferences.dynamiclights.curvalue );
		break;		

	case ID_IDENTIFYTARGET:
		trap_Cvar_SetValue( "cg_drawCrosshairNames", s_preferences.identifytarget.curvalue );
		break;

	case ID_SYNCEVERYFRAME:
		trap_Cvar_SetValue( "r_finish", s_preferences.synceveryframe.curvalue );
		break;

	case ID_FORCEMODEL:
		trap_Cvar_SetValue( "cg_forcemodel", s_preferences.forcemodel.curvalue );
		break;

	case ID_DRAWTEAMOVERLAY:
		trap_Cvar_SetValue( "cg_drawTeamOverlay", s_preferences.drawteamoverlay.curvalue );
		break;

	case ID_ALLOWDOWNLOAD:
		trap_Cvar_SetValue( "cl_allowDownload", s_preferences.allowdownload.curvalue );
		trap_Cvar_SetValue( "sv_allowDownload", s_preferences.allowdownload.curvalue );
		break;

	case ID_BOTMENU:
		trap_Cvar_SetValue( "uie_autoclosebotmenu", s_preferences.botmenu.curvalue );
		break;

	case ID_GIBS:
		trap_Cvar_SetValue( "cg_gibs", s_preferences.gibs.curvalue );
		break;

	case ID_BLOOD:
		trap_Cvar_SetValue( "com_blood", s_preferences.blood.curvalue );
		break;

	case ID_DEFERPLAYER:
		trap_Cvar_SetValue( "cg_deferPlayers", s_preferences.deferplayer.curvalue );
		break;

	case ID_DRAWFPS:
		trap_Cvar_SetValue( "cg_drawFPS", s_preferences.drawfps.curvalue );
		break;

	case ID_DRAWTIMER:
		trap_Cvar_SetValue( "cg_drawTimer", s_preferences.drawtimer.curvalue );
		break;

	case ID_AMMOWARNING:
		trap_Cvar_SetValue( "cg_drawAmmoWarning", s_preferences.drawAmmoWarning.curvalue );
		break;

	case ID_LAGOMETER:
		trap_Cvar_SetValue( "cg_lagometer", s_preferences.drawlagometer.curvalue );
		break;

	case ID_DRAWGUN:
		trap_Cvar_SetValue( "cg_drawgun", s_preferences.drawgun.curvalue );
		break;

	case ID_PREDICTITEMS:
		trap_Cvar_SetValue( "cg_predictItems", s_preferences.predictitems.curvalue );
		break;

	case ID_SMOOTHCLIENTS:
		trap_Cvar_SetValue( "cg_smoothClients", s_preferences.smoothclient.curvalue );
		break;

	case ID_SHADOWS:
		trap_Cvar_SetValue( "cg_shadows", s_preferences.shadows.curvalue );
		break;

	case ID_NEWESCAPEMENU:
		trap_Cvar_SetValue( "uie_ingame_dynamicmenu", s_preferences.newESCmenu.curvalue );
		break;

	case ID_OVERBRIGHT:
		trap_Cvar_SetValue( "r_mapOverBrightBits", s_preferences.mapOverBright.curvalue );
		break;

	case ID_OLDRAIL:
		trap_Cvar_SetValue( "cg_oldRail", s_preferences.oldrail.curvalue );
		break;

	case ID_OLDPLASMA:
		trap_Cvar_SetValue( "cg_oldPlasma", s_preferences.oldplasma.curvalue );
		break;

	case ID_OLDROCKET:
		trap_Cvar_SetValue( "cg_oldRocket", s_preferences.oldrocket.curvalue );
		break;

	case ID_TRUELIGHTNING:
		trap_Cvar_SetValue( "cg_truelightning", s_preferences.truelightning.curvalue ? 0.5 : 0.0);
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}




/*
=================
Crosshair_Draw
=================
*/
static void Crosshair_Draw( void *self ) {
	menulist_s	*s;
	float		*color;
	int			x, y;
	int			style;
	qboolean	focus;

	s = (menulist_s *)self;
	x = s->generic.x;
	y =	s->generic.y;

	style = UI_SMALLFONT;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);

	if ( s->generic.flags & QMF_GRAYED )
		color = text_color_disabled;
	else if ( focus )
	{
		color = text_color_highlight;
		style |= UI_PULSE;
	}
	else if ( s->generic.flags & QMF_BLINK )
	{
		color = text_color_highlight;
		style |= UI_BLINK;
	}
	else
		color = text_color_normal;

	if ( focus )
	{
		// draw cursor
		UI_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, style|UI_RIGHT, color );
	if( !s->curvalue ) {
		return;
	}
	UI_DrawHandlePic( x + SMALLCHAR_WIDTH, y - 4, 24, 24, s_preferences.crosshairShader[s->curvalue] );
}




/*
=================
Preferences_Statusbar
=================
*/
static void Preferences_Statusbar(void* self)
{
	char* s;

	switch( ((menucommon_s*)self)->id ) {
		case ID_SYNCEVERYFRAME:
			s = "on = game waits for OpenGL calls to finish";
			break;
		case ID_OVERBRIGHT:
			s = "Map light saturation (default = 2)";
			break;
		case ID_FOV:
			s = "Field of view angle (default = 90)";
			break;
		case ID_ZOOMFOV:
			s = "Zoomed view angle";
			break;
		case ID_SIMPLEITEMS:
			s = "Picture instead of 3d object";
			break;
		case ID_HIGHQUALITYSKY:
			s = "off = no portal view of destination as well";
			break;
		case ID_DRAWTEAMOVERLAY:
			s = "Team specific messages and info";
			break;
		case ID_TEAMCHATHEIGHT:
			s = "Lines of team chat text visible";
			break;
		case ID_TEAMCHATTIME:
			s = "Max time in view (milli-seconds)";
			break;
		case ID_SMOOTHCLIENTS:
			s = "Other players movement predicted";
			break;
		case ID_PREDICTITEMS:
			s = "Item pickup predicted";
			break;
	}

	if (s)
		UI_DrawString(320, 480 - 64, s, UI_CENTER|UI_SMALLFONT, color_white);
}




static void Preferences_MenuInit( void )
{
	memset( &s_preferences, 0 ,sizeof(preferences_t) );

	Preferences_Cache();

	s_preferences.menu.wrapAround = qtrue;
	s_preferences.menu.fullscreen = qtrue;

	s_preferences.banner.generic.type  = MTYPE_BTEXT;
	s_preferences.banner.generic.x	   = 320;
	s_preferences.banner.generic.y	   = 16;
	s_preferences.banner.string		   = "GAME OPTIONS";
	s_preferences.banner.color         = color_white;
	s_preferences.banner.style         = UI_CENTER;

	s_preferences.framel.generic.type  = MTYPE_BITMAP;
	s_preferences.framel.generic.name  = ART_FRAMEL;
	s_preferences.framel.generic.flags = QMF_INACTIVE;
	s_preferences.framel.generic.x	   = 0;
	s_preferences.framel.generic.y	   = 78;
	s_preferences.framel.width  	   = 256;
	s_preferences.framel.height  	   = 329;

	s_preferences.framer.generic.type  = MTYPE_BITMAP;
	s_preferences.framer.generic.name  = ART_FRAMER;
	s_preferences.framer.generic.flags = QMF_INACTIVE;
	s_preferences.framer.generic.x	   = 376;
	s_preferences.framer.generic.y	   = 76;
	s_preferences.framer.width  	   = 256;
	s_preferences.framer.height  	   = 334;


	s_preferences.hud.generic.type     = MTYPE_PTEXT;
	s_preferences.hud.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.hud.generic.id	    = ID_HUD;
	s_preferences.hud.generic.callback	= Preferences_Event;
	s_preferences.hud.generic.x	    = 152;
	s_preferences.hud.generic.y	    = 240 - 2 * PROP_HEIGHT;
	s_preferences.hud.string			= "HUD";
	s_preferences.hud.style			= UI_RIGHT;
	s_preferences.hud.color			= color_red;

	s_preferences.render.generic.type     = MTYPE_PTEXT;
	s_preferences.render.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.render.generic.id	    = ID_RENDER;
	s_preferences.render.generic.callback	= Preferences_Event;
	s_preferences.render.generic.x	    = 152;
	s_preferences.render.generic.y	    = 240 - PROP_HEIGHT;
	s_preferences.render.string			= "RENDER";
	s_preferences.render.style			= UI_RIGHT;
	s_preferences.render.color			= color_red;

	s_preferences.objects.generic.type     = MTYPE_PTEXT;
	s_preferences.objects.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.objects.generic.id	    = ID_OBJECTS;
	s_preferences.objects.generic.callback	= Preferences_Event;
	s_preferences.objects.generic.x	    = 152;
	s_preferences.objects.generic.y	    = 240;
	s_preferences.objects.string			= "OBJECTS";
	s_preferences.objects.style			= UI_RIGHT;
	s_preferences.objects.color			= color_red;

	s_preferences.misc.generic.type     = MTYPE_PTEXT;
	s_preferences.misc.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.misc.generic.id	    = ID_MISC;
	s_preferences.misc.generic.callback	= Preferences_Event;
	s_preferences.misc.generic.x	    = 152;
	s_preferences.misc.generic.y	    = 240 + PROP_HEIGHT;
	s_preferences.misc.string			= "MISC";
	s_preferences.misc.style			= UI_RIGHT;
	s_preferences.misc.color			= color_red;

	s_preferences.back.generic.type	    = MTYPE_BITMAP;
	s_preferences.back.generic.name     = ART_BACK0;
	s_preferences.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.back.generic.callback = Preferences_Event;
	s_preferences.back.generic.id	    = ID_BACK;
	s_preferences.back.generic.x		= 0;
	s_preferences.back.generic.y		= 480-64;
	s_preferences.back.width  		    = 128;
	s_preferences.back.height  		    = 64;
	s_preferences.back.focuspic         = ART_BACK1;


	// tabbed controls

	s_preferences.crosshair.generic.type		= MTYPE_TEXT;
	s_preferences.crosshair.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NODEFAULTINIT|QMF_OWNERDRAW;
	s_preferences.crosshair.generic.name		= "Crosshair:";
	s_preferences.crosshair.generic.callback	= Preferences_Event;
	s_preferences.crosshair.generic.ownerdraw	= Crosshair_Draw;
	s_preferences.crosshair.generic.id			= ID_CROSSHAIR;

	s_preferences.simpleitems.generic.type        = MTYPE_RADIOBUTTON;
	s_preferences.simpleitems.generic.name	      = "Simple Items:";
	s_preferences.simpleitems.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.simpleitems.generic.callback    = Preferences_Event;
	s_preferences.simpleitems.generic.statusbar= Preferences_Statusbar;
	s_preferences.simpleitems.generic.id          = ID_SIMPLEITEMS;

	s_preferences.wallmarks.generic.type          = MTYPE_RADIOBUTTON;
	s_preferences.wallmarks.generic.name	      = "Marks on Walls:";
	s_preferences.wallmarks.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.wallmarks.generic.callback      = Preferences_Event;
	s_preferences.wallmarks.generic.id            = ID_WALLMARKS;

	s_preferences.brass.generic.type              = MTYPE_RADIOBUTTON;
	s_preferences.brass.generic.name	          = "Ejecting Brass:";
	s_preferences.brass.generic.flags	          = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.brass.generic.callback          = Preferences_Event;
	s_preferences.brass.generic.id                = ID_EJECTINGBRASS;

	s_preferences.dynamiclights.generic.type      = MTYPE_RADIOBUTTON;
	s_preferences.dynamiclights.generic.name	  = "Dynamic Lights:";
	s_preferences.dynamiclights.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.dynamiclights.generic.callback  = Preferences_Event;
	s_preferences.dynamiclights.generic.id        = ID_DYNAMICLIGHTS;

	s_preferences.highqualitysky.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.highqualitysky.generic.name	  = "High Quality Sky:";
	s_preferences.highqualitysky.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.highqualitysky.generic.callback = Preferences_Event;
	s_preferences.highqualitysky.generic.statusbar= Preferences_Statusbar;
	s_preferences.highqualitysky.generic.id       = ID_HIGHQUALITYSKY;

	s_preferences.shadows.generic.type     = MTYPE_SPINCONTROL;
	s_preferences.shadows.generic.name	   = "Shadow type:";
	s_preferences.shadows.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.shadows.generic.callback = Preferences_Event;
	s_preferences.shadows.generic.id       = ID_SHADOWS;
	s_preferences.shadows.itemnames			= shadow_types;

	s_preferences.mapOverBright.generic.type     = MTYPE_SPINCONTROL;
	s_preferences.mapOverBright.generic.name	   = "Over Bright Bits:";
	s_preferences.mapOverBright.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.mapOverBright.generic.callback = Preferences_Event;
	s_preferences.mapOverBright.generic.statusbar= Preferences_Statusbar;
	s_preferences.mapOverBright.generic.id       = ID_OVERBRIGHT;
	s_preferences.mapOverBright.itemnames			= overbright_values;

	s_preferences.gibs.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.gibs.generic.name	   = "Body Gibs:";
	s_preferences.gibs.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.gibs.generic.callback = Preferences_Event;
	s_preferences.gibs.generic.id       = ID_GIBS;

	s_preferences.blood.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.blood.generic.name	   = "Blood:";
	s_preferences.blood.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.blood.generic.callback = Preferences_Event;
	s_preferences.blood.generic.id       = ID_BLOOD;


	s_preferences.identifytarget.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.identifytarget.generic.name	  = "Identify Target:";
	s_preferences.identifytarget.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.identifytarget.generic.callback = Preferences_Event;
	s_preferences.identifytarget.generic.id       = ID_IDENTIFYTARGET;

	s_preferences.drawfps.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.drawfps.generic.name	   = "Draw FPS:";
	s_preferences.drawfps.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawfps.generic.callback = Preferences_Event;
	s_preferences.drawfps.generic.id       = ID_DRAWFPS;

	s_preferences.drawtimer.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.drawtimer.generic.name	   = "Draw Timer:";
	s_preferences.drawtimer.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawtimer.generic.callback = Preferences_Event;
	s_preferences.drawtimer.generic.id       = ID_DRAWTIMER;

	s_preferences.drawlagometer.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.drawlagometer.generic.name	   = "Draw Lagometer:";
	s_preferences.drawlagometer.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawlagometer.generic.callback = Preferences_Event;
	s_preferences.drawlagometer.generic.id       = ID_LAGOMETER;

	s_preferences.drawAmmoWarning.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.drawAmmoWarning.generic.name	   = "Draw Ammo Warning:";
	s_preferences.drawAmmoWarning.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawAmmoWarning.generic.callback = Preferences_Event;
	s_preferences.drawAmmoWarning.generic.id       = ID_AMMOWARNING;

	s_preferences.drawgun.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.drawgun.generic.name	   = "Draw Gun:";
	s_preferences.drawgun.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawgun.generic.callback = Preferences_Event;
	s_preferences.drawgun.generic.id       = ID_DRAWGUN;

	s_preferences.drawteamoverlay.generic.type     = MTYPE_SPINCONTROL;
	s_preferences.drawteamoverlay.generic.name	   = "Draw Team Overlay:";
	s_preferences.drawteamoverlay.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawteamoverlay.generic.callback = Preferences_Event;
	s_preferences.drawteamoverlay.generic.statusbar= Preferences_Statusbar;
	s_preferences.drawteamoverlay.generic.id       = ID_DRAWTEAMOVERLAY;
	s_preferences.drawteamoverlay.itemnames			= teamoverlay_names;

	s_preferences.teamchatheight.generic.type       = MTYPE_FIELD;
	s_preferences.teamchatheight.generic.name       = "Team Chat Height:";
	s_preferences.teamchatheight.generic.id		= ID_TEAMCHATHEIGHT;
	s_preferences.teamchatheight.generic.callback	= Preferences_InputEvent;
	s_preferences.teamchatheight.generic.statusbar= Preferences_Statusbar;
	s_preferences.teamchatheight.generic.flags      = QMF_SMALLFONT|QMF_PULSEIFFOCUS|QMF_NUMBERSONLY;
	s_preferences.teamchatheight.field.widthInChars = 2;
	s_preferences.teamchatheight.field.maxchars     = 2;

	s_preferences.teamchattime.generic.type       = MTYPE_FIELD;
	s_preferences.teamchattime.generic.name       = "Team Chat Time:";
	s_preferences.teamchattime.generic.id		= ID_TEAMCHATTIME;
	s_preferences.teamchattime.generic.callback	= Preferences_InputEvent;
	s_preferences.teamchattime.generic.statusbar= Preferences_Statusbar;
	s_preferences.teamchattime.generic.flags      = QMF_SMALLFONT|QMF_PULSEIFFOCUS|QMF_NUMBERSONLY;
	s_preferences.teamchattime.field.widthInChars = 5;
	s_preferences.teamchattime.field.maxchars     = 5;

	s_preferences.fov.generic.type       = MTYPE_FIELD;
	s_preferences.fov.generic.name       = "FOV:";
	s_preferences.fov.generic.id		= ID_FOV;
	s_preferences.fov.generic.callback	= Preferences_InputEvent;
	s_preferences.fov.generic.statusbar= Preferences_Statusbar;
	s_preferences.fov.generic.flags      = QMF_SMALLFONT|QMF_PULSEIFFOCUS|QMF_NUMBERSONLY;
	s_preferences.fov.field.widthInChars = 3;
	s_preferences.fov.field.maxchars     = 3;

	s_preferences.zoomfov.generic.type       = MTYPE_FIELD;
	s_preferences.zoomfov.generic.name       = "Zoom FOV:";
	s_preferences.zoomfov.generic.id		= ID_ZOOMFOV;
	s_preferences.zoomfov.generic.callback	= Preferences_InputEvent;
	s_preferences.zoomfov.generic.statusbar= Preferences_Statusbar;
	s_preferences.zoomfov.generic.flags      = QMF_SMALLFONT|QMF_PULSEIFFOCUS|QMF_NUMBERSONLY;
	s_preferences.zoomfov.field.widthInChars = 3;
	s_preferences.zoomfov.field.maxchars     = 3;

	s_preferences.predictitems.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.predictitems.generic.name	   = "Predict Items:";
	s_preferences.predictitems.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.predictitems.generic.callback = Preferences_Event;
	s_preferences.predictitems.generic.statusbar= Preferences_Statusbar;
	s_preferences.predictitems.generic.id       = ID_PREDICTITEMS;

	s_preferences.smoothclient.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.smoothclient.generic.name	   = "Smooth Clients:";
	s_preferences.smoothclient.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.smoothclient.generic.callback = Preferences_Event;
	s_preferences.smoothclient.generic.statusbar= Preferences_Statusbar;
	s_preferences.smoothclient.generic.id       = ID_SMOOTHCLIENTS;

	s_preferences.forcemodel.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.forcemodel.generic.name	  = "Force Player Models:";
	s_preferences.forcemodel.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.forcemodel.generic.callback = Preferences_Event;
	s_preferences.forcemodel.generic.id       = ID_FORCEMODEL;

	s_preferences.oldrail.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.oldrail.generic.name	  = "Old Railgun effect:";
	s_preferences.oldrail.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.oldrail.generic.callback = Preferences_Event;
	s_preferences.oldrail.generic.id       = ID_OLDRAIL;

	s_preferences.oldplasma.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.oldplasma.generic.name	  = "Old Plasma effect:";
	s_preferences.oldplasma.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.oldplasma.generic.callback = Preferences_Event;
	s_preferences.oldplasma.generic.id       = ID_OLDPLASMA;

	s_preferences.oldrocket.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.oldrocket.generic.name	  = "Old Rocket effect:";
	s_preferences.oldrocket.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.oldrocket.generic.callback = Preferences_Event;
	s_preferences.oldrocket.generic.id       = ID_OLDROCKET;

	s_preferences.truelightning.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.truelightning.generic.name	  = "True lightning:";
	s_preferences.truelightning.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.truelightning.generic.callback = Preferences_Event;
	s_preferences.truelightning.generic.id       = ID_TRUELIGHTNING;


	s_preferences.deferplayer.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.deferplayer.generic.name	   = "Defer model loading:";
	s_preferences.deferplayer.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.deferplayer.generic.callback = Preferences_Event;
	s_preferences.deferplayer.generic.id       = ID_DEFERPLAYER;

	s_preferences.synceveryframe.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.synceveryframe.generic.name	  = "Sync Every Frame:";
	s_preferences.synceveryframe.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.synceveryframe.generic.callback = Preferences_Event;
	s_preferences.synceveryframe.generic.id       = ID_SYNCEVERYFRAME;

	s_preferences.allowdownload.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.allowdownload.generic.name	   = "Automatic Downloading:";
	s_preferences.allowdownload.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.allowdownload.generic.callback = Preferences_Event;
	s_preferences.allowdownload.generic.id       = ID_ALLOWDOWNLOAD;

	s_preferences.botmenu.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.botmenu.generic.name	  = "AutoClose Bot Menu:";
	s_preferences.botmenu.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.botmenu.generic.callback = Preferences_Event;
	s_preferences.botmenu.generic.id       = ID_BOTMENU;

	s_preferences.newESCmenu.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.newESCmenu.generic.name	   = "Dynamic Escape Menu:";
	s_preferences.newESCmenu.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.newESCmenu.generic.callback = Preferences_Event;
	s_preferences.newESCmenu.generic.id       = ID_NEWESCAPEMENU;



	Menu_AddItem( &s_preferences.menu, &s_preferences.banner );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framer );

	Menu_AddItem( &s_preferences.menu, &s_preferences.hud);
	Menu_AddItem( &s_preferences.menu, &s_preferences.render);
	Menu_AddItem( &s_preferences.menu, &s_preferences.objects);
	Menu_AddItem( &s_preferences.menu, &s_preferences.misc);

	Menu_AddItem( &s_preferences.menu, &s_preferences.crosshair );

	Menu_AddItem( &s_preferences.menu, &s_preferences.simpleitems );
	Menu_AddItem( &s_preferences.menu, &s_preferences.wallmarks );
	Menu_AddItem( &s_preferences.menu, &s_preferences.brass );
	Menu_AddItem( &s_preferences.menu, &s_preferences.dynamiclights );
	Menu_AddItem( &s_preferences.menu, &s_preferences.identifytarget );
	Menu_AddItem( &s_preferences.menu, &s_preferences.highqualitysky );
	Menu_AddItem( &s_preferences.menu, &s_preferences.synceveryframe );
	Menu_AddItem( &s_preferences.menu, &s_preferences.forcemodel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawteamoverlay );
	Menu_AddItem( &s_preferences.menu, &s_preferences.allowdownload );
	Menu_AddItem( &s_preferences.menu, &s_preferences.botmenu);

	Menu_AddItem( &s_preferences.menu, &s_preferences.gibs);
	Menu_AddItem( &s_preferences.menu, &s_preferences.blood);
	Menu_AddItem( &s_preferences.menu, &s_preferences.deferplayer);
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawfps);
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawtimer);
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawlagometer);
	Menu_AddItem( &s_preferences.menu, &s_preferences.predictitems);
	Menu_AddItem( &s_preferences.menu, &s_preferences.smoothclient);
	Menu_AddItem( &s_preferences.menu, &s_preferences.shadows);
	Menu_AddItem( &s_preferences.menu, &s_preferences.newESCmenu);
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawAmmoWarning);
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawgun);
	Menu_AddItem( &s_preferences.menu, &s_preferences.teamchattime);
	Menu_AddItem( &s_preferences.menu, &s_preferences.teamchatheight);
	Menu_AddItem( &s_preferences.menu, &s_preferences.fov);
	Menu_AddItem( &s_preferences.menu, &s_preferences.zoomfov);
	Menu_AddItem( &s_preferences.menu, &s_preferences.mapOverBright);

	Menu_AddItem( &s_preferences.menu, &s_preferences.oldrail);
	Menu_AddItem( &s_preferences.menu, &s_preferences.oldplasma);
	Menu_AddItem( &s_preferences.menu, &s_preferences.oldrocket);
	Menu_AddItem( &s_preferences.menu, &s_preferences.truelightning);

	Menu_AddItem( &s_preferences.menu, &s_preferences.back );

	Preferences_SetMenuItems();
	Control_Init();
}


/*
===============
Preferences_Cache
===============
*/
void Preferences_Cache( void ) {
	int		n;

	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
		s_preferences.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
	}
}


/*
===============
UI_PreferencesMenu
===============
*/
void UI_PreferencesMenu( void ) {
	Preferences_MenuInit();
	UI_PushMenu( &s_preferences.menu );
}
