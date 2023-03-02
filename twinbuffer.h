/*
Group - 29

Shaurya Marwah - 2019B3A70459P
Hari Sankar - 2019B3A70564P
Ruchir Kumbhare - 2019B5A70650P
Ashwin Murali - 2019B2A70957P
Dilip Venkatesh - 2020A7PS1203P

*/

#ifndef TWINBUFFER_H
#define TWINBUFFER_H

#include <stdio.h>
#include <stdbool.h>

#define SIZE 50

typedef struct twinbuffer
{
    char *buffer;
    int begin;
    int fwd;
    FILE *fp;
    bool end;
    int whichReloaded;
    int buffer_size;
} twinbuffer;

twinbuffer *twinbuffer_init(FILE *fp, int buffer_size);

char readOneCharacter(twinbuffer *tb);

void retract(int num, twinbuffer *tb);

#endif