include(ExternalProject)

INCLUDE(FindPkgConfig)

find_package(SDL REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Cal3D REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set this to "no" if you don't want this file to be overwritten
set(UPDATE_CONF yes)

# tune these for your setup & compile options
# IMPORTANT: if you are using a linker that needs libraries in correct order, make sure you list EYE_CANDY before PNG_SCREENSHOT if you use both

### Current release default options ###
set(FEATURES ${FEATURES} CLUSTER_INSIDES)	        # Group objects into separate clusters for clipping on inside maps
set(FEATURES ${FEATURES} CUSTOM_LOOK)		        # allows you to customize your look.
set(FEATURES ${FEATURES} CUSTOM_UPDATE)		        # allows autoupdating of custom look information
set(FEATURES ${FEATURES} FUZZY_PATHS)		        # Makes Tab Map walking not always follow exaclty the same path
set(FEATURES ${FEATURES} NEW_SOUND)		        # Enables extended sound effects system
set(FEATURES ${FEATURES} PNG_SCREENSHOT)	        # make screenshots in W3C's PNG format in game, requires libpng
set(FEATURES ${FEATURES} TEXT_ALIASES)		        # Text aliases
set(FEATURES ${FEATURES} USE_INLINE)			# enable some optimizations to use inline functions instead of pure function calls
# new for 1.9.0 release
set(FEATURES ${FEATURES} BANDWIDTH_SAVINGS)		# enable bandwidth saving changes for message protocol
# new for 1.9.2 release
set(FEATURES ${FEATURES} ANIMATION_SCALING)		# Adds support for animation scaling, depending on the number of animation commands in the actor queue.
set(FEATURES ${FEATURES} FSAA)			# Adds support for full screen anti-aliasing using OpenGL multisampling.
set(FEATURES ${FEATURES} NEW_NEW_CHAR_WINDOW)		# Use the new character creation window; controls always on-screen
set(FEATURES ${FEATURES} NEW_TEXTURES)		# Enables the new texture loading and caching system (http://www.eternal-lands.com/forum/index.php?showtopic=54756)
# new for 1.9.3 release
set(FEATURES ${FEATURES} FASTER_MAP_LOAD)		# Enables optimizations to speed up the process of switching between maps
set(FEATURES ${FEATURES} FASTER_STARTUP	)	# An attempt to reduce the startup time of EL somewhat
set(FEATURES ${FEATURES} MIDDLE_MOUSE_PASTE)	# Paste using the middle mouse button from the primary buffer on X systems
# new for 1.9.4 release
set(FEATURES ${FEATURES} NEW_EYES)			# Support for different eye colors, needs new data files

### Testing options (these options are possible for the next release and should be tested) ###
#set(FEATURES ${FEATURES} ANTI_ALIAS)			# allows to enable/disable anti-aliasing in el.ini
#set(FEATURES ${FEATURES} DYNAMIC_ANIMATIONS)		# (appears broken) Synchronizes animation to FPS instead of a fixed timer
#set(FEATURES ${FEATURES} EXT_ACTOR_DICT)		# Removes remaining hard-coded actor def dictionaries - requires updated actor defs files (http://el.grug.redirectme.net/actor_defs.zip)
#set(FEATURES ${FEATURES} NEW_ALPHA	)		# (undocumented)
#set(FEATURES ${FEATURES} USE_SIMD	)		# Enables usage of simd instructions

### Machine specific options (fixes or performance enhancements) ###
#set(FEATURES ${FEATURES} EL_BIG_ENDIAN)		# Enable big-endian code (automatically detected for OSX)
#set(FEATURES ${FEATURES} NO_PF_MACRO)		# Use a function instead of a macro for pf_get_tile()
#set(FEATURES ${FEATURES} SIMPLE_LOD)			# enable a simplistic distance culling to improve performance

### Debug options ###
#set(FEATURES ${FEATURES} CONTEXT_MENUS_TEST)		# Enable "#cmtest" command to help test/demo the context menu code
#set(FEATURES ${FEATURES} DEBUG	)		# (undocumented)
#set(FEATURES ${FEATURES} DEBUG_XML	)		# Enables missing (optional) XML string property messages
#set(FEATURES ${FEATURES} DEBUG_POINT_PARTICLES)	# (undocumented)
#set(FEATURES ${FEATURES} DEBUG_TIME)			# Fakes the acceleration of time for use in debugging shadows and the like.
#set(FEATURES ${FEATURES} DEBUG_MAP_SOUND)		# Adds (some??) map boundary areas to tab maps and additional logging to stdout
#set(FEATURES ${FEATURES} ECDEBUGWIN)			# press ctrl-alt-c to open the Eye Candy debug window: trigger most effects without casting spells or server side events
#set(FEATURES ${FEATURES} EXTRA_DEBUG)		# (undocumented)
#set(FEATURES ${FEATURES} MEMORY_DEBUG)		# gather information about memory allocation and freeing
#set(FEATURES ${FEATURES} MISSILES_DEBUG)		# Enables debug for missiles feature. It will create a file missiles_log.txt file in your settings directory.
#set(FEATURES ${FEATURES} MUTEX_DEBUG)		# (undocumented)
#set(FEATURES ${FEATURES} OPENGL_TRACE)		# make far more frequent checks for OpenGL errors (requires -DDEBUG to be of any use). Will make error_log.txt a lot larger.
#set(FEATURES ${FEATURES} TIMER_CHECK)	# (undocumented)
#set(FEATURES ${FEATURES} _EXTRA_SOUND_DEBUG)		# Enable debug for sound effects


### Other options (Experimental, unfinished, defunct or otherwise unknown) ###
#set(FEATURES ${FEATURES} NEW_CURSOR)			# New coloured cursors made by Emajekral (Experimental) Extract http://users.on.net/~gingerman/sky_cursor-textures.zip into datadir/textures/
#set(FEATURES ${FEATURES} PAWN	)		# Experimental, not for release, will need server support to function properly. This *will* eat your cat. You've been warned. Enables the Pawn abstract machine.
#set(FEATURES ${FEATURES} UID)			# use unique ID sent from server for custom looks. (INCOMPLETE)
#set(FEATURES ${FEATURES} USE_ACTORS_OPTIMIZER)	# Enables actor optimizations
#set(FEATURES ${FEATURES} USE_BOOST)
#set(FEATURES ${FEATURES} USE_TR1	)		# Needs tr1 version of c++ stl, used for NEW_FILE_IO
#set(FEATURES ${FEATURES} WRITE_XML	)		# dangerous to use, will wipe out some XML files that are part of the main game download
#set(FEATURES ${FEATURES} ZLIBW	)		# Enables being able to write gzip compressed files, requires -lzlib/-lz needed for map_editor
#set(FEATURES ${FEATURES} OLD_MISC_OBJ_DIR	)	# if not defined, ./3dobjects/misc_objects/ paths becomes ./3dobjects/



add_definitions(-DLINUX -DELC -D_7ZIP_ST)
foreach(feature ${FEATURES})
    add_definitions(-D${feature})
endforeach()
#OPTIONS += $(GIT_VERSION)

# the objects we need
SET(MEMORY_DEBUG_COBJ elmemory.cpp)
SET(TEXT_ALIASES_COBJ text_aliases.cpp)
SET(PAWN_COBJ pawn/amx.c pawn/amxaux.c pawn/amxcons.c pawn/amxel.c
    pawn/amxfloat.c pawn/amxstring.c pawn/elpawn.c
    )

SET(NEW_TEXTURES_COBJ image_loading.cpp)
SET(NEW_TEXTURES_CXXOBJ engine/hardwarebuffer.cpp)
SET(CUSTOM_UPDATE_COBJ custom_update.cpp new_update.cpp)
#disabled it for now, made too much trouble
#FSAA_COBJ = fsaa/fsaa_glx.c fsaa/fsaa.c
SET(FSAA_COBJ fsaa/fsaa_dummy.cpp fsaa/fsaa.cpp)
set(COBJS

    xz/7zCrc.c xz/7zCrcOpt.c xz/Alloc.c xz/Bra86.c xz/Bra.c xz/BraIA64.c
    xz/CpuArch.c xz/Delta.c xz/LzFind.c xz/Lzma2Dec.c xz/Lzma2Enc.c
    xz/LzmaDec.c xz/LzmaEnc.c xz/Sha256.c xz/Xz.c xz/XzCrc64.c xz/XzDec.c
    xz/XzEnc.c
)
foreach(feature ${FEATURES})
  if(${feature}_COBJ)
    set(COBJS ${COBJS} ${${feature}_COBJ})
  endif()
endforeach()

set(CXXOBJS

    io/fileutil.cpp
    io/e3d_io.cpp io/elc_io.cpp	io/map_io.cpp io/elpathwrapper.cpp io/xmlcallbacks.cpp
    io/half.cpp io/normal.cpp io/elfilewrapper.cpp io/unzip.cpp io/ioapi.cpp io/zip.cpp io/ziputil.cpp

    actor_scripts.cpp
items.cpp
    cache.cpp cal.cpp calc.cpp chat.cpp cluster.cpp colors.cpp console.cpp consolewin.cpp
    counters.cpp cursors.cpp dds.cpp ddsimage.cpp dialogues.cpp draw_scene.cpp eye_candy_debugwin.cpp
    elconfig.cpp encyclopedia.cpp errors.cpp events.cpp
    filter.cpp font.cpp framebuffer.cpp frustum.cpp
    gamewin.cpp gl_init.cpp hud.cpp help.cpp highlight.cpp
    ignore.cpp init.cpp interface.cpp

    keys.cpp knowledge.cpp langselwin.cpp lights.cpp list.cpp load_gl_extensions.cpp loginwin.cpp loading_win.cpp
    main.cpp manufacture.cpp map.cpp mapwin.cpp memory.cpp
    md5.cpp mines.cpp minimap.cpp misc.cpp missiles.cpp multiplayer.cpp
    new_actors.cpp new_character.cpp notepad.cpp
    openingwin.cpp image.cpp

    particles.cpp paste.cpp pathfinder.cpp pm_log.cpp
    queue.cpp reflection.cpp rules.cpp	sky.cpp
    skeletons.cpp skills.cpp serverpopup.cpp servers.cpp session.cpp shadows.cpp sound.cpp
    spells.cpp stats.cpp storage.cpp special_effects.cpp
    tabs.cpp text.cpp textures.cpp tile_map.cpp timers.cpp translate.cpp
    update.cpp url.cpp weather.cpp widgets.cpp makeargv.cpp popup.cpp hash.cpp emotes.cpp

    2d_objects.cpp 3d_objects.cpp achievements.cpp actor_init.cpp actors.cpp
    alphamap.cpp asc.cpp
    astrology.cpp
    bbox_tree.cpp books.cpp buddy.cpp buffs.cpp bags.cpp
    cal3d_wrapper.cpp command_queue.cpp
    context_menu.cpp
    elwindows.cpp
    elloggingwrapper.cpp engine/logging.cpp exceptions/extendedexception.cpp
    hud_indicators.cpp hud_timer.cpp icon_window.cpp io/cal3d_io_wrapper.cpp item_info.cpp item_lists.cpp named_colours.cpp
    optimizer.cpp quest_log.cpp select.cpp sendvideoinfo.cpp trade_log.cpp user_menus.cpp
    trade.cpp
    xml/xmlhelper.cpp eye_candy_wrapper.cpp
    eye_candy/eye_candy.cpp eye_candy/math_cache.cpp eye_candy/effect_lamp.cpp
    eye_candy/effect_candle.cpp
    eye_candy/effect_campfire.cpp eye_candy/effect_fountain.cpp
    eye_candy/effect_teleporter.cpp eye_candy/effect_firefly.cpp
    eye_candy/effect_sword.cpp eye_candy/effect_summon.cpp
    eye_candy/effect_selfmagic.cpp eye_candy/effect_targetmagic.cpp
    eye_candy/effect_ongoing.cpp eye_candy/effect_impact.cpp
    eye_candy/effect_smoke.cpp eye_candy/effect_bag.cpp eye_candy/effect_cloud.cpp
    eye_candy/effect_harvesting.cpp eye_candy/effect_wind.cpp
    eye_candy/effect_breath.cpp eye_candy/effect_glow.cpp
    eye_candy/effect_mines.cpp eye_candy/effect_missile.cpp
    eye_candy/orbital_mover.cpp eye_candy/kepler_orbit.cpp
    eye_candy/effect_staff.cpp
    shader/noise.cpp shader/shader.cpp

)

add_definitions(${LIBXML2_DEFINITIONS} ${SDL_DEFINITIONS})
include_directories(${LIBXML2_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${CAL3D_INCLUDE_DIRS})

foreach(feature ${FEATURES})
  if(${feature}_CXXOBJ)
    set(CXXOBJS ${CXXOBJS} ${${feature}_CXXOBJ})
  endif()
endforeach()

SET( OBJS ${COBJS} ${CXXOBJS} )
#HEADER_DIRS = . books eye_candy io pawn fsaa engine xz

#EXE=eternal_lands
add_executable(eternal_lands ${OBJS})
#PNG_SCREENSHOT_LIBS = -lpng
target_link_libraries(eternal_lands
${SDL_LIBRARY} ${LIBXML2_LIBRARIES} ${CAL3D_LIBRARIES}
 -lSDL_net
    -lSDL_image -lopenal -lGL -lGLU -lm ${EXTRA_LIBS}
    -lz -lpng -lvorbis -lvorbisfile -lstdc++ -lX11
  -lGL -lpthread -lstdc++
)
foreach(feature ${FEATURES})
  if(${${feature}_LIBS})
    target_link_libraries(eternal_lands ${${feature}_LIBS})
  endif()
endforeach()


#docs:
#    cd docs && doxygen Doxyfile

# define GIT_VERSION if we have a version file
#VTMP := $(shell [ -r git_version ] && cat git_version)
#ifdef VTMP
#GIT_VERSION = -DGIT_VERSION=\"$(VTMP)\"
#endif

# generate the git_version file
#VERSION = $(shell git describe --abbrev=4 --dirty --always)
#gen_git_version:
#    @echo -n "$(VERSION)" > git_version
#    @echo "New version file: $(VERSION)"
#    @rm -f console.o $(EXE)

#pawn_scripts/features.inc: make.conf
#    echo "#if defined _features_included" > $@; \
#    echo "\t#endinput" >> $@; \
#    echo "#endif" >> $@; \
#    echo "#define _features_included" >> $@; \
#    for feature in $(FEATURES); do \
#        echo "#define $$feature" >> $@; \
#    done
