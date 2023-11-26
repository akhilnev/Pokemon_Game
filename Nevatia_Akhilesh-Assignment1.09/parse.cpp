#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "parse.h"

//
void pokemon_parser(std::vector<pokemon_t*> *pokemon) {

    

    char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/pokemon.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/pokemon.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("pokemon.csv");
        }
    }

    std::string line = "";

    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
    std::string temp;
    pokemon_t* p; 
   
    int count = 0;

    if(!scanner.is_open()) {
        exit(EXIT_FAILURE);
    }

    getline(scanner, line);
    while(scanner.good()) {
     p = (pokemon_t*) malloc(sizeof(*p));
    
    getline(scanner, temp, ',');
    id = atoi(temp.c_str());
    p->id = id;

    getline(scanner, identifier, ',');
    p->identifier = (char*) strdup(identifier.c_str());
    
    
    getline(scanner, temp, ',');
    species_id = atoi(temp.c_str());
    p->species_id = species_id;

    getline(scanner, temp, ',');
    height = atoi(temp.c_str());
    p->height = height;

     getline(scanner, temp, ',');
    weight = atoi(temp.c_str());
    p->weight = weight;

    getline(scanner, temp, ',');
    base_experience = atoi(temp.c_str());
    p->base_experience = base_experience;

    getline(scanner, temp, ',');
    order = atoi(temp.c_str());
    p->order = order;

    getline(scanner, temp);
    is_default = atoi(temp.c_str());
    p->is_default = is_default;

   // getline(scanner, temp);

    pokemon->push_back(p);
    count++;
    line = "";
    }
}



void moves_parser(std::vector<moves_t*> *moves) {
    char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/moves.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/moves.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("moves.csv");
        }
    }
//    scanner.open("moves.csv");
    std::string line = "";
    moves_t* m;
    std::string temp;

    int id; 
    std::string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_c;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;

    if(!scanner.is_open()) {
        exit(EXIT_FAILURE);
    }

    getline(scanner, line);

     while(scanner.good()) {
        m = (moves_t*) malloc(sizeof(*m));
     

    getline(scanner, temp, ',');
    if(temp != "") {
    id = atoi(temp.c_str());
    m->id = id;
    }
    else 
    {
        m->id = INT_MAX;
    }

    getline(scanner, identifier, ',');
    m->identifier = (char*) strdup(identifier.c_str());

    getline(scanner, temp, ',');
    if(temp != "") {
    generation_id = atoi(temp.c_str());
    m->generation_id = generation_id;
    }
    else {
        m->generation_id = INT_MAX;
    }
    
    getline(scanner, temp, ',');
    if(temp != "") {
    type_id = atoi(temp.c_str());
    m->type_id = type_id;
    }
    else {
    m->type_id = INT_MAX;
    }


    getline(scanner, temp, ',');
    if(temp != "") {
    power = atoi(temp.c_str());
    m->power = power;
    }
    else {
        m->power = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    pp = atoi(temp.c_str());
    m->pp = pp;
    }
    else {
    m->pp = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    accuracy = atoi(temp.c_str());
    m->accuracy = accuracy;
    }
    else {
    m->accuracy = INT_MAX;
    }

     getline(scanner, temp, ',');
    if(temp != "") {
    priority = atoi(temp.c_str());
    m->priority = priority;
    }
    else {
    m->priority = INT_MAX;
    }

     getline(scanner, temp, ',');
    if(temp != "") {
    target_id = atoi(temp.c_str());
    m->target_id = target_id;
    }
    else {
        m->target_id = INT_MAX;
    }

     getline(scanner, temp, ',');
     if(temp != "") {
    damage_c = atoi(temp.c_str());
    m->damage_c = damage_c;
     }
     else
     {
        m->damage_c = INT_MAX;
     }

     getline(scanner, temp, ',');
     if(temp != "") {
    effect_id = atoi(temp.c_str());
    m->effect_id = effect_id;
     }
     else {
        m->effect_id = INT_MAX;
     }

     getline(scanner, temp, ',');
     if(temp != "") {
    effect_chance = atoi(temp.c_str());
    m->effect_chance = effect_chance;
     }
     else {
        m->effect_chance = INT_MAX;
     }

     getline(scanner, temp, ',');
     if(temp != "") {
    contest_type_id = atoi(temp.c_str());
    m->contest_type_id = contest_type_id;
     }
     else {
        m->contest_type_id = INT_MAX;
     }

    getline(scanner, temp, ',');
     if(temp != "") {
    contest_effect_id = atoi(temp.c_str());
    m->contest_effect_id = contest_effect_id;
     }
     else {
        m->contest_effect_id = INT_MAX;
     }

     getline(scanner, temp);
     if(temp != "") {
    super_contest_effect_id = atoi(temp.c_str());
    m->super_contest_effect_id = super_contest_effect_id;
     }
     else {
        m->super_contest_effect_id = INT_MAX;
     }

    moves->push_back(m);

     }
}

void pokemon_moves_parser(std::vector<pokemon_moves_t*> *pokemon_moves) {
        char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/pokemon_moves.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_moves.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("pokemon_moves.csv");
        }
    }
    //scanner.open("pokemon_moves.csv");
    std::string line = "";
    pokemon_moves_t* m;
    std::string temp;

    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

    if(!scanner.is_open()) {
        exit(EXIT_FAILURE);
    }

    getline(scanner, line);


    while(scanner.good()) {
         m = (pokemon_moves_t*) malloc(sizeof(*m));

    getline(scanner, temp, ',');
    if(temp != "") {
    pokemon_id = atoi(temp.c_str());
    m->pokemon_id = pokemon_id;
    }
    else 
    {
        m->pokemon_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    version_group_id = atoi(temp.c_str());
    m->version_group_id = version_group_id;
    }
    else 
    {
        m->version_group_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    move_id = atoi(temp.c_str());
    m->move_id = move_id;
    }
    else 
    {
        m->move_id = INT_MAX;
    }

 getline(scanner, temp, ',');
    if(temp != "") {
    pokemon_move_method_id = atoi(temp.c_str());
    m->pokemon_move_method_id = pokemon_move_method_id;
    }
    else 
    {
        m->pokemon_move_method_id = INT_MAX;
    }

     getline(scanner, temp, ',');
    if(temp != "") {
    level = atoi(temp.c_str());
    m->level = level;
    }
    else 
    {
        m->level = INT_MAX;
    }

     getline(scanner, temp);
    if(temp != "") {
    order = atoi(temp.c_str());
    m->order = order;
    }
    else 
    {
        m->order = INT_MAX;
    }

    pokemon_moves->push_back(m);
 line = "";
    }


    
}


void pokemon_species_parser(std::vector<pokemon_species_t*> *pokemon_species) {
    char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/pokemon_species.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_species.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("pokemon_species.csv");
        }
    }
    
   // std::ifstream scanner;
   // scanner.open("pokemon_species.csv");
    std::string line = "";
    pokemon_species_t* m;
    std::string temp;
    std::string identifier;

    int id;
 //   char* identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;

    if(!scanner.is_open()) {
        exit(EXIT_FAILURE);
    }

    getline(scanner, line);

    while(scanner.good()) { 
        m = (pokemon_species_t*) malloc(sizeof(*m));

    getline(scanner, temp, ',');
    if(temp != "") {
    id = atoi(temp.c_str());
    m->id = id;
    }
    else 
    {
        m->id = INT_MAX;
    }

    getline(scanner, identifier, ',');
    if(temp != "") {
    m->identifier = (char*) strdup(identifier.c_str());
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    generation_id = atoi(temp.c_str());
    m->generation_id = generation_id;
    }
    else {
        m->generation_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    evolves_from_species_id = atoi(temp.c_str());
    m->evolves_from_species_id = evolves_from_species_id;
    }
    else {
        m->evolves_from_species_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    evolution_chain_id = atoi(temp.c_str());
    m->evolution_chain_id = evolution_chain_id;
    }
    else {
        m->evolution_chain_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    color_id = atoi(temp.c_str());
    m->color_id = color_id;
    }
    else {
        m->color_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    shape_id = atoi(temp.c_str());
    m->shape_id = shape_id;
    }
    else {
        m->shape_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    habitat_id = atoi(temp.c_str());
    m->habitat_id = habitat_id;
    }
    else {
        m->habitat_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    gender_rate = atoi(temp.c_str());
    m->gender_rate = gender_rate;
    }
    else {
        m->gender_rate = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    capture_rate = atoi(temp.c_str());
    m->capture_rate = capture_rate;
    }
    else {
        m->capture_rate = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    base_happiness = atoi(temp.c_str());
    m->base_happiness = base_happiness;
    }
    else {
        m->base_happiness = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    is_baby = atoi(temp.c_str());
    m->is_baby = is_baby;
    }
    else {
        m->is_baby = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    hatch_counter = atoi(temp.c_str());
    m->hatch_counter = hatch_counter;
    }
    else {
        m->hatch_counter = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    has_gender_differences = atoi(temp.c_str());
    m->has_gender_differences = has_gender_differences;
    }
    else {
        m->has_gender_differences = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    growth_rate_id = atoi(temp.c_str());
    m->growth_rate_id = growth_rate_id;
    }
    else {
        m->growth_rate_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    forms_switchable = atoi(temp.c_str());
    m->forms_switchable = forms_switchable;
    }
    else {
        m->forms_switchable = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    is_legendary = atoi(temp.c_str());
    m->is_legendary = is_legendary;
    }
    else {
        m->is_legendary = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    is_mythical = atoi(temp.c_str());
    m->is_mythical = is_mythical;
    }
    else {
        m->is_mythical = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    order = atoi(temp.c_str());
    m->order = order;
    }
    else {
        m->order = INT_MAX;
    }

    getline(scanner, temp);
    if(temp != "") {
    conquest_order = atoi(temp.c_str());
    m->conquest_order = conquest_order;
    }
    else {
        m->conquest_order = INT_MAX;
    }

    pokemon_species->push_back(m);
    line = "";
    }

}

void experience_parser(std::vector<experience_t*> *experience) {
    char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/experience.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/experience.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("experience.csv");
        }
    }
   // std::ifstream scanner;
   // scanner.open("experience.csv");
    std::string line = "";
    experience_t* m;
    std::string temp;

    int growth_rate_id;
    int level;
    int exp;

        if(!scanner.is_open()) {
        exit(EXIT_FAILURE);
    }

    getline(scanner, line);

    while(scanner.good()) { 
        m = (experience_t*) malloc(sizeof(*m));

    getline(scanner, temp, ',');
    if(temp != "") {
    growth_rate_id = atoi(temp.c_str());
    m->growth_rate_id = growth_rate_id;
    }
    else 
    {
        m->growth_rate_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    level = atoi(temp.c_str());
    m->level = level;
    }
    else 
    {
        m->level = INT_MAX;
    }        

    getline(scanner, temp);
    if(temp != "") {
    exp = atoi(temp.c_str());
    m->experience = exp;
    }
    else 
    {
        m->experience = INT_MAX;
    }

    experience->push_back(m);
    line = "";    

    }
}

void type_names_parser(std::vector<type_names_t*> *type_names) {
    char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/type_names.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/type_names.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("type_names.csv");
        }
    }
    //std::ifstream scanner;
    //scanner.open("type_names.csv");
    std::string line = "";
    type_names_t* m;
    std::string temp;

    int type_id;
    int local_lang_id;
 //   char* name;

    getline(scanner, line);

    while(scanner.good()) { 
        m = (type_names_t*) malloc(sizeof(*m));
    getline(scanner, temp, ',');
    if(temp != "") {
    type_id = atoi(temp.c_str());
    m->type_id = type_id;
    }
    else 
    {
        m->type_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    local_lang_id = atoi(temp.c_str());
    m->local_lang_id = local_lang_id;
    }
    else 
    {
        m->local_lang_id = INT_MAX;
    }

    getline(scanner, temp);
    m->name = (char*) strdup(temp.c_str());

    if(local_lang_id == 9) {
    type_names->push_back(m);

    }
    line = "";
    }
}


void pokemon_stats_parser(std::vector<pokemon_stats_t*> *pokemon_stats) {

    char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/pokemon_stats.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_stats.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("pokemon_stats.csv");
        }
    }

    //std::ifstream scanner;
    //scanner.open("pokemon_stats.csv");
    std::string line = "";
    pokemon_stats_t* m;
    std::string temp;

    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;

    if(!scanner.is_open()) {
        exit(EXIT_FAILURE);
    }

    getline(scanner, line);

    while(scanner.good()) { 
    m = (pokemon_stats_t*) malloc(sizeof(*m));
    getline(scanner, temp, ',');
    if(temp != "") {
    pokemon_id = atoi(temp.c_str());
    m->pokemon_id = pokemon_id;
    }
    else 
    {
        m->pokemon_id = INT_MAX;
    }
    

    getline(scanner, temp, ',');
    if(temp != "") {
    stat_id = atoi(temp.c_str());
    m->stat_id = stat_id;
    }
    else 
    {
        m->stat_id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    base_stat = atoi(temp.c_str());
    m->base_stat = base_stat;
    }
    else 
    {
        m->base_stat = INT_MAX;
    }

    getline(scanner, temp);
    if(temp != "") {
    effort = atoi(temp.c_str());
    m->effort = effort;
    }
    else 
    {
        m->effort = INT_MAX;
    }

    pokemon_stats->push_back(m);
    line = "";
    }
}

void stats_parser(std::vector<stats_t*> *stats) {
    char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/stats.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/stats.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("stats.csv");
        }
    }

//     std::ifstream scanner;
//    scanner.open("stats.csv");
    std::string line = "";
    stats_t* m;
    std::string temp;
    std::string identifier;

    int id;
    int damage_c;
    int is_battle_only;
    int game_index;

    if(!scanner.is_open()) {
        exit(EXIT_FAILURE);
    }

    getline(scanner, line);
    while(scanner.good()) { 
        m = (stats_t*) malloc(sizeof(*m));

    getline(scanner, temp, ',');
    if(temp != "") {
    id = atoi(temp.c_str());
    m->id = id;
    }
    else 
    {
        m->id = INT_MAX;
    }

    getline(scanner, temp, ',');
    if(temp != "") {
    damage_c = atoi(temp.c_str());
    m->damage_c = damage_c;
    }
    else 
    {
        m->damage_c = INT_MAX;
    }

    getline(scanner, identifier, ',');
    m->identifier = (char*) strdup(identifier.c_str());

    getline(scanner, temp, ',');
    if(temp != "") {
    is_battle_only = atoi(temp.c_str());
    m->is_battle_only = is_battle_only;
    }
    else 
    {
        m->is_battle_only = INT_MAX;
    }

    getline(scanner, temp);
    if(temp != "") {
    game_index = atoi(temp.c_str());
    m->game_index = game_index;
    }
    else 
    {
        m->game_index = INT_MAX;
    }

    stats->push_back(m);
    line = "";
    }

}

void pokemon_types_parser(std::vector<pokemon_types_t*> *pokemon_types) {
    char* home = getenv("HOME");
    std::string t = "/.poke327/pokedex/pokedex/data/csv/pokemon_types.csv";
    char* address = (char*) strdup(t.c_str());
    char* location = (char*) malloc(1+ strlen(home) + strlen(address));
    strcpy(location, home);
    strcat(location, address);

    std::ifstream scanner;
    scanner.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_types.csv");
    if(!scanner.is_open()) {
        scanner.open(location); 
        if(!scanner.is_open()) {
            scanner.open("pokemon_types.csv");
        }
    }

//    std::ifstream scanner;
//    scanner.open("pokemon_types.csv");
    std::string line = "";
    pokemon_types_t* m;
    std::string temp;

    int pokemon_id;
    int type_id;
    int slot;

    if(!scanner.is_open()) {
        exit(EXIT_FAILURE);
    }

    getline(scanner, line);

    while(scanner.good()) { 
        m = (pokemon_types_t*) malloc(sizeof(*m));

    getline(scanner, temp, ',');
    if(temp != "") {
    pokemon_id = atoi(temp.c_str());
    m->pokemon_id = pokemon_id;
    }
    else 
    {
        m->pokemon_id = INT_MAX;
    }
    


    getline(scanner, temp, ',');
    if(temp != "") {
    type_id = atoi(temp.c_str());
    m->type_id = type_id;
    }
    else 
    {
        m->type_id = INT_MAX;
    }

    getline(scanner, temp);
    if(temp != "") {
    slot = atoi(temp.c_str());
    m->slot = slot;
    }
    else 
    {
        m->slot = INT_MAX;
    }

    pokemon_types->push_back(m);
    line = "";

    }
}