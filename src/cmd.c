#include <stdio.h>
#include  <avr/pgmspace.h>
#include "action.h"
#include "cmd.h"

#define STATE_BEGIN 0
#define STATE_A 1
#define STATE_A_F 2
#define STATE_R 3
#define STATE_R_F 4
#define STATE_W 5
#define STATE_W_F 6

struct _info {
        char * str;
        char sym;
        unsigned int count;
} inf;

static void next(struct _info *info);
static int skip(struct _info * info);

int cmd_parse(char *str, action_t *act)
{
        int result = 0;
        unsigned char state = STATE_BEGIN;

        act->addrFlags = 0;
        act->readSize = 0;
        act->writeSize = 0;

        inf.str = str;
        inf.count = 0;
        next(&inf);

        while (inf.sym != 0) {
                switch (state) {
                case STATE_BEGIN:
                        if (inf.sym == 'A' || inf.sym == 'a') {
                                state = STATE_A;
                                act->addrFlags = ACTION_ADDRESS_GET;
                        } else if (inf.sym == 'R' || inf.sym == 'r') {
                                state = STATE_R;
                        } else if (inf.sym == 'W' || inf.sym == 'w') {
                                state = STATE_W;
                        } else {
                                result = -1;
                        }
                        next(&inf);
                        break;
                case STATE_A:
                        if (skip(&inf) == 0) {
                                result = -1;
                        } else {
                                state = STATE_A_F;
                        }
                        break;
                case STATE_A_F:
                        if (inf.sym >= '0' && inf.sym <= '9') {
                                //TODO получить численный аргумент
                                // И в случае успеха установить флаг.
                                act->addrFlags = ACTION_ADDRESS_GET;
                        } else if (inf.sym == 'R' || inf.sym == 'r') {
                                state = STATE_R;
                        } else if (inf.sym == 'W' || inf.sym == 'w') {
                                state = STATE_W;
                        } else {
                                result = -1;
                        }
                        next(&inf);
                        break;
                case STATE_R:
                        if (skip(&inf) == 0) {
                                result = -1;
                        } else {
                                state = STATE_R_F;
                        }
                        break;
                case STATE_R_F:
                        if (inf.sym >= '0' && inf.sym <= '9') {
                                //TODO получить количество считываемых данных
                        } else {
                                result = -1;
                        }
                        break;
                case STATE_W:
                        if (skip(&inf) == 0) {
                                result = -1;
                        } else {
                                state = STATE_W_F;
                        }
                        break;
                case STATE_W_F:
                        if (inf.sym >= '0' && inf.sym <= '9') {
                                //TODO получить записываемые данные
                        } else if (inf.sym == 'R' || inf.sym == 'r') {
                                state = STATE_R;
                        } else {
                                result = -1;
                        }
                }

                if (result != 0) {
                        break;
                }
        }

        if (result != 0) {
                fputs_P(PSTR("Syntax error"), stderr);

                puts(str);

                while (inf.count > 0) {
                        putchar('-');
                        inf.count--;
                }
                fputs_P(PSTR("^"), stderr);
        }
        return result;
}

static void next(struct _info * info)
{
        info->sym = *info->str++;
        info->count++;
}

/**
 * Перебирает строку пока не встретит непробельный символ или ноль.
 */
static int skip(struct _info * info)
{
        int result = 0;
        while (info->sym <= 0x20 && info->sym > 0) {
                info->sym = *info->str++;
                info->count++;
                result++;
        }
        return result;
}
