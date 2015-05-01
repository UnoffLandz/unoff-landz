#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <SDL_keysym.h>
#include "errors.h"
#include "keys.h"
#include "asc.h"
#include "init.h"
#include "misc.h"
#ifdef FASTER_MAP_LOAD
#include "io/elfilewrapper.h"
#endif

// default definitions for keys
uint32_t K_QUIT=ALT|'x';
#ifdef WINDOWS
// Windows SDL reports [Alt Gr] as [Ctrl], which hinders German users typing '@',
// so don't use Ctrl-q as a symbol to exit
uint32_t K_QUIT_ALT=ALT|'x';
#elif OSX
uint32_t K_QUIT_ALT=KMOD_LMETA|'q';
#else
uint32_t K_QUIT_ALT=CTRL|'q';
#endif
uint32_t K_CAMERAUP=SDLK_UP;
uint32_t K_CAMERADOWN=SDLK_DOWN;
uint32_t K_ZOOMOUT=SDLK_PAGEDOWN;
uint32_t K_ZOOMIN=SDLK_PAGEUP;
uint32_t K_TURNLEFT=SDLK_INSERT;
uint32_t K_TURNRIGHT=SDLK_DELETE;
uint32_t K_ADVANCE=SDLK_HOME;
uint32_t K_HEALTHBAR=ALT|'h';
uint32_t K_VIEWNAMES=ALT|'n';
uint32_t K_VIEWHP=ALT|'b';
uint32_t K_STATS=CTRL|'a';
uint32_t K_QUESTLOG=CTRL|'g';
uint32_t K_SESSION=CTRL|'z';
uint32_t K_WALK=CTRL|'w';
uint32_t K_LOOK=CTRL|'l';
uint32_t K_USE=CTRL|'u';
uint32_t K_OPTIONS=CTRL|'o';
uint32_t K_REPEATSPELL=CTRL|'r';
uint32_t K_SIGILS=CTRL|'s';
uint32_t K_MANUFACTURE=CTRL|'m';
uint32_t K_ITEMS=CTRL|'i';
uint32_t K_MAP=SDLK_TAB;
uint32_t K_MINIMAP=ALT|'m';
uint32_t K_ROTATELEFT=SDLK_LEFT;
uint32_t K_ROTATERIGHT=SDLK_RIGHT;
uint32_t K_FROTATELEFT=SHIFT|SDLK_LEFT;
uint32_t K_FROTATERIGHT=SHIFT|SDLK_RIGHT;
uint32_t K_BROWSER=SDLK_F2;
uint32_t K_BROWSERWIN=CTRL|SDLK_F2;
uint32_t K_ESCAPE=SDLK_ESCAPE;
uint32_t K_CONSOLE=SDLK_F1;
uint32_t K_SHADOWS=SDLK_F3;
uint32_t K_KNOWLEDGE=CTRL|'k';
uint32_t K_ENCYCLOPEDIA=CTRL|'e';
uint32_t K_HELP=CTRL|'h';
uint32_t K_NOTEPAD=CTRL|'n';
uint32_t K_HIDEWINS=ALT|'d';
uint32_t K_ITEM1=CTRL|'1';
uint32_t K_ITEM2=CTRL|'2';
uint32_t K_ITEM3=CTRL|'3';
uint32_t K_ITEM4=CTRL|'4';
uint32_t K_ITEM5=CTRL|'5';
uint32_t K_ITEM6=CTRL|'6';
uint32_t K_ITEM7=CTRL|'7';
uint32_t K_ITEM8=CTRL|'8';
uint32_t K_ITEM9=CTRL|'9';
uint32_t K_ITEM10=CTRL|'0';
uint32_t K_ITEM11=CTRL|'-';
uint32_t K_ITEM12=CTRL|'=';
uint32_t K_SCREENSHOT=CTRL|'p';
uint32_t K_VIEWTEXTASOVERTEXT=ALT|'o';
uint32_t K_AFK=CTRL|ALT|'a';
uint32_t K_SIT=ALT|'s';
uint32_t K_RANGINGLOCK=ALT|'r';
uint32_t K_BUDDY=CTRL|'b';
uint32_t K_NEXT_CHAT_TAB=CTRL|SDLK_PAGEDOWN;
uint32_t K_PREV_CHAT_TAB=CTRL|SDLK_PAGEUP;
uint32_t K_RULES=SDLK_F5;
uint32_t K_SPELL1=ALT|'1';
uint32_t K_SPELL2=ALT|'2';
uint32_t K_SPELL3=ALT|'3';
uint32_t K_SPELL4=ALT|'4';
uint32_t K_SPELL5=ALT|'5';
uint32_t K_SPELL6=ALT|'6';
uint32_t K_SPELL7=ALT|'7';
uint32_t K_SPELL8=ALT|'8';
uint32_t K_SPELL9=ALT|'9';
uint32_t K_SPELL10=ALT|'0';
uint32_t K_SPELL11=ALT|'-';
uint32_t K_SPELL12=ALT|'=';
uint32_t K_TABCOMPLETE=CTRL|' ';
uint32_t K_WINDOWS_ON_TOP=ALT|'w';
uint32_t K_MARKFILTER=CTRL|'f';
uint32_t K_OPAQUEWIN=CTRL|'d';
uint32_t K_GRAB_MOUSE=ALT|'g';
uint32_t K_FIRST_PERSON=ALT|'f';
uint32_t K_EXTEND_CAM=ALT|'e';
#ifndef OSX
uint32_t K_CUT=CTRL|'x';
uint32_t K_COPY=CTRL|'c';
uint32_t K_PASTE=CTRL|'v';
#else
uint32_t K_CUT=KMOD_LMETA|'x';
uint32_t K_COPY=KMOD_LMETA|'c';
uint32_t K_PASTE=KMOD_LMETA|'v';
#endif
uint32_t K_COPY_ALT=CTRL|SDLK_INSERT;
uint32_t K_PASTE_ALT=SHIFT|SDLK_INSERT;
#ifdef ECDEBUGWIN
uint32_t K_ECDEBUGWIN=ALT|CTRL|'c';
#endif
uint32_t K_EMOTES=CTRL|'j';
uint32_t K_RANGINGWIN=CTRL|'t';

// Remaining keys are not assigned to the keyboard but
// can be redefined or used by the #keypress command.
// They will get values at startup along with any keys
// undefined in key.ini
uint32_t K_COUNTERS=SDLK_UNKNOWN;
uint32_t K_HELPSKILLS=SDLK_UNKNOWN;

typedef struct
{
	char name[25];
	uint32_t *value;
} key_store_entry;

static key_store_entry key_store[] =
{
	{ "#K_QUIT", &K_QUIT },
	{ "#K_QUIT_ALT", &K_QUIT_ALT },
	{ "#K_CAMERAUP", &K_CAMERAUP },
	{ "#K_CAMERADOWN", &K_CAMERADOWN },
	{ "#K_ZOOMOUT", &K_ZOOMOUT },
	{ "#K_ZOOMIN", &K_ZOOMIN },
	{ "#K_TURNLEFT", &K_TURNLEFT },
	{ "#K_TURNRIGHT", &K_TURNRIGHT },
	{ "#K_ADVANCE", &K_ADVANCE },
	{ "#K_HEALTHBAR", &K_HEALTHBAR },
	{ "#K_VIEWNAMES", &K_VIEWNAMES },
	{ "#K_VIEWHP", &K_VIEWHP },
	{ "#K_STATS", &K_STATS },
	{ "#K_QUESTLOG", &K_QUESTLOG },
	{ "#K_SESSION", &K_SESSION },
	{ "#K_WALK", &K_WALK },
	{ "#K_LOOK", &K_LOOK },
	{ "#K_USE", &K_USE },
	{ "#K_OPTIONS", &K_OPTIONS },
	{ "#K_REPEATSPELL", &K_REPEATSPELL },
	{ "#K_SIGILS", &K_SIGILS },
	{ "#K_MANUFACTURE", &K_MANUFACTURE },
	{ "#K_ITEMS", &K_ITEMS },
	{ "#K_MAP", &K_MAP },
	{ "#K_MINIMAP", &K_MINIMAP },
	{ "#K_ROTATELEFT", &K_ROTATELEFT },
	{ "#K_ROTATERIGHT", &K_ROTATERIGHT },
	{ "#K_FROTATELEFT", &K_FROTATELEFT },
	{ "#K_FROTATERIGHT", &K_FROTATERIGHT },
	{ "#K_BROWSER", &K_BROWSER },
	{ "#K_BROWSERWIN", &K_BROWSERWIN },
	{ "#K_ESCAPE", &K_ESCAPE },
	{ "#K_CONSOLE", &K_CONSOLE },
	{ "#K_SHADOWS", &K_SHADOWS },
	{ "#K_KNOWLEDGE", &K_KNOWLEDGE },
	{ "#K_ENCYCLOPEDIA", &K_ENCYCLOPEDIA },
	{ "#K_HELP", &K_HELP },
	{ "#K_NOTEPAD", &K_NOTEPAD },
	{ "#K_HIDEWINS", &K_HIDEWINS },
	{ "#K_ITEM1", &K_ITEM1 },
	{ "#K_ITEM2", &K_ITEM2 },
	{ "#K_ITEM3", &K_ITEM3 },
	{ "#K_ITEM4", &K_ITEM4 },
	{ "#K_ITEM5", &K_ITEM5 },
	{ "#K_ITEM6", &K_ITEM6 },
	{ "#K_ITEM7", &K_ITEM7 },
	{ "#K_ITEM8", &K_ITEM8 },
	{ "#K_ITEM9", &K_ITEM9 },
	{ "#K_ITEM10", &K_ITEM10 },
	{ "#K_ITEM11", &K_ITEM11 },
	{ "#K_ITEM12", &K_ITEM12 },
	{ "#K_SCREENSHOT", &K_SCREENSHOT },
	{ "#K_VIEWTEXTASOVERTEXT", &K_VIEWTEXTASOVERTEXT },
	{ "#K_AFK", &K_AFK },
	{ "#K_SIT", &K_SIT },
	{ "#K_RANGINGLOCK", &K_RANGINGLOCK },
	{ "#K_BUDDY", &K_BUDDY },
	{ "#K_NEXT_CHAT_TAB", &K_NEXT_CHAT_TAB },
	{ "#K_PREV_CHAT_TAB", &K_PREV_CHAT_TAB },
	{ "#K_RULES", &K_RULES },
	{ "#K_SPELL1", &K_SPELL1 },
	{ "#K_SPELL2", &K_SPELL2 },
	{ "#K_SPELL3", &K_SPELL3 },
	{ "#K_SPELL4", &K_SPELL4 },
	{ "#K_SPELL5", &K_SPELL5 },
	{ "#K_SPELL6", &K_SPELL6 },
	{ "#K_SPELL7", &K_SPELL7 },
	{ "#K_SPELL8", &K_SPELL8 },
	{ "#K_SPELL9", &K_SPELL9 },
	{ "#K_SPELL10", &K_SPELL10 },
	{ "#K_SPELL11", &K_SPELL11 },
	{ "#K_SPELL12", &K_SPELL12 },
	{ "#K_TABCOMPLETE", &K_TABCOMPLETE },
	{ "#K_WINDOWS_ON_TOP", &K_WINDOWS_ON_TOP },
	{ "#K_MARKFILTER", &K_MARKFILTER },
	{ "#K_OPAQUEWIN", &K_OPAQUEWIN },
	{ "#K_GRAB_MOUSE", &K_GRAB_MOUSE },
	{ "#K_FIRST_PERSON", &K_FIRST_PERSON },
	{ "#K_EXTEND_CAM", &K_EXTEND_CAM },
	{ "#K_CUT", &K_CUT },
	{ "#K_COPY", &K_COPY },
	{ "#K_PASTE", &K_PASTE },
	{ "#K_COPY_ALT", &K_COPY_ALT },
	{ "#K_PASTE_ALT", &K_PASTE_ALT },
#ifdef ECDEBUGWIN
	{ "#K_ECDEBUGWIN", &K_ECDEBUGWIN },
#endif
	{ "#K_EMOTES", &K_EMOTES },
	{ "#K_RANGINGWIN", &K_RANGINGWIN },
	{ "#K_COUNTERS", &K_COUNTERS },
	{ "#K_HELPSKILLS", &K_HELPSKILLS }
};


uint32_t get_key_value(const char* name)
{
	size_t num_keys = sizeof(key_store)/sizeof(key_store_entry);
	size_t i;
	if ((name == NULL) || strlen(name) == 0)
	{
		LOG_ERROR("%s() empty name\n", __FUNCTION__);
		return 0;
	}
	for (i=0; i<num_keys; i++)
	{
		if (strcasecmp(name, key_store[i].name) == 0)
			return *key_store[i].value;
	}
	return 0;
}

static void add_key(uint32_t *key, uint32_t n)
{
	switch (n)
	{
		case 303:
		case 304:
			*key |= SHIFT;
			break;
		case 305:
		case 306:
			*key |= CTRL;
			break;
		case 307:
		case 308:
			*key |= ALT;
			break;
		default:
			*key = (n & 0xFFFF) | (*key &0xFFFF0000);
        }
}

static uint32_t CRC32(const char *data, int len)
{
	unsigned int result = 0;
	int i, j;
    uint8_t octet;

	for (i = 0; i < len; i++)
	{
		octet = *(data++);
		for (j = 0; j < 8; j++)
		{
			if ((octet >> 7) ^ (result >> 31))
				result = (result << 1) ^ 0x04c11db7;
			else
				result = (result << 1);
			octet <<= 1;
		}
	}

	return ~result;
}

static uint16_t get_key_code(const char *key)
{
	int len = strlen(key);

	if (len==1)
	{
		return tolower(key[0]);
	}
	else
	{
		uint32_t crc = CRC32(key,len);
		switch(crc){
			case 0x414243d2: //UP
				return 273;
			case 0x8b9c5c32: //F1
				return 282;
			case 0x86df7aeb: //F2
				return 283;
			case 0x821e675c: //F3
				return 284;
			case 0x9c593759: //F4
				return 285;
			case 0x98982aee: //F5
				return 286;
			case 0x95db0c37: //F6
				return 287;
			case 0x911a1180: //F7
				return 288;
			case 0xa955ac3d: //F8
				return 289;
			case 0xad94b18a: //F9
				return 290;
			case 0xbbde3454: //F10
				return 291;
			case 0xbf1f29e3: //F11
				return 292;
			case 0xb25c0f3a: //F12
				return 293;
			case 0xb69d128d: //F13
				return 294;
			case 0xa8da4288: //F14
				return 295;
			case 0xac1b5f3f: //F15
				return 296;
			case 0xe5b332af: //BACKSPACE
				return 8;
			case 0x3d6742da: //TAB
				return 9;
			case 0xe4f512ce: //CLEAR
				return 12;
			case 0xe5c642f: //RETURN
				return 13;
			case 0x1a3dbcf4: //PAUSE
				return 19;
			case 0xb23e322f: //ESCAPE
				return 27;
			case 0xe0ea4208: //SPACE
				return 32;
			case 0x3f048816: //DELETE
				return 127;
			case 0x5dd541: //KP0
				return 256;
			case 0x49cc8f6: //KP1
				return 257;
			case 0x9dfee2f: //KP2
				return 258;
			case 0xd1ef398: //KP3
				return 259;
			case 0x1359a39d: //KP4
				return 260;
			case 0x1798be2a: //KP5
				return 261;
			case 0x1adb98f3: //KP6
				return 262;
			case 0x1e1a8544: //KP7
				return 263;
			case 0x265538f9: //KP8
				return 264;
			case 0x2294254e: //KP9
				return 265;
			case 0xc9681663: //KP_PERIOD
				return 266;
			case 0xf2032002: //KP_DIVIDE
				return 267;
			case 0xc69c9177: //KP_MULTIPLY
				return 268;
			case 0xe05a3b75: //KP_MINUS
				return 269;
			case 0x7a14ede0: //KP_PLUS
				return 270;
			case 0xb95fb1fa: //KP_ENTER
				return 271;
			case 0x997d27b6: //KP_EQUALS
				return 272;
			case 0x412c789a: //DOWN
				return 274;
			case 0xcfd43bcf: //RIGHT
				return 275;
			case 0x14618acf: //LEFT
				return 276;
			case 0xb448467c: //INSERT
				return 277;
			case 0xd59321ba: //HOME
				return 278;
			case 0x863456b7: //END
				return 279;
			case 0xd541afe1: //PAGEUP
				return 280;
			case 0x77a53c61: //PAGEDOWN
				return 281;
			case 0x8563dfd4: //NUMLOCK
				return 300;
			case 0x4b601de5: //CAPSLOCK
				return 301;
			case 0x7b642f: //SCROLLOCK
				return 302;
			case 0x6fa8765e: //RSHIFT
				return 303;
			case 0x5a59f8b9: //LSHIFT
				return 304;
			case 0xc535c663: //RCTRL
				return 305;
			case 0xb5e083f0: //LCTRL
				return 306;
			case 0xf7a834fb: //RALT
				return 307;
			case 0x39b9e58e: //LALT
				return 308;
			case 0x34796737: //RMETA
				return 309;
			case 0x44ac22a4: //LMETA
				return 310;
			case 0x8ec5890c: //LSUPER
				return 311;
			case 0xbb3407eb: //RSUPER
				return 312;
			case 0x2d5a7586: //MODE
				return 313;
			case 0x87140862: //COMPOSE
				return 314;
			case 0x512a6d4b: //HELP
				return 315;
			case 0xdc87c39e: //PRINT
				return 316;
			case 0xbdf2d984: //SYSREQ
				return 317;
			case 0xd318f49: //BREAK
				return 318;
			case 0x46854e9d: //MENU
				return 319;
			case 0x8758b6ec: //POWER
				return 320;
			case 0x1e43eaa9: //EURO
				return 321;
			case 0xdf6ba7e: //UNDO
				return 322;
			default:
				return SDLK_UNKNOWN;
		}
	}
}

static void parse_key_line(const char *line)
{
	char kstr[100], t1[100], t2[100], t3[100], t4[100];
	uint32_t key = 0;
	int nkey = sscanf(line, " #K_%99s = %99s %99s %99s %99s", kstr,
		t1, t2, t3, t4);
	size_t num_keys = sizeof(key_store)/sizeof(key_store_entry);
	size_t i;

	if (nkey <= 1)
		return;

	add_key(&key, get_key_code(t1));
	if (nkey > 2 && t2[0] != '#')
	{
		add_key(&key, get_key_code(t2));
		if (nkey > 3 && t3[0] != '#')
		{
			add_key(&key, get_key_code(t3));
			if (nkey > 4 && t4[0] != '#')
			{
				add_key(&key, get_key_code(t4));
			}
		}
	}

	for (i=0; i<num_keys; i++)
		if (strcasecmp(kstr, &key_store[i].name[3]) == 0) // skip "#K_"
		{
			*key_store[i].value = key;
			break;
		}
}

// load the dynamic definitions for keys
void read_key_config()
{
	char line[512];
	el_file_ptr f;
	size_t num_keys = sizeof(key_store)/sizeof(key_store_entry);
	uint32_t last_key_value = SDLK_LAST;
	size_t i;

	f = el_open_custom("key.ini");
	if (f)
	{
		while (el_fgets(line, sizeof(line), f))
			parse_key_line(line);
		el_close(f);
	}

	// look for unassigned keys and assign one up from SDLK_LAST
	for (i=0; i<num_keys; i++)
		if (*key_store[i].value == 0)
			*key_store[i].value = ++last_key_value;
}

// Returns (in the buffer provided) a string describing the specified keydef.
const char *get_key_string(uint32_t keydef, char *buf, size_t buflen)
{
	char base = keydef & 0xFF;
    const char *mod = "";
	if (keydef & CTRL)
		mod = "ctrl-";
	else if (keydef & ALT)
		mod = "alt-";
	else if (keydef & SHIFT)
		mod = "shift-";
	safe_snprintf(buf, buflen, "%s%c", mod, base);
	return buf;
}
