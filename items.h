#ifndef ITEMS_H_INCLUDED
#define ITEMS_H_INCLUDED

#define MAX_ITEMS 1500

struct item_type{
    char item_name[1024];
    int harvestable; //flag that item is harvestable
    int harvest_multiplier;//amount harvested on each harvesting cycle
    int emu;
    int interval;
    int exp;
    int food_value;
    int food_cooldown;
    int organic_nexus;
    int vegetal_nexus;
};
struct item_type item[MAX_ITEMS];


#endif // ITEMS_H_INCLUDED
