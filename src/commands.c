/**
 * MIT License
 *
 * Copyright (c) 2023 Alexey Ryabov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "activities.h"
#include "commands.h"
#include "habits.h"
#include "utils.h"

void command_add(const char *name, unsigned long int quantity)
{
    char *d_name = strdup(name);

    if (!d_name) {
        printf("Couldn't add a new '%s' habit\n", name);
    }
    else if (!habits_add(d_name, quantity)) {
        printf("Couldn't add a new '%s' habit\n", name);
        free(d_name);
    }
    else {
        habits_save();
        printf("The habit '%s' with quantity %lu has been added\n", name,
            quantity);
    }
}

void command_del(const char *name)
{
    struct habit *h = habits_find(name);

    if (!h) {
        printf("The '%s' habit not found\n", name);
    }
    else {
        habits_remove(h);
        habits_save();
        printf("The habit '%s' has been deleted\n", name);
    }
}

void command_list()
{
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_mday -= 6;
    time_t min = mktime(&tm);

    activities_load(&min, &now, NULL);

    struct habit *h = (struct habit *)habits;

    while (h != NULL) {
        int pad = habits_name_len_max +
            (strlen(h->name) - u_utf8_strlen(h->name));
        printf("% *s: ", pad, h->name);
        struct tm dt = *localtime(&now);
        for (int day = 0; day < 7; day++) {
            struct activity *act = activity_find(&dt, h);
            if (!act)
                printf("\u2B1A ");
            else if (act->quantity < h->quantity)
                printf("\u2B13 ");
            else
                printf("\u25A0 ");

            dt.tm_mday--;
            mktime(&dt);
        }
        putchar('\n');
        h = (struct habit *)h->llist.next;
    }

    activities_unload();
}

void command_log(const char *name, unsigned long int quantity)
{
    struct habit *h = habits_find(name);

    if (!h) {
        printf("The '%s' habit not found\n", name);
    }
    else {
        if (activities_add(name, quantity))
            printf("Added %lu times to habit '%s' for today\n", quantity,
                name);
        else
            puts("Something went wrong");
    }
}
