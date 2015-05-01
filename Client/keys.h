/*!
 * \file
 * \ingroup hotkey
 * \brief Handling of hotkeys.
 */
#ifndef __KEYS_H__
#define __KEYS_H__

#include <SDL_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \name Key Modifiers
 */
/*! @{ */
#define SHIFT (1 << 31)   /*!< Shift modifier is pressed */
#define CTRL (1 << 30)    /*!< Ctrl modifier is pressed */
#define ALT (1 << 29)     /*!< Alt modifier is pressed */
/*! @} */

/*!
 * \name Predefined keys
 */
/*! @{ */
extern uint32_t K_QUIT;           /*!< key for closing the game */
extern uint32_t K_QUIT_ALT;       /*!< alternative key for closing the game */
extern uint32_t K_CAMERAUP;       /*!< key for changing the view angle of the camera up */
extern uint32_t K_CAMERADOWN;     /*!< key for changing the view angle of the camera down */
extern uint32_t K_ZOOMOUT;        /*!< key used for zooming out */
extern uint32_t K_ZOOMIN;         /*!< key used for zooming in */
extern uint32_t K_TURNLEFT;       /*!< key used to turn left */
extern uint32_t K_TURNRIGHT;      /*!< key used to turn right */
extern uint32_t K_ADVANCE;        /*!< key used to move one step (tile) forward */
extern uint32_t K_HEALTHBAR;      /*!< key used to toggle display of the healthbar */
extern uint32_t K_VIEWNAMES;      /*!< key used to toggle display of names */
extern uint32_t K_VIEWHP;         /*!< key used to toggle display of hitpoints (material points) */
extern uint32_t K_STATS;          /*!< key to toggle display of \ref stats_window */
extern uint32_t K_QUESTLOG;       /*!< key to toggle display of \ref questlog_window */
extern uint32_t K_SESSION;        /*!< key to toggle display of session tab in stats_window */
extern uint32_t K_WALK;           /*!< key used to switch to walk cursor */
extern uint32_t K_LOOK;           /*!< key used to switch to look cursor */
extern uint32_t K_USE;            /*!< key used to switch to use cursor */
extern uint32_t K_OPTIONS;        /*!< key used to toggle display of \ref options_window */
extern uint32_t K_REPEATSPELL;    /*!< key used to repeat the last spell used */
extern uint32_t K_SIGILS;         /*!< key used to toggle display of \ref spells_window */
extern uint32_t K_MANUFACTURE;    /*!< key used to toggle display of \ref manufacture_window */
extern uint32_t K_ITEMS;          /*!< key used to toggle display of \ref items_window */
extern uint32_t K_MAP;            /*!< key used to go into \ref interface_map mode */
extern uint32_t K_MINIMAP;        /*!< key used to open minimap window */
extern uint32_t K_ROTATELEFT;     /*!< key used to rotate left (counter clockwise) */
extern uint32_t K_ROTATERIGHT;    /*!< key used to rotate right (clockwise) */
extern uint32_t K_FROTATELEFT;    /*!< key used to fine rotate left (counter clockwise) */
extern uint32_t K_FROTATERIGHT;   /*!< key used to fine rotate right (clockwise) */
extern uint32_t K_BROWSER;        /*!< key used to open the browser using the last displayed URL */
extern uint32_t K_BROWSERWIN;     /*!< key used to open the URL list window */
extern uint32_t K_ESCAPE;         /*!< key used to try fleeing in combat */
extern uint32_t K_CONSOLE;        /*!< key used to toggle \ref interface_console mode */
extern uint32_t K_SHADOWS;        /*!< key used to toggle rendering of shadows */
extern uint32_t K_KNOWLEDGE;      /*!< key used to toggle display of \ref knowledge_window */
extern uint32_t K_ENCYCLOPEDIA;   /*!< key used to toggle display of \ref encyclopedia_window */
extern uint32_t K_HELP;           /*!< key used to toggle display of \ref help_window */
extern uint32_t K_RULES;          /*!< key used to toggle display of \ref rules_window */
extern uint32_t K_NOTEPAD;        /*!< key used to toggle display of \ref notepad_window */
extern uint32_t K_HIDEWINS;       /*!< key used to hide all open windows */
extern uint32_t K_SCREENSHOT;         /*!< key used to make a screenshot */
extern uint32_t K_VIEWTEXTASOVERTEXT; /*!< key used to toggle display of text bubbles (overtext) */
extern uint32_t K_AFK;                /*!< key used to display AFK messages */
extern uint32_t K_SIT;                /*!< key used to toggle sitting status, i.e. sit down/stand up */
extern uint32_t K_RANGINGLOCK;        /*!< key used to toggle ranging-lock status */
extern uint32_t K_BUDDY;              /*!< key used to toggle the buddy window */
extern uint32_t K_NEXT_CHAT_TAB;      /*!< key used to switch to next chat tab */
extern uint32_t K_PREV_CHAT_TAB;      /*!< key used to switch to previous tab */
extern uint32_t K_TABCOMPLETE;        /*!< key used to autocomplete commands/buddy names */
extern uint32_t K_WINDOWS_ON_TOP;     /*!< key used to toggle the windows_on_top option */
extern uint32_t K_MARKFILTER;         /*!< key used to toggle the TAB map mark filtering function */
extern uint32_t K_OPAQUEWIN;          /*!< key used to toggle window opacity */
extern uint32_t K_GRAB_MOUSE;         /*!< key used to toggle whether mouse is in HUD or camera control mode */
extern uint32_t K_FIRST_PERSON;       /*!< key used to toggle first person/third person view mode*/
extern uint32_t K_EXTEND_CAM;         /*!< key used to toggle extended camera mode*/
extern uint32_t K_CUT;                /*!< key used to cut text and copy it into a clipboard */ 
extern uint32_t K_COPY;               /*!< key used to copy text into a clipboard */ 
extern uint32_t K_PASTE;              /*!< key used to copy text from a clipboard into EL */
extern uint32_t K_COPY_ALT;           /*!< alternative key used to copy text into a clipboard */ 
extern uint32_t K_PASTE_ALT;          /*!< alternative key used to copy text from a clipboard into EL */
#ifdef ECDEBUGWIN
extern uint32_t K_ECDEBUGWIN;         /*!< open Eye Candy debug window */
#endif /* ECDEBUGWIN */
extern uint32_t K_EMOTES;             /*!< key used to toggle display of \ref emotes_window */
extern uint32_t K_RANGINGWIN;         /*!< key used to toggle display of ranging win */
extern uint32_t K_COUNTERS;           /*!< key used to toggle display of counters window  */
extern uint32_t K_HELPSKILLS;         /*!< key used to toggle display of help window skills tab  */
/*! @} */

/*! \name Quickbar access keys */
/*! @{ */
extern uint32_t K_ITEM1;
extern uint32_t K_ITEM2;
extern uint32_t K_ITEM3;
extern uint32_t K_ITEM4;
extern uint32_t K_ITEM5;
extern uint32_t K_ITEM6;
extern uint32_t K_ITEM7;
extern uint32_t K_ITEM8;
extern uint32_t K_ITEM9;
extern uint32_t K_ITEM10;
extern uint32_t K_ITEM11;
extern uint32_t K_ITEM12;
/*! @} */

/*! \name Magic quickbar access keys */
/*! @{ */
extern uint32_t K_SPELL1;
extern uint32_t K_SPELL2;
extern uint32_t K_SPELL3;
extern uint32_t K_SPELL4;
extern uint32_t K_SPELL5;
extern uint32_t K_SPELL6;
extern uint32_t K_SPELL7;
extern uint32_t K_SPELL8;
extern uint32_t K_SPELL9;
extern uint32_t K_SPELL10;
extern uint32_t K_SPELL11;
extern uint32_t K_SPELL12;
/*! @} */

/*!
 * \ingroup loadsave
 * \brief   Reads the key configuration from the default key.ini file.
 *
 *      Reads the shortcut key configuration from the default key.ini file.
 *
 * \callgraph
 */
void read_key_config();


/*!
 * \brief   Returns a string describing the specified keydef.
 *
 *      Returns (in the buffer provided) a string describing the specified keydef.
 *
 * \callgraph
 */
const char *get_key_string(uint32_t keydef, char *buf, size_t buflen);

/*!
 * \brief   Returns the value of the specified keydef.
 * 
 *      Returns the key value or 0 if not found.
 *
 * \callgraph
 */
uint32_t get_key_value(const char* name);

#ifdef __cplusplus
} // extern "C"
#endif

#endif	//__KEYS_H__
