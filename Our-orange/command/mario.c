#include "stdio.h"
#include "type.h"

int main(int argc, char * argv[])
{
    char SUPER_MARIO[] = {
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 42, 42,
    42, 42, 42, 42, 42, 42, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 10, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35, 35,
    46, 46, 46, 46, 35, 46, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 35, 46, 46, 35, 35, 35, 46, 46, 46, 46, 46, 35, 35, 46, 46, 46,
    46, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35,
    46, 46, 46, 46, 46, 46, 46, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 35, 35, 35, 10, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
    46, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 46,
    46, 46, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 46, 46, 46, 35,
    10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35,
    42, 35, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 35, 46, 35, 46, 35, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 35, 46, 35, 46, 35, 10, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 35, 35, 35, 35, 42, 42, 42, 42, 42, 42, 42, 35, 35, 35,
    35, 35, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 46,
    35, 46, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 46, 35, 46, 35,
    10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 46, 46, 46, 35, 42, 42,
    42, 46, 42, 42, 42, 42, 46, 42, 35, 35, 35, 46, 46, 46, 46, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 35, 46, 46, 46, 35, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 35, 46, 46, 46, 35, 10, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 46, 46, 46, 46, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 35,
    35, 46, 46, 46, 46, 46, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35,
    35, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35, 10,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 46, 46, 46, 46, 42, 42, 42,
    42, 32, 32, 32, 32, 42, 42, 42, 42, 42, 46, 46, 46, 46, 10, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35, 35, 32, 32, 32, 32,
    32, 32, 32, 32, 35, 35, 35, 35, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35,
    35, 35, 35, 35, 10, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 10, 35, 46, 46, 46,
    35, 46, 46, 46, 46, 46, 46, 35, 46, 35, 35, 46, 46, 46, 35, 46, 46, 46,
    46, 46, 46, 35, 46, 35, 35, 46, 46, 46, 35, 46, 46, 46, 46, 46, 46, 35,
    46, 35, 35, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    45, 45, 45, 35, 10, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 45, 45, 45, 45, 45, 45,
    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 35, 10, 35, 46, 46, 35,
    46, 46, 46, 46, 35, 46, 46, 46, 46, 35, 35, 46, 46, 35, 46, 46, 46, 46,
    35, 46, 46, 46, 46, 35, 35, 46, 46, 35, 46, 46, 46, 46, 35, 46, 46, 46,
    46, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 10, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 35, 45, 45,
    45, 45, 45, 45, 45, 45, 45, 45, 35, 10, 35, 46, 46, 46, 46, 46, 35, 46,
    46, 46, 46, 46, 46, 35, 35, 46, 46, 46, 46, 46, 35, 46, 46, 46, 46, 46,
    46, 35, 35, 46, 46, 46, 46, 46, 35, 46, 46, 46, 46, 46, 46, 35, 32, 32,
    32, 32, 35, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 35, 10, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 32, 32, 32, 32, 35, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    35, 10, 35, 46, 35, 46, 46, 35, 46, 46, 46, 46, 35, 46, 46, 35, 35, 46,
    35, 46, 46, 35, 46, 46, 46, 46, 35, 46, 46, 35, 35, 46, 35, 46, 46, 35,
    46, 46, 46, 46, 35, 46, 46, 35, 32, 32, 32, 32, 35, 45, 45, 45, 45, 45,
    45, 45, 45, 45, 45, 35, 10, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 35,
    35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 0};
	int i;
	for(i=0;i<sizeof(SUPER_MARIO);i++){
        printf("%c", SUPER_MARIO[i]);
    }
    printf("\n");
    return 0;
}