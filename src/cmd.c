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
#define STATE_R_F_D 7
#define STATE_A_F_D 8

#define ERROR_OK 0
#define ERROR_EXPECT_AWR 1
#define ERROR_EXPECT_WHITE 2
#define ERROR_EXPECT_RW 3
#define ERROR_EXPECT_RW_D 4
#define ERROR_EXPECT_R 5
#define ERROR_EXPECT_R_D 6

#define  ERROR_EXPECT_DIGIT 101
#define  ERROR_UNUSABLE 102
#define  ERROR_OVERFLOW 103

struct _info {
        char * str;
        char sym;
        unsigned int count;
} inf;

static void next(struct _info *info);
static int toint(struct _info * info, unsigned char *res);

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
                        if (inf.sym > 0 && inf.sym <= ' ') {
                                // Улыбаемся и машем пока пропускаем пробелы.
                        } else if (inf.sym == 'A' || inf.sym == 'a') {
                                state = STATE_A;
                                act->addrFlags = ACTION_ADDRESS_GET;
                        } else if (inf.sym == 'R' || inf.sym == 'r') {
                                state = STATE_R;
                        } else if (inf.sym == 'W' || inf.sym == 'w') {
                                state = STATE_W;
                        } else {
                                result = ERROR_EXPECT_AWR;
                        }
                        next(&inf);
                        break;
                case STATE_A:
                        if (inf.sym >= '0' && inf.sym <= '9') {
                                state = STATE_A_F;
                        } else if (inf.sym > 0 && inf.sym <= ' ') {
                                state = STATE_A_F;
                                next(&inf);
                        } else {
                                result = ERROR_EXPECT_WHITE;
                        }
                        break;
                case STATE_A_F:
                        if (inf.sym >= '0' && inf.sym <= '9') {
                                // Получить численный аргумент
                                // И в случае успеха установить флаг.
                                result = toint(&inf, &act->address);
                                if (result == ERROR_OK) {
                                        act->addrFlags = ACTION_ADDRESS_SET;
                                }
                                state = STATE_A_F_D;
                        } else if (inf.sym == 'R' || inf.sym == 'r') {
                                state = STATE_R;
                        } else if (inf.sym == 'W' || inf.sym == 'w') {
                                state = STATE_W;
                        } else if (inf.sym > 0 && inf.sym <= ' ') {
                                // nop
                        } else {
                                result = ERROR_EXPECT_RW_D;
                        }
                        next(&inf);
                        break;
                case STATE_A_F_D:
                        if (inf.sym == 'R' || inf.sym == 'r') {
                                state = STATE_R;
                        } else if (inf.sym == 'W' || inf.sym == 'w') {
                                state = STATE_W;
                        } else if (inf.sym > 0 && inf.sym <= ' ') {
                                // nop
                        } else {
                                result = ERROR_EXPECT_RW_D;
                        }
                        next(&inf);
                        break;
                case STATE_R:
                        if (inf.sym >= '0' && inf.sym <= '9') {
                                state = STATE_R_F;
                        } else if (inf.sym > 0 && inf.sym <= ' ') {
                                state = STATE_R_F;
                                next(&inf);
                        } else {
                                result = ERROR_EXPECT_WHITE;
                        }
                        break;
                case STATE_R_F:
                        if (inf.sym > 0 && inf.sym <= ' ') {
                                next(&inf);
                        } else if (inf.sym >= '0' && inf.sym <= '9') {
                                // Получить количество считываемых данных
                                unsigned char r = 0;
                                result = toint(&inf, &r);
                                if (result == ERROR_OK) {
                                        act->readSize = (unsigned int) r;
                                }
                                state = STATE_R_F_D;
                        } else {
                                result = ERROR_EXPECT_DIGIT;
                        }
                        break;
                case STATE_R_F_D:
                        if (inf.sym > 0 && inf.sym <= ' ') {
                                next(&inf);
                        } else {
                                result = ERROR_EXPECT_WHITE;
                        }
                        break;
                case STATE_W:
                        if (inf.sym >= '0' && inf.sym <= '9') {
                                state = STATE_W_F;
                        } else if (inf.sym > 0 && inf.sym <= ' ') {
                                state = STATE_W_F;
                                next(&inf);
                        } else {
                                result = ERROR_EXPECT_WHITE;
                        }
                        break;
                case STATE_W_F:
                        if (inf.sym > 0 && inf.sym <= ' ') {
                                next(&inf);
                        } else if (inf.sym >= '0' && inf.sym <= '9') {
                                // Получить записываемые данные
                                result = toint(&inf,
                                                &act->buff[act->writeSize]);
                                if (result == ERROR_OK) {
                                        act->writeSize++;
                                }
                        } else if (inf.sym == 'R' || inf.sym == 'r') {
                                state = STATE_R;
                                next(&inf);
                        } else {
                                result = ERROR_EXPECT_R_D;
                        }
                }

                if (result != ERROR_OK) {
                        break;
                }
        }

        if (result != ERROR_OK) {
                fprintf_P(stderr, PSTR("Error: "));

                switch (result) {
                case ERROR_EXPECT_AWR:
                        fprintf_P(stderr, PSTR("expect 'a' 'w' or 'r'\n"));
                        break;
                case ERROR_EXPECT_WHITE:
                        fprintf_P(stderr, PSTR("expect white space\n"));
                        break;
                case ERROR_EXPECT_RW_D:
                        fprintf_P(stderr, PSTR("expect 'w' 'r' or digit\n"));
                        break;
                case ERROR_EXPECT_RW:
                        fprintf_P(stderr, PSTR("expect 'w' or 'r'\n"));
                        break;
                case ERROR_EXPECT_R_D:
                        fprintf_P(stderr, PSTR("expect 'r' or digit\n"));
                        break;
                case ERROR_EXPECT_R:
                        fprintf_P(stderr, PSTR("expect 'r'\n"));
                        break;
                case ERROR_EXPECT_DIGIT:
                        fprintf_P(stderr, PSTR("expect digit\n"));
                        break;
                case ERROR_UNUSABLE:
                        fprintf_P(stderr, PSTR("unusable digit\n"));
                        break;
                case ERROR_OVERFLOW:
                        fprintf_P(stderr, PSTR("overflow\n"));
                        break;
                default:
                        fprintf_P(stderr, PSTR("unknown error\n"), stderr);
                }

                fputs(str, stderr);

                while (inf.count > 2) {
                        fputc('-', stderr);
                        inf.count--;
                }
                fputs_P(PSTR("^\n"), stderr);
        }
        return result;
}

static void next(struct _info * info)
{
        info->sym = *info->str++;
        info->count++;
}

/**
 * Получение числа из строки.
 * \param info Указатель на структуру _info
 * \param res Указатель на переменную для сохранения результата.
 * \return Ноль, если число было успешно считано.
 */
static int toint(struct _info * info, unsigned char *res)
{
        unsigned int result = 0;
        unsigned char radix = 10;

        if (info->sym == '0') {
                next(info);
                if (info->sym == 'x' || info->sym == 'X') {
                        radix = 16;
                        next(info);
                } else if (info->sym == 'b' || info->sym == 'B') {
                        radix = 2;
                        next(info);
                } else {
                        radix = 8;
                }
        }

        unsigned char t;

        while (info->sym != 0) {
                if (info->sym >= '0' && info->sym <= '9') {
                        t = info->sym - '0';
                } else if (info->sym >= 'a' && info->sym <= 'f') {
                        t = info->sym - 'a' + 10;
                } else if (info->sym >= 'A' && info->sym <= 'F') {
                        t = info->sym - 'A' + 10;
                } else if (info->sym >= 0 && info->sym <= ' ') {
                        break;
                } else {
                        return ERROR_EXPECT_DIGIT;
                }

                if (t >= radix) {
                        return ERROR_UNUSABLE;
                }

                next(info);

                result = result * radix + t;

                if (result > 255) {
                        return ERROR_OVERFLOW;
                }
        }

        *res = result & 0xff;
        return 0;
}
