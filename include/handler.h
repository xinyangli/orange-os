#ifndef ORANGE_OS_HANDLER_H
#define ORANGE_OS_HANDLER_H
#include "x86def.h"

void empty_handler();
void clock_handler();
void KeyBoardHandler();
void set_8259a();

#endif //ORANGE_OS_HANDLER_H