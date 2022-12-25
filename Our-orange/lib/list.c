#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PUBLIC char* list()
{
    MESSAGE msg;
    msg.type = LIST;
    send_recv(BOTH, TASK_FS, &msg);
    return msg.BUF2;
}
