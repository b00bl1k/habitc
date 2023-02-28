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

#ifndef __HABITS_H__
#define __HABITS_H__

#include "llists.h"

struct habit {
    struct llist_item llist;
    unsigned long int quantity;
    char *name;
};

extern struct habit *habits;
extern size_t habits_name_len_max;

/**
 * \brief Load habits from the file
 */
int habits_load(void);

/**
 * \brief Save habits to the file
 */
int habits_save(void);

/**
 * \brief Search habit by name
 *
 * \return NULL if not found
 */
struct habit *habits_find(const char *name);

/**
 * \brief Add a new habit
 *
 * \param name pointer to the habit's name, will be borrowed
 * \return non zero on success
 */
int habits_add(char *name, unsigned long int quantity);

/**
 * \brief Remove habit
 */
void habits_remove(struct habit *habit);

/**
 * \brief Remove all habits and free memory
 */
void habits_free(void);

#endif /* ~__HABITS_H__ */
