#ifndef __ITEM_INFO_H
#define __ITEM_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL/SDL_types.h>
#include "bags.h"

/* if true, items descrtion tooltips are enabled in the GUI */
extern int show_item_desc_text;

/**
 * @ingroup item_info
 * @brief get the item description from the lookup table
 *
 * @param item_id the item unique id
 * @param image_id the item image id
 * @return Returns the description text
 * @callgraph
 */
const char *get_item_description(uint16_t item_id, int image_id);

/**
 * @ingroup item_info
 * @brief get the item emu from the lookup table
 *
 * @param item_id the item unique id
 * @param image_id the item image id
 * @return Returns the item enu
 * @callgraph
 */
int get_item_emu(uint16_t item_id, int image_id);

/**
 * @ingroup item_info
 * @brief get the number of items that match these ids
 *
 * @param item_id the item unique id
 * @param image_id the item image id
 * @return Returns the number of matching items
 * 
 * @callgraph
 */
int get_item_count(uint16_t item_id, int image_id);

/**
 * @ingroup item_info
 * @brief find out if we have item information
 *
 * @return Returns 1 if the information is available, otherwise 0
 * @callgraph
 */
int item_info_available(void);

/**
 * @ingroup item_info
 * @brief CONSOLE_LOG help about enabling item_info, but only once and if needed
 *
 * @callgraph
 */
void item_info_help_if_needed(void);

/**
 * @ingroup item_info
 * @brief match passed string against specified item descriptions and return details for matches
 *
 * @param storage_items_filter an array for the matching results, element to zero if matching
 * @param storage_items the items to check
 * @param filter_item_text the text to match, the needle
 * @param no_storage the number of items to check
 *
 * @callgraph
 */
void filter_items_by_description(uint8_t *storage_items_filter, const ground_item *storage_items, const char *filter_item_text, int no_storage);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
