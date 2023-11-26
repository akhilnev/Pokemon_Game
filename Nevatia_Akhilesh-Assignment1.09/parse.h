#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <string>
#include <climits>

//
typedef struct pokemon {
    int id;
 //   std::string identifier;
    char* identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;

} pokemon_t;

typedef struct moves {
    int id; 
    char* identifier;
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
    
} moves_t;

typedef struct pokemon_moves {
    
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

} pokemon_moves_t;

typedef struct pokemon_species {

    int id;
    char* identifier;
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

} pokemon_species_t;

typedef struct experience {

    int growth_rate_id;
    int level;
    int experience;

} experience_t;

typedef struct type_names {
    int type_id;
    int local_lang_id;
    char* name;
} type_names_t;

typedef struct pokemon_stats {

    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;

} pokemon_stats_t;

typedef struct stats {

    int id;
    int damage_c;
    char* identifier;
    int is_battle_only;
    int game_index;

} stats_t;

typedef struct pokemon_types {

    int pokemon_id;
    int type_id;
    int slot;

} pokemon_types_t;

void pokemon_parser(std::vector<pokemon_t*> *pokemon);
void moves_parser(std::vector<moves_t*> *moves);
void pokemon_moves_parser(std::vector<pokemon_moves_t*> *pokemon_moves);
void pokemon_species_parser(std::vector<pokemon_species_t*> *pokemon_species);
void experience_parser(std::vector<experience_t*> *experience);
void type_names_parser(std::vector<type_names_t*> *type_names);
void pokemon_stats_parser(std::vector<pokemon_stats_t*> *pokemon_stats);
void stats_parser(std::vector<stats_t*> *stats);
void pokemon_types_parser(std::vector<pokemon_types_t*> *pokemon_types);
//void print_pokemon(std::vector<pokemon_t*> *pokemon);