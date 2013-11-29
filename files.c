#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "files.h"
#include "protocol.h" //required for read_motd
#include "string_functions.h"
#include "numeric_functions.h"

void read_motd(int sock){

    FILE *file;
    char line_in[1024]="";

    file = fopen ("motd.msg","r");

    if(file){

        while( fgets(line_in, 80, file) != NULL){
            str_trim_right(line_in);
            if(strcmp(line_in, "")==0) strcpy(line_in, " ");
            send_server_text(sock, CHAT_SERVER, line_in);
        }
    }
    else {
        perror("can't find motd");
        exit(EXIT_FAILURE);
    }

    fclose(file);

}

void save_character(char *char_name, int id){

    FILE *file;
    char file_name[1024]="";

    sprintf(file_name, "%s.ply", char_name);

    if((file=fopen(file_name, "w"))) {

        if(!fprintf(file, "%s\n %i\n %u\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n",
            characters.character[id]->password,     //1
            characters.character[id]->time_played,  //2
            characters.character[id]->char_status,  //3
            characters.character[id]->active_chan,  //4
            characters.character[id]->chan[0],      //5
            characters.character[id]->chan[1],      //6
            characters.character[id]->chan[2],      //7
            characters.character[id]->chan[3],      //8
            characters.character[id]->gm_permission, //9
            characters.character[id]->ig_permission,//10
            characters.character[id]->map_id,       //11
            characters.character[id]->map_tile,     //12
            characters.character[id]->guild_id,     //13
            characters.character[id]->char_type,    //14
            characters.character[id]->skin_type,    //15
            characters.character[id]->hair_type,    //16
            characters.character[id]->shirt_type,   //17
            characters.character[id]->pants_type,   //18
            characters.character[id]->boots_type,   //19
            characters.character[id]->head_type,    //20
            characters.character[id]->shield_type,  //21
            characters.character[id]->weapon_type,  //22
            characters.character[id]->cape_type,    //23
            characters.character[id]->helmet_type,  //24
            characters.character[id]->neck_type,    //25
            characters.character[id]->max_health,   //26
            characters.character[id]->current_health,//27
            characters.character[id]->visual_proximity,
            characters.character[id]->local_text_proximity
            )){
            printf("character file %s\n", file_name);
            perror("can't save data to file");
            exit(EXIT_FAILURE);
        }

        printf("save character [%s]\n", characters.character[id]->char_name);
        fclose(file);
    }
    else {
        printf("character file %s\n", file_name);
        perror("cannot find file");
        exit(EXIT_FAILURE);
    }

}

void save_new_character(char *char_name, int id){

    FILE *file;

    save_character(char_name, id);

    if((file=fopen("character.lst", "a"))) {

        printf("Added new character [%s]\n", char_name);

        if(!fprintf(file, "\n%s",
            char_name
            )){
            printf("error adding to character list\n");
            perror("save_new_character");
            exit(EXIT_FAILURE);
        }

    }
    else {
        printf("can't find characters list\n");
        perror("save_new_charactor");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

int load_character(char *file_name, int i){

    FILE *file;

    if((file=fopen(file_name, "r"))==NULL) return -1;

    if(!fscanf(file, "%s\n %i\n %u\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n %i\n",
        characters.character[i]->password,     //1
        &characters.character[i]->time_played, //2
        &characters.character[i]->char_status, //3
        &characters.character[i]->active_chan, //4
        &characters.character[i]->chan[0],     //5
        &characters.character[i]->chan[1],     //6
        &characters.character[i]->chan[2],     //7
        &characters.character[i]->chan[3],     //8
        &characters.character[i]->gm_permission,//9
        &characters.character[i]->ig_permission,//10
        &characters.character[i]->map_id,       //11
        &characters.character[i]->map_tile,     //12
        &characters.character[i]->guild_id,     //13
        &characters.character[i]->char_type,    //14
        &characters.character[i]->skin_type,    //15
        &characters.character[i]->hair_type,    //16
        &characters.character[i]->shirt_type,   //17
        &characters.character[i]->pants_type,   //18
        &characters.character[i]->boots_type,   //19
        &characters.character[i]->head_type,    //20
        &characters.character[i]->shield_type,  //21
        &characters.character[i]->weapon_type,  //22
        &characters.character[i]->cape_type,    //23
        &characters.character[i]->helmet_type,  //24
        &characters.character[i]->neck_type,    //25
        &characters.character[i]->max_health,   //26
        &characters.character[i]->current_health,//27
        &characters.character[i]->visual_proximity, //28
        &characters.character[i]->local_text_proximity //29
        )){
        printf("character file %s\n", file_name);
        perror("data missing from file");
        exit(EXIT_FAILURE);
    }

    fclose(file);

    return 0;

}

int load_channel(char *file_name, int i){

    FILE *file;

    if((file=fopen(file_name, "r"))==NULL) return -1;

    if(!fscanf(file, "%u %s %i %i %[^\n]", /* the %u is needed because chan_type is an enum */
        &channels.channel[i]->chan_type,
        channels.channel[i]->password,
        &channels.channel[i]->owner_id,
        &channels.channel[i]->channel_id,
        channels.channel[i]->description
        )){
        printf("channel file %s\n", file_name);
        perror("data missing from file");
        exit(EXIT_FAILURE);
    }

    fclose(file);

    return 0;
}

int load_guild(char *file_name, int i){

    FILE *file;

    if((file=fopen(file_name, "r"))==NULL) return -1;

    if(!fscanf(file, "%s %i %i %i %i %i",
        guilds.guild[i]->guild_tag,
        &guilds.guild[i]->tag_colour,
        &guilds.guild[i]->log_on_notification_colour,
        &guilds.guild[i]->log_off_notification_colour,
        &guilds.guild[i]->guild_chan_text_colour,
        &guilds.guild[i]->guild_chan_number
        )){
        printf("guild file %s\n", file_name);
        perror("data missing from file");
        exit(EXIT_FAILURE);
    }

    fclose(file);

    return 0;
}

int load_map(char *file_name, int id){

    FILE *file;

    unsigned char bytes[4];

    char elm_filename[1024]="";

    int tile_map_offset=0;
    int height_map_offset=0;
    int threed_object_offset=0;

    int tile_map_size=0;
    int height_map_size=0;

    int j=0;

    //open the map overview file
    if((file=fopen(file_name, "r"))==NULL) return -1;

    // extract the data
    if(!fscanf(file, "%s %i",
        maps.map[id]->elm_filename,
        &maps.map[id]->map_axis
        )){
        perror("problem loading data from file");
        exit(EXIT_FAILURE);
    }

    //printf("data acquired from file\n");

    //close the file
    fclose(file);

    //open the elm file
    extract_file_name(maps.map[id]->elm_filename, elm_filename);

    //printf("Attempting to load .elm file for %s [%s]\n", maps.map[id]->map_name, elm_filename);

    if((file=fopen(elm_filename, "r"))==NULL) {
        perror("file missing");
        exit(EXIT_FAILURE);
    }

    //read file identification bytes (should = elmf)
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem loading file identification bytes 1");
        exit (EXIT_FAILURE);
    }

    if(bytes[0]=='e' && bytes[1]=='l' && bytes[2]=='m' && bytes[3]=='f'){
        //printf("file identification bytes are %c %c %c %c\n", bytes[0], bytes[1], bytes[2], bytes[3]);
    }
    else {
        perror("incorrect file identification bytes");
        exit (EXIT_FAILURE);
    }

    //read horizontal tile count
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading horizontal tile count");
        exit (EXIT_FAILURE);
    }

    j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("horizontal tiles %i\n", j);

    /* because a char can move 6 steps per tile, we calculate the map axis as 6x the number
    of horizontal tiles. In the case of Isla Prima, the number of horizontal tiles is 32, so
    our map axis is calculated as 6x32=192 */
    maps.map[id]->map_axis=j*6;

    //read vertical tile count
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading vertical tile count");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("vertical tiles %i\n", j);

    /* Because we assume that maps will always be a perfect square, if vertical tiles do not equal
    the number of horizontal tiles, we need to flag it as a serious problem */
    if(maps.map[id]->map_axis/6!=j){
        perror ("map is not a perfect square");
        exit (EXIT_FAILURE);
    }

    //read tile map offset
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading tile map offset");
        exit (EXIT_FAILURE);
    }

    tile_map_offset=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("tile map offset %i\n", tile_map_offset);

    // We assume that tile map offset will always be 124 bytes otherwise our map data will be loaded incorrectly
    if(tile_map_offset!=124){
        perror ("abnormal map header size");
        exit (EXIT_FAILURE);
    }

    //read height map offset
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading height map offset");
        exit (EXIT_FAILURE);
    }

    height_map_offset=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("height map offset %i\n", height_map_offset);

    //read 3d object structure length
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading 3d object structure length");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("3d object structure length %i\n", j);

    //read 3d object count
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading 3d object count");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("3d object count %i\n", j);

    //read 3d object offset
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading 3d object offset");
        exit (EXIT_FAILURE);
    }

    threed_object_offset=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("3d object count %i\n", threed_object_offset);


    //read 2d object structure length
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading 2d object structure len");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("2d object structure len %i\n", j);

    //read 2d object count
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading 2d object count");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("2d object count %i\n", j);

    //read 2d object offset
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading 2d object offset");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("2d object offset %i\n", j);

    //read lights structure length
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading lights structure length");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("lights structure length %i\n", j);

    //read lights count
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading lights count");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("lights count %i\n", j);

    //read lights offset
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading lights offset");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("lights offset %i\n", j);

    //read dungeon flag
    if(fread(bytes, 1, 1, file)!=1){
        perror ("problem reading dungeon flag");
        exit (EXIT_FAILURE);
    }

    //printf("dungeon flag %i\n", bytes[0]);

    //read version flag
    if(fread(bytes, 1, 1, file)!=1){
        perror ("problem reading version flag");
        exit (EXIT_FAILURE);
    }

    //printf("version flag %i\n", bytes[0]);

    //read reserved byte 3
    if(fread(bytes, 1, 1, file)!=1){
        perror ("problem reading reserved byte 3");
        exit (EXIT_FAILURE);
    }

    //printf("reserved byte 3 %i\n", bytes[0]);

    //read reserved byte 4
    if(fread(bytes, 1, 1, file)!=1){
        perror ("problem reading reserved byte 4");
        exit (EXIT_FAILURE);
    }

    //printf("reserved byte 4 %i\n", bytes[0]);

    //read ambient red
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading ambient red");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("ambient red %f\n", (float)j);

    //read ambient green
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading ambient green");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("ambient green %f\n", (float)j);

    //read ambient blue
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading ambient blue");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("ambient blue %f\n", (float)j);

    //read particles structure len
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading particules structure length");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("particles structure len %i\n", j);

    //read particles count
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading particules count");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("particles count %i\n", j);

    //read particles offset
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading particules offset");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("particles offset %i\n", j);

    //read clusters offset
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading clusters offset");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("clusters offset %i\n", j);

    //read reserved 9
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 9");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 9 %i\n", j);

    //read reserved 10
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 10");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 10 %i\n", j);

    //read reserved 11
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 11");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 11 %i\n", j);

    //read reserved 12
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 12");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 12 %i\n", j);

    //read reserved 13
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 13");
        exit (EXIT_FAILURE);
    }

    j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 13 %i\n", j);

    //read reserved 14
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 14");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 14 %i\n", j);

    //read reserved 15
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 15");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 15 %i\n", j);

    //read reserved 16
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 16");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 16 %i\n", j);

    //read reserved 17
    if(fread(bytes, 4, 1, file)!=1){
        perror ("problem reading res 17");
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 17 %i\n", j);

    //read tile map

    /* the height map offset indicates the end of the tile map, hence we use it to calculate
    the size of the tile map by deducting the tile map offset (header size). We can then read
    that number of bytes to extract our tile map data. In the case of Isla Prima, the tile map
    consists of 32 x 32 =1024 bytes */

    tile_map_size=height_map_offset-tile_map_offset;
    maps.map[id]->tile_map_size=tile_map_size;

     if(fread(maps.map[id]->tile_map, tile_map_size, 1, file)!=1){
        perror ("problem reading tile map");
        exit (EXIT_FAILURE);
    }

    //read height map

    /* the 3d object offset indicates the end of the height map, hence we use it to calculate
    the size of height map by deducting the height map offset. We can then read that number of
    bytes to extract our height map data. In the case of Isla Prima, the height map consists
    of 192 x 192 = 36864 bytes */

    height_map_size=threed_object_offset-height_map_offset;
    maps.map[id]->height_map_size=height_map_size;

    if(fread(maps.map[id]->height_map, height_map_size, 1, file)!=1){
        perror ("problem reading height map");
        exit (EXIT_FAILURE);
    }

    return 0;

}

void load_all_channels(const char *file_name){

    FILE *file;
    char channel_file_name[1024];
    int i=0;

    //check we have an existing list file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) {

        printf("Can't find channel list file [%s]. Creating new one\n", file_name);

        if((file=fopen(file_name, "w"))==NULL) {
            perror("unable to create list file in function load_all_channels");
            exit(EXIT_FAILURE);
        }
    }

    printf("\nLoading channel list file [%s]...\n", file_name);

    if((file=fopen(file_name, "r"))) {

        while ((fscanf(file, "%s", channels.channel[i]->channel_name))!=-1){

            sprintf(channel_file_name, "%s.chn", channels.channel[i]->channel_name);

            if(load_channel(channel_file_name, i)==FILE_FOUND){
               printf("loaded [%i] %s\n", i, channels.channel[i]->channel_name);
            }
            else{
                printf("missing %s\n", channel_file_name);
                perror("missing file in function load_all_channels");
                exit(EXIT_FAILURE);
            }

            i++;

            if(i==channels.max){
                perror("maximum game channels exceeded in function load_all_channels");
                exit(EXIT_FAILURE);
            }

        }
    }

    printf("[%i] channels were loaded\n", i);

    channels.count=i;

    fclose(file);
}

void load_all_characters(const char *file_name) {

    FILE *file;
    char character_file_name[1024];
    int i=0;

    //check we have an existing list file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) {

        printf("Can't find character list file [%s]. Creating new one\n", file_name);

        if((file=fopen(file_name, "w"))==NULL) {
            perror("unable to create list file in function load_all_characters");
            exit(EXIT_FAILURE);
        }
    }

    printf("\nLoading character list file [%s]...\n", file_name);

    while ((fscanf(file, "%s", characters.character[i]->char_name))!=-1){

        sprintf(character_file_name, "%s.ply", characters.character[i]->char_name);

        if(load_character(character_file_name, i)==FILE_FOUND) {
            printf("loaded [%i] %s\n", i, characters.character[i]->char_name);
        }
        else{
            printf("missing %s\n", character_file_name);
            perror("missing file in function load_all_characters");
            exit(EXIT_FAILURE);
        }

        i++;

        if(i==characters.max){
            perror("maximum game characters exceeded in function load_all_characters");
            exit(EXIT_FAILURE);
        }

    }

    printf("[%i] characters were loaded\n", i);

    characters.count=i;

    fclose(file);
}

void load_all_guilds(const char *file_name){

    FILE *file;
    char guild_file_name[1024];
    int i=0;

    //check we have an existing list file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) {

        printf("Can't find guild list file [%s]. Creating new one\n", file_name);

        if((file=fopen(file_name, "w"))==NULL) {
            perror("unable to create list file in function load_all_guilds");
            exit(EXIT_FAILURE);
        }
    }

    printf("\nLoading guild list file [%s]...\n", file_name);


    if((file=fopen(file_name, "r"))) {

        while ((fscanf(file, "%s", guilds.guild[i]->guild_name))!=-1){

            sprintf(guild_file_name, "%s.gld", guilds.guild[i]->guild_name);

            if(load_guild(guild_file_name, i)==FILE_FOUND) {
                printf("loaded [%i] %s\n", i, guilds.guild[i]->guild_name);
            }
            else{
                printf("missing %s\n", guild_file_name);
                perror("missing file in function load_all_guilds");
                exit(EXIT_FAILURE);
            }

            i++;

            if(i==guilds.max){
                perror("maximum game guilds exceeded in function load_all_guilds");
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("[%i] guilds were loaded\n", i);

    guilds.count=i;

    fclose(file);

}

void load_all_maps(const char *file_name){

    FILE *file;
    char map_file_name[1024];
    int i=0;

    //check we have an existing list file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) {

        printf("Can't find guild list file [%s]. Creating new one\n", file_name);

        if((file=fopen(file_name, "w"))==NULL) {
            perror("unable to create list file in function load_all_guilds");
            exit(EXIT_FAILURE);
        }
    }

    printf("\nLoading map list file [%s]...\n", file_name);

    if((file=fopen(file_name, "r"))) {

        while ((fscanf(file, "%s", maps.map[i]->map_name))!=-1){

            sprintf(map_file_name, "%s.map", maps.map[i]->map_name);

            if(load_map(map_file_name, i)==FILE_FOUND) {
                printf("loaded [%i] %s\n", i, maps.map[i]->map_name);
            }
            else{
                printf("missing %s\n", map_file_name);
                perror("missing file in function load_all_characters");
                exit(EXIT_FAILURE);
            }

            i++;

            if(i==maps.max){
                perror("maximum game maps exceeded in function load_all_maps");
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("[%i] maps were loaded\n", i);

    maps.count=i;

    fclose(file);
}

int get_file_size(char *file_name){

    FILE *file;
    int count=0;

    if((file=fopen(file_name, "r"))) {

        while(fgetc(file) != EOF) {
            count++;
        }
    }

    fclose(file);

    return count;
}


void log_to_file(char *file_name, char *text) {

    FILE *file;

    //check we have an existing list file and, if not, then create one
    if((file=fopen(file_name, "a"))==NULL) {

        printf("Can't find file [%s]. Creating new one\n", file_name);

        if((file=fopen(file_name, "a"))==NULL) {
            perror("unable to create file in function log_to_file");
            exit(EXIT_FAILURE);
        }
    }

    if(!fprintf(file, "%s", text)){
        printf("log file %s\n", file_name);
        perror("can't save data to file");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void log_event(int event_type, char *text_in){

    char file_name[1024]="";
    char text_out[1024]="";

    time_t rawtime;
    struct tm * timeinfo;
    char time_str[80]="";

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    sprintf(time_str, "[%i:%i:%i - %i:%i:%i]", timeinfo->tm_sec, timeinfo->tm_min, timeinfo->tm_hour, timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year);

    switch(event_type){

        case EVENT_NEW_CHAR:
            strcpy(file_name, "character.log");
            sprintf(text_out, "%s New Character -%s", time_str, text_in);
        break;

        case EVENT_ERROR:
            strcpy(file_name, "error.log");
            sprintf(text_out, "%s New Character -%s", time_str, text_in);
        break;

        case EVENT_SESSION:
            strcpy(file_name, "session.log");
            sprintf(text_out, "%s Character -%s", time_str, text_in);
        break;

        default:
            strcpy(file_name, "error.log");
            sprintf(text_out, "%s Unknown Event %s", time_str, text_in);
        break;

    }

    log_to_file(file_name, text_out);
}
