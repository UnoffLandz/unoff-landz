cmake_minimum_required(VERSION 2.8.4)
project(unoff_landz)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH})

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D__UNIX__ -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS)
IF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D__UNIX__ -D_CRT_NONSTDC_NO_DEPRECATE)
    ADD_DEFINITIONS(/W4)
ELSE()
#-D_GLIBCXX_DEBUG
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11") #-Weverything -Wno-padded -Wno-c++98-compat-pedantic
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} "  ) #--coverage
ENDIF()


find_package(LibEV REQUIRED)
find_package(SQLite3 REQUIRED)
set(SRC broadcast_actor_functions.c
character_inventory.c
character_movement.c
character_race.c
characters.c
chat.c
client_protocol_handler.c
database_functions.c
date_time_functions.c
db_attribute_tbl.c
db_character_race_tbl.c
db_character_tbl.c
db_character_type_tbl.c
db_chat_channel_tbl.c
db_game_data_tbl.c
db_gender_tbl.c
db_map_tbl.c
db_season_tbl.c
file_functions.c
gender.c
hash_commands.c
idle_buffer.c
initialise_server.c
logging.c
log_in.c
main.c
maps.c
movement.c
numeric_functions.c
pathfinding.c
season.c
server_messaging.c
server_protocol_functions.c
server_start_stop.c
string_functions.c
)
set(INCLUDES
    attributes.h
    broadcast_actor_functions.h
    character_inventory.h
    character_movement.h
    character_race.h
    character_race_tbl.h
    characters.h
    character_type.h
    chat.h
    client_protocol.h
    client_protocol_handler.h
    clients.h
    colour.h
    database_functions.h
    date_time_functions.h
    db_attribute_tbl.h
    db_character_inventory_tbl.h
    db_character_race_tbl.h
    db_character_tbl.h
    db_character_type_tbl.h
    db_chat_channel_tbl.h
    db_game_data_tbl.h
    db_gender_tbl.h
    db_map_tbl.h
    db_season_tbl.h
    file_functions.h
    game_data.h
    gender.h
    global.h
    hash_commands.h
    idle_buffer.h
    initialise_server.h
    items.h
    logging.h
    log_in.h
    maps.h
    movement.h
    numeric_functions.h
    pathfinding.h
    season.h
    server_messaging.h
    server_parameters.h
    server_protocol_functions.h
    server_protocol.h
    server_start_stop.h
    string_functions.h
)
include_directories(${LIBEV_INCLUDE_DIR})
add_executable(unoff ${SRC} ${INCLUDES})
target_link_libraries(unoff ${LIBEV_LIBRARY} ${SQLITE3_LIBRARIES})
