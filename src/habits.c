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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "config.h"
#include "habits.h"
#include "utils.h"

struct habit *habits;
size_t habits_name_len_max;

int habits_load()
{
    const char *fname = conf_get_habits_file();

    FILE *f = fopen(fname, "r");
    if (!f)
        return 0;

    ssize_t length;
    size_t n = 0;
    char *line = NULL;
    char *parts[2];

    while ((length = getline(&line, &n, f)) != -1) {
        // remove tralinig spaces
        u_rtrim(line);

        int count = u_split(line, parts, ',', 2);
        if (count != 2)
            continue;

        unsigned long int q;
        if (!u_stul(parts[1], &q))
            continue;

        struct habit *h = malloc(sizeof(*h));
        if (h) {
            size_t name_len = u_utf8_strlen(parts[0]);
            if (habits_name_len_max < name_len)
                habits_name_len_max = name_len;
            h->name = parts[0];
            h->quantity = q;
            llist_insert((struct llist_item **)&habits, (struct llist_item *)h);
            n = 0;
            line = NULL;
        }
    }

    if (line != NULL)
        free(line);

    fclose(f);
}

int habits_save()
{
    const char *fname = conf_get_habits_file();

    FILE *f = fopen(fname, "w");
    if (!f)
        return 0;

    struct habit *h = (struct habit *)habits;
    while (h != NULL) {
        fprintf(f, "%s,%u\n", h->name, h->quantity);
        h = (struct habit *)h->llist.next;
    }

    fclose(f);

    return 1;
}

struct habit *habits_find(const char *name)
{
    struct habit *h = (struct habit *)habits;

    while (h != NULL) {
        if (strcmp(h->name, name) == 0)
            return h;

        h = (struct habit *)h->llist.next;
    }

    return 0;
}

int habits_add(char *name, unsigned long int quantity)
{
    if (habits_find(name)) {
        // already exists
        return 0;
    }

    struct habit *h = malloc(sizeof(*h));

    if (!h)
        return 0;

    size_t name_len = u_utf8_strlen(name);
    if (habits_name_len_max < name_len)
        habits_name_len_max = name_len;
    h->name = name;
    h->quantity = quantity;
    llist_insert((struct llist_item **)&habits, (struct llist_item *)h);

    return 1;
}

void habits_remove(struct habit *habit)
{
    llist_remove((struct llist_item **)&habits, (struct llist_item *)habit);
}

void habits_free()
{
    while (habits != NULL) {
        struct habit *h = (struct habit *)habits;
        free(h->name);
        llist_remove((struct llist_item **)&habits, (struct llist_item *)h);
    }
}
