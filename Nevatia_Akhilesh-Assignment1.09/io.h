#ifndef IO_H
# define IO_H

# include "pair.h"
# include "poke327.h"

class character;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_handle_input(pair_t dest);
void io_queue_message(const char *format, ...);
void io_battle(character *aggressor, character *defender);
void pokemon_battle(pc *p,  poke *pp);
void attack_selection(pc *p, poke *pp, int pI);
void attack_round(pc *p, poke *pp, int pI, int moveSelection);

#endif
