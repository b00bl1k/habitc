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

#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "activities.h"
#include "config.h"
#include "utils.h"

static struct activity *activities;
static size_t activities_count;
static size_t activities_size;

static int activity_cmp(const struct activity *act, const struct tm *tm,
    const struct habit *habit)
{
    return (act->habit == habit && act->day == tm->tm_mday
            && act->month == tm->tm_mon && act->year == tm->tm_year);
}

struct activity *activity_find(const struct tm *tm,
    const struct habit *habit)
{
    struct activity *act;

    for (size_t i = 0; i < activities_count; i++) {
        act = &activities[i];
        if (activity_cmp(act, tm, habit))
            return act;
    }

    return NULL;
}

static int activity_add(struct tm *tm, const struct habit *habit,
    unsigned long int quantity)
{
    struct activity *act;

    if (activities_count) {
        act = &activities[activities_count - 1];
        if (activity_cmp(act, tm, habit)) {
            act->quantity += quantity;
            return 1;
        }

        // TODO search from end?

        act = activity_find(tm, habit);
        if (act) {
            act->quantity += quantity;
            return 1;
        }
    }

    if (((activities_count + 1) * sizeof(*act)) > activities_size) {
        size_t new_size = (activities_count + 10) * sizeof(*act);

        if (activities)
            activities = realloc(activities, new_size);
        else
            activities = malloc(new_size);

        if (!activities)
            return 0;

        activities_size = new_size;
    }

    act = &activities[activities_count];
    act->year = tm->tm_year;
    act->month = tm->tm_mon;
    act->day = tm->tm_mday;
    act->habit = habit;
    act->quantity = quantity;
    activities_count++;

    return 1;
}

int activities_add(const char *name, unsigned long int quantity)
{
    char ts[sizeof("2023-02-23T12:35:55+00:00")];

    time_t now = time(NULL);
    strftime(ts, sizeof(ts), "%FT%T%z", localtime(&now));

    const char *fname = conf_get_activities_file();

    FILE *f = fopen(fname, "a");
    if (!f)
        return 0;

    fprintf(f, "%s,%s,%u\n", ts, name, quantity);
    fclose(f);

    return 1;
}

int activities_load(const time_t *min, const time_t *max,
    const struct habit *habit)
{
    const char *fname = conf_get_activities_file();

    FILE *f = fopen(fname, "r");
    if (!f)
        return 0;

    ssize_t length;
    size_t n = 0;
    char *line = NULL;
    char *parts[3];
    const struct habit *curr_habit = habit;

    tzset();

    while ((length = getline(&line, &n, f)) != -1) {
        struct tm tm = {0};

        u_rtrim(line);

        int count = u_split(line, parts, ',', ARRAY_SIZE(parts));
        if (count != 3)
            continue;

        const char *s = strptime(parts[0], "%FT%T%z", &tm);
        if (!s || *s != '\0') {
            printf("invalid date %s\n", parts[0]);
            continue;
        }

        if (!curr_habit || (strcmp(curr_habit->name, parts[1]) != 0)) {
            if (habit)
                continue;

            curr_habit = habits_find(parts[1]);
            if (!curr_habit) {
                printf("unknown habit %s\n", parts[1]);
                continue;
            }
        }

        unsigned long int q;
        if (!u_stul(parts[2], &q)) {
            printf("invalid quantity %s\n", parts[2]);
            continue;
        }

        int gmtoff = tm.tm_gmtoff;
        time_t ts = mktime(&tm) - (gmtoff + timezone);

        if (min && ts < *min)
            continue;

        if (max && ts > *max)
            continue;

        struct tm *t = localtime(&ts);

        if (!activity_add(t, curr_habit, q))
            puts("activity load error");
    }

    if (line != NULL)
        free(line);

    fclose(f);

    return 1;
}

void activities_unload()
{
    if (activities) {
        free(activities);
        activities = NULL;
        activities_count = 0;
        activities_size = 0;
    }
}
