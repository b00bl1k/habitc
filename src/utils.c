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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

void u_rtrim(char *s)
{
    char *delim;

    // TODO improve

    delim = strstr(s, "\n");
    if (delim)
        *delim = 0;
}

int u_stul(const char *s, unsigned long int *d)
{
    char *endp;
    errno = 0;
    *d = strtoul(s, &endp, 10);

    return (errno == 0 && *endp == '\0');
}

int u_split(char *str, char **arr, char c, unsigned int max)
{
    unsigned int count;
    char s[2] = {c, '\0'};

    char *token = strtok(str, s);

    for (count = 0; (count < max) && (token != NULL); count++) {
        arr[count] = token;

        token = strtok(NULL, s);
    }

    return count;
}

size_t u_utf8_strlen(const char *str)
{
    size_t len = 0;

    while (*str != '\0')
    {
        unsigned char c = *str;

        len++;

        if ((c & 0x80) == 0)
            str += 1;
        else if ((c & 0xe0) == 0xc0)
            str += 2;
        else if ((c & 0xf0) == 0xe0)
            str += 3;
        else if ((c & 0xf8) == 0xf0)
            str += 4;
        else
            return 0;
    }

    return len;
}
