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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "activities.h"
#include "commands.h"
#include "habits.h"
#include "utils.h"

#define HABITC_VERSION "0.1.0"

static int parse_opts(int argc, char *const *argv);
static int parse_command(const char *cmd_name);
static void usage(void);

enum commands {
    CMD_NOT_SPECIFIED,
    CMD_ADD,
    CMD_DEL,
    CMD_LIST,
    CMD_LOG,
};

struct app_command {
    const char *name;
    enum commands cmd;
    const char *description;
    const char *usage;
};

enum commands app_command = CMD_NOT_SPECIFIED;
const struct app_command app_commands[] = {
    {"add", CMD_ADD, "\tAdd a habit.", "habitc add [-h] [-q num] name"},
    {"del", CMD_DEL, "\tDelete a habit.", "habitc del [-h] name"},
    {"list", CMD_LIST, "\tList all tracked habits", "habitc list [-h]"},
    {"log", CMD_LOG, "\tCheckin progress for a habit.", "habitc log [-h] [-q num] name"},
};
const char *command_arg = NULL; // additional positional arg for the commands
unsigned long int quantity = 1;

int main(int argc, char *const *argv)
{
    if (parse_opts(argc, argv))
        return 0;

    habits_load();

    switch (app_command) {
    case CMD_NOT_SPECIFIED:
        usage();
        break;

    case CMD_ADD:
        command_add(command_arg, quantity);
        break;

    case CMD_DEL:
        command_del(command_arg);
        break;

    case CMD_LIST:
        command_list();
        break;

    case CMD_LOG:
        command_log(command_arg, quantity);
        break;
    }

    habits_free();

    return 0;
}

static int parse_opts(int argc, char *const *argv)
{
    const char *opts = "-hvq:";

    int opt = getopt(argc, argv, opts);
    while (opt != -1)
    {
        switch (opt)
        {
        case 1:
            if (app_command != CMD_NOT_SPECIFIED) {
                if (command_arg == NULL)
                    command_arg = optarg;
                else {
                    puts("invalid usage");
                    return 1;
                }
            }
            else if (!parse_command(optarg)) {
                puts("unknown command specified");
                return 1;
            }
            break;

        case 'h':
            if (app_command == CMD_NOT_SPECIFIED)
                usage();
            else
                printf("Usage: %s\n", app_commands[app_command - 1].usage);
            return 1;

        case 'v':
            puts("habitc v" HABITC_VERSION);
            return 1;

        case 'q':
            if (!u_stul(optarg, &quantity) || (quantity == 0)) {
                puts("invalid quantity");
                return 1;
            }
            break;
        }

        opt = getopt(argc, argv, opts);
    }

    return 0;
}

static int parse_command(const char *cmd_name)
{
    for (int i = 0; i < ARRAY_SIZE(app_commands); i++) {
        if (strcmp(app_commands[i].name, cmd_name) == 0) {
            app_command = app_commands[i].cmd;
            return 1;
        }
    }

    return 0;
}

static void usage()
{
    puts("Usage: habitc [options] <command> [args]");
    puts("\nOptions:");
    puts("  -h\tShow help and exit.");
    puts("\nCommands:");

    for (int i = 0; i < ARRAY_SIZE(app_commands); i++)
        printf("  %s%s\n", app_commands[i].name, app_commands[i].description);
}
