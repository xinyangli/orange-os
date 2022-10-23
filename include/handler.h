#ifndef ORANGE_OS_HANDLER_H
#define ORANGE_OS_HANDLER_H
#include "protect.h"

void empty_handler();
void clock_handler();
void KeyBoardHandler();
void init_gate(GATE *p_gate, u8 type, void *handler, u8 privilege);
void set_8259a();

#endif //ORANGE_OS_HANDLER_H