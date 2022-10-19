// Copyright (C) 1999-2000 Id Software, Inc.
//




#include "ui_local.h"

#define ART_FRAMEL			"menu/art/frame2_l"
#define ART_FRAMER			"menu/art/frame1_r"
#define ART_MODEL0			"menu/art/model_0"
#define ART_MODEL1			"menu/art/model_1"
#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"
#define ART_FX_BASE			"menu/art/fx_base"
#define ART_FX_BLUE			"menu/art/fx_blue"
#define ART_FX_CYAN			"menu/art/fx_cyan"
#define ART_FX_GREEN		"menu/art/fx_grn"
#define ART_FX_RED			"menu/art/fx_red"
#define ART_FX_TEAL			"menu/art/fx_teal"
#define ART_FX_WHITE		"menu/art/fx_white"
#define ART_FX_YELLOW		"menu/art/fx_yel"

#define ID_NAME			10
#define ID_HANDICAP		11
#define ID_EFFECTS		12
#define ID_BACK			13
#define ID_MODEL		14
#define ID_MODELTYPE	15
#define ID_EFFECTS2		16

#define MAX_NAMELENGTH	20

#define MENUTEXT_COLUMN 144

#define TEXT_RESCALE 0.75

typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menufield_s			name;
	menulist_s			handicap;
	menulist_s			effects;
	menulist_s			effects2;

	menubitmap_s		back;
	menubitmap_s		model;
	menubitmap_s		item_null;
	menutext_s			modeltype;

	qhandle_t			fxBasePic;
	qhandle_t			fxPic[7];
	int					current_fx;
	modelAnim_t			player;

	awardInfo_t			awards;
} playersettings_t;


static playersettings_t	s_playersettings;

static int gamecodetoui[] = {4,2,3,0,5,1,6};
static int uitogamecode[] = {4,6,2,3,1,5,7};

static const char *handicap_items[] = {
	"None",
	"95",
	"90",
	"85",
	"80",
	"75",
	"70",
	"65",
	"60",
	"55",
	"50",
	"45",
	"40",
	"35",
	"30",
	"25",
	"20",
	"15",
	"10",
	"5",
	0
};



/*
=================
PlayerSettings_SetPlayerModelType
=================
*/
static void PlayerSettings_SetPlayerModelType( void )
{
	if (UIE_PlayerInfo_IsTeamModel())
	{
		s_playersettings.modeltype.string = "Team Model";
	}
	else
	{
		s_playersettings.modeltype.string = "DM Model";
	}

	PText_Init(&s_playersettings.modeltype);
}



/*
=================
PlayerSettings_ToggleModelType
=================
*/
static void PlayerSettings_ToggleModelType( void )
{
	qboolean type;

	if (UIE_PlayerInfo_IsTeamModel()) {
		type = qfalse;
	}
	else {
		type = qtrue;
	}

	UIE_PlayerInfo_DrawTeamModel(&s_playersettings.player, type);
	PlayerSettings_SetPlayerModelType();
}




/*
=================
PlayerSettings_DrawName
=================
*/
static void PlayerSettings_DrawName( void *self ) {
	menufield_s		*f;
	qboolean		focus;
	int				style;
	char			*txt;
	char			c;
	float			*color;
	int				n;
	int				basex, x, y;
	char			name[32];

	f = (menufield_s*)self;
	basex = f->generic.x;
	y = f->generic.y;
	focus = (f->generic.parent->cursor == f->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawScaledProportionalString( basex, y, "Name", style, TEXT_RESCALE, color );
//	UI_DrawProportionalString( basex, y, "Name", style, color );

	// draw the actual name
	basex += 64;
	y += PROP_HEIGHT;
	txt = f->field.buffer;
	color = g_color_table[ColorIndex(COLOR_WHITE)];
	x = basex;
	while ( (c = *txt) != 0 ) {
		if ( !focus && Q_IsColorString( txt ) ) {
			n = ColorIndex( *(txt+1) );
			if( n == 0 ) {
				n = 7;
			}
			color = g_color_table[n];
			txt += 2;
			continue;
		}
		UI_DrawChar( x, y, c, style, color );
		txt++;
		x += SMALLCHAR_WIDTH;
	}

	// draw cursor if we have focus
	if( focus ) {
		if ( trap_Key_GetOverstrikeMode() ) {
			c = 11;
		} else {
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar( basex + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color_white );
	}

	// draw at bottom also using proportional font
	Q_strncpyz( name, f->field.buffer, sizeof(name) );
	Q_CleanStr( name );
	UI_DrawProportionalString( 320, 440, name, UI_CENTER|UI_BIGFONT, text_color_normal );
}


/*
=================
PlayerSettings_DrawHandicap
=================
*/
static void PlayerSettings_DrawHandicap( void *self ) {
	menulist_s		*item;
	qboolean		focus;
	int				style;
	float			*color;

	item = (menulist_s *)self;
	focus = (item->generic.parent->cursor == item->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawScaledProportionalString( item->generic.x, item->generic.y, "Handicap", style, TEXT_RESCALE, color );
//	UI_DrawProportionalString( item->generic.x, item->generic.y, "Handicap", style, color );
	UI_DrawProportionalString( item->generic.x + 64, item->generic.y + PROP_HEIGHT, handicap_items[item->curvalue], style, color );
}


/*
=================
PlayerSettings_DrawEffects
=================
*/
static void PlayerSettings_DrawEffects( void *self ) {
	menulist_s		*item;
	qboolean		focus;
	int				style;
	float			*color;
	float			scale;
	int				textlen;

	item = (menulist_s *)self;
	focus = (item->generic.parent->cursor == item->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

//	UI_DrawProportionalString( item->generic.x, item->generic.y, "Effects", style, color );
	scale = TEXT_RESCALE;
	if (item->generic.id == ID_EFFECTS) {
		UI_DrawScaledProportionalString( item->generic.x, item->generic.y, "Rail core:", style, scale, color );
	} else {
		UI_DrawScaledProportionalString( item->generic.x, item->generic.y, "Rail ring:", style, scale, color );
	}

	textlen = UI_ProportionalStringWidth("Rail core:") * UI_ProportionalSizeScale(style) * scale + 14;

	UI_DrawHandlePic( item->generic.x + textlen, item->generic.y + 4, 128, 8, s_playersettings.fxBasePic );
	UI_DrawHandlePic( item->generic.x + textlen + item->curvalue * 16 + 8, item->generic.y + 2, 16, 12, s_playersettings.fxPic[item->curvalue] );
}


/*
=================
PlayerSettings_DrawPlayer
=================
*/
static void PlayerSettings_DrawPlayer( void *self ) {
	UIE_PlayerInfo_AnimateModel(&s_playersettings.player);
}


/*
=================
PlayerSettings_DrawMenu
=================
*/
static void PlayerSettings_MenuDraw(void)
{
	UIE_AwardIcons_DrawValues(&s_playersettings.awards);

	if (uis.firstdraw)
		PlayerSettings_SetPlayerModelType();

	// standard menu drawing
	Menu_Draw( &s_playersettings.menu );
}


/*
=================
PlayerSettings_SaveChanges
=================
*/
static void PlayerSettings_SaveChanges( void ) {
	// name
	trap_Cvar_Set( "name", s_playersettings.name.field.buffer );

	// handicap
	trap_Cvar_SetValue( "handicap", 100 - s_playersettings.handicap.curvalue * 5 );

	// effects color
	trap_Cvar_SetValue( "color1", uitogamecode[s_playersettings.effects.curvalue] );
	trap_Cvar_SetValue( "color2", uitogamecode[s_playersettings.effects2.curvalue] );
}


/*
=================
PlayerSettings_MenuKey
=================
*/
static sfxHandle_t PlayerSettings_MenuKey( int key ) {
	if( key == K_MOUSE2 || key == K_ESCAPE ) {
		PlayerSettings_SaveChanges();
	}
	return Menu_DefaultKey( &s_playersettings.menu, key );
}


/*
=================
PlayerSettings_SetMenuItems
=================
*/
static void PlayerSettings_SetMenuItems( void ) {
	vec3_t	viewangles;
	int		c;
	int		h;

	// name
	Q_strncpyz( s_playersettings.name.field.buffer, UI_Cvar_VariableString("name"), sizeof(s_playersettings.name.field.buffer) );

	// effects color
	c = trap_Cvar_VariableValue( "color1" ) - 1;
	if( c < 0 || c > 6 ) {
		c = 6;
	}
	s_playersettings.effects.curvalue = gamecodetoui[c];

	// effects color 2
	c = trap_Cvar_VariableValue( "color2" ) - 1;
	if( c < 0 || c > 6 ) {
		c = 6;
	}
	s_playersettings.effects2.curvalue = gamecodetoui[c];

	// model/skin
	UIE_PlayerInfo_InitModel(&s_playersettings.player);

	// handicap
	h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
	s_playersettings.handicap.curvalue = 20 - h / 5;
}


/*
=================
PlayerSettings_AwardEvent
=================
*/
static void PlayerSettings_AwardEvent( void* ptr, int notification ) {
	int		n;

	if (notification != QM_ACTIVATED) {
		return;
	}

	UIE_AwardIcons_PlaySound(&s_playersettings.awards, (menucommon_s*)ptr);
}



/*
=================
PlayerSettings_MenuEvent
=================
*/
static void PlayerSettings_MenuEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_HANDICAP:
		trap_Cvar_Set( "handicap", va( "%i", 100 - 25 * s_playersettings.handicap.curvalue ) );
		break;

	case ID_MODEL:
		PlayerSettings_SaveChanges();
		UI_PlayerModelMenu();
		break;

	case ID_MODELTYPE:
		PlayerSettings_ToggleModelType();
		break;

	case ID_BACK:
		PlayerSettings_SaveChanges();
		UI_PopMenu();
		break;
	}
}


/*
=================
PlayerSettings_MenuInit
=================
*/
static void PlayerSettings_MenuInit( void ) {
	int		y;
	float 	sizeScale;

	memset(&s_playersettings,0,sizeof(playersettings_t));

	s_playersettings.menu.fullscreen = qtrue;
	s_playersettings.menu.wrapAround = qtrue;
	s_playersettings.menu.draw = PlayerSettings_MenuDraw;

	PlayerSettings_Cache();

	s_playersettings.menu.key        = PlayerSettings_MenuKey;
	s_playersettings.menu.wrapAround = qtrue;
	s_playersettings.menu.fullscreen = qtrue;

	sizeScale = UI_ProportionalSizeScale( UI_SMALLFONT );

	s_playersettings.banner.generic.type  = MTYPE_BTEXT;
	s_playersettings.banner.generic.x     = 320;
	s_playersettings.banner.generic.y     = 16;
	s_playersettings.banner.string        = "PLAYER SETTINGS";
	s_playersettings.banner.color         = color_white;
	s_playersettings.banner.style         = UI_CENTER;

	s_playersettings.framel.generic.type  = MTYPE_BITMAP;
	s_playersettings.framel.generic.name  = ART_FRAMEL;
	s_playersettings.framel.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playersettings.framel.generic.x     = 0;
	s_playersettings.framel.generic.y     = 78;
	s_playersettings.framel.width         = 256;
	s_playersettings.framel.height        = 329;

	s_playersettings.framer.generic.type  = MTYPE_BITMAP;
	s_playersettings.framer.generic.name  = ART_FRAMER;
	s_playersettings.framer.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playersettings.framer.generic.x     = 376;
	s_playersettings.framer.generic.y     = 76;
	s_playersettings.framer.width         = 256;
	s_playersettings.framer.height        = 334;

	y = 112;
	s_playersettings.name.generic.type			= MTYPE_FIELD;
	s_playersettings.name.generic.flags			= QMF_NODEFAULTINIT;
	s_playersettings.name.generic.ownerdraw		= PlayerSettings_DrawName;
	s_playersettings.name.field.widthInChars	= MAX_NAMELENGTH;
	s_playersettings.name.field.maxchars		= MAX_NAMELENGTH;
	s_playersettings.name.generic.x				= MENUTEXT_COLUMN;
	s_playersettings.name.generic.y				= y;
	s_playersettings.name.generic.left			= MENUTEXT_COLUMN - 8;
	s_playersettings.name.generic.top			= y - 8;
	s_playersettings.name.generic.right			= MENUTEXT_COLUMN + 250;
	s_playersettings.name.generic.bottom		= y + 2 * PROP_HEIGHT;

	y += 5 * PROP_HEIGHT / 2;
	s_playersettings.handicap.generic.type		= MTYPE_SPINCONTROL;
	s_playersettings.handicap.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.handicap.generic.id		= ID_HANDICAP;
	s_playersettings.handicap.generic.ownerdraw	= PlayerSettings_DrawHandicap;
	s_playersettings.handicap.generic.x			= MENUTEXT_COLUMN;
	s_playersettings.handicap.generic.y			= y;
	s_playersettings.handicap.generic.left		= MENUTEXT_COLUMN - 8;
	s_playersettings.handicap.generic.top		= y - 8;
	s_playersettings.handicap.generic.right		= MENUTEXT_COLUMN + 250;
	s_playersettings.handicap.generic.bottom	= y + 2 * PROP_HEIGHT;
	s_playersettings.handicap.numitems			= 20;

/*	y += 5 * PROP_HEIGHT / 2;
	s_playersettings.effects.generic.type		= MTYPE_SPINCONTROL;
	s_playersettings.effects.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.effects.generic.id			= ID_EFFECTS;
	s_playersettings.effects.generic.ownerdraw	= PlayerSettings_DrawEffects;
	s_playersettings.effects.generic.x			= MENUTEXT_COLUMN;
	s_playersettings.effects.generic.y			= y;
	s_playersettings.effects.generic.left		= MENUTEXT_COLUMN - 8;
	s_playersettings.effects.generic.top		= y - 8;
	s_playersettings.effects.generic.right		= MENUTEXT_COLUMN + 250;
	s_playersettings.effects.generic.bottom		= y + 2* PROP_HEIGHT;
	s_playersettings.effects.numitems			= 7;
*/

	y += 5 * PROP_HEIGHT / 2;
	s_playersettings.effects.generic.type		= MTYPE_SPINCONTROL;
	s_playersettings.effects.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.effects.generic.id			= ID_EFFECTS;
	s_playersettings.effects.generic.ownerdraw	= PlayerSettings_DrawEffects;
	s_playersettings.effects.generic.x			= MENUTEXT_COLUMN;
	s_playersettings.effects.generic.y			= y;
	s_playersettings.effects.generic.left		= MENUTEXT_COLUMN - 8;
	s_playersettings.effects.generic.top		= y - 4;
	s_playersettings.effects.generic.right		= MENUTEXT_COLUMN + 250;
	s_playersettings.effects.generic.bottom		= y + PROP_HEIGHT;
	s_playersettings.effects.numitems			= 7;

	y += 3 * PROP_HEIGHT/2;
	s_playersettings.effects2.generic.type		= MTYPE_SPINCONTROL;
	s_playersettings.effects2.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.effects2.generic.id			= ID_EFFECTS2;
	s_playersettings.effects2.generic.ownerdraw	= PlayerSettings_DrawEffects;
	s_playersettings.effects2.generic.x			= MENUTEXT_COLUMN;
	s_playersettings.effects2.generic.y			= y;
	s_playersettings.effects2.generic.left		= MENUTEXT_COLUMN - 8;
	s_playersettings.effects2.generic.top		= y - 4;
	s_playersettings.effects2.generic.right		= MENUTEXT_COLUMN + 250;
	s_playersettings.effects2.generic.bottom		= y + PROP_HEIGHT;
	s_playersettings.effects2.numitems			= 7;


	s_playersettings.model.generic.type			= MTYPE_BITMAP;
	s_playersettings.model.generic.name			= ART_MODEL0;
	s_playersettings.model.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.model.generic.id			= ID_MODEL;
	s_playersettings.model.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.model.generic.x			= 640;
	s_playersettings.model.generic.y			= 480-64;
	s_playersettings.model.width				= 128;
	s_playersettings.model.height				= 64;
	s_playersettings.model.focuspic				= ART_MODEL1;

	s_playersettings.player.bitmap.generic.type		= MTYPE_BITMAP;
	s_playersettings.player.bitmap.generic.flags		= QMF_INACTIVE;
	s_playersettings.player.bitmap.generic.ownerdraw	= PlayerSettings_DrawPlayer;
	s_playersettings.player.bitmap.generic.x			= PLAYERMODEL_X;
	s_playersettings.player.bitmap.generic.y			= PLAYERMODEL_Y;
	s_playersettings.player.bitmap.width				= PLAYERMODEL_WIDTH;
	s_playersettings.player.bitmap.height				= PLAYERMODEL_HEIGHT;

	s_playersettings.back.generic.type			= MTYPE_BITMAP;
	s_playersettings.back.generic.name			= ART_BACK0;
	s_playersettings.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.back.generic.id			= ID_BACK;
	s_playersettings.back.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.back.generic.x				= 0;
	s_playersettings.back.generic.y				= 480-64;
	s_playersettings.back.width					= 128;
	s_playersettings.back.height				= 64;
	s_playersettings.back.focuspic				= ART_BACK1;

	s_playersettings.item_null.generic.type		= MTYPE_BITMAP;
	s_playersettings.item_null.generic.flags	= QMF_LEFT_JUSTIFY|QMF_MOUSEONLY|QMF_SILENT;
	s_playersettings.item_null.generic.x		= 0;
	s_playersettings.item_null.generic.y		= 0;
	s_playersettings.item_null.width			= 640;
	s_playersettings.item_null.height			= 480;

	s_playersettings.modeltype.generic.type			= MTYPE_PTEXT;
	s_playersettings.modeltype.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.modeltype.generic.x				= 480;
	s_playersettings.modeltype.generic.y				= PLAYERMODEL_TEXTHEIGHT + PROP_HEIGHT*sizeScale;
	s_playersettings.modeltype.generic.id				= ID_MODELTYPE;
	s_playersettings.modeltype.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.modeltype.string					= "";
	s_playersettings.modeltype.color					= text_color_normal;
	s_playersettings.modeltype.style					= UI_CENTER|UI_DROPSHADOW|UI_SMALLFONT;

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.banner );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.framel );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.framer );

	// setup and add award values
	s_playersettings.awards.bCentre = qtrue;
	s_playersettings.awards.iconOffset = 0;
	s_playersettings.awards.iconRaise = 24;
	UIE_AwardIcons_InitControls(&s_playersettings.menu, &s_playersettings.awards, PlayerSettings_AwardEvent);

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.name );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.handicap );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.effects );
 	Menu_AddItem( &s_playersettings.menu, &s_playersettings.effects2 );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.model );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.back );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.modeltype );

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.player.bitmap );

	// kills selected item when cursor moves off it, no item
	// registered after this will activate
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.item_null );

	PlayerSettings_SetMenuItems();
}


/*
=================
PlayerSettings_Cache
=================
*/
void PlayerSettings_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_MODEL0 );
	trap_R_RegisterShaderNoMip( ART_MODEL1 );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );

	UIE_AwardIcons_Cache(&s_playersettings.awards);

	s_playersettings.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
	s_playersettings.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
	s_playersettings.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
	s_playersettings.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
	s_playersettings.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_TEAL );
	s_playersettings.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
	s_playersettings.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
	s_playersettings.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
}


/*
=================
UI_PlayerSettingsMenu
=================
*/
void UI_PlayerSettingsMenu( void ) {
	PlayerSettings_MenuInit();
	UI_PushMenu( &s_playersettings.menu );
}
