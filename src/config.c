#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"

const char *env_home;
char file_path[256];

static void ensure_conf_dir_exists()
{
    env_home = getenv("HOME");
    snprintf(file_path, sizeof(file_path), "%s/.local/share/habitc", env_home);

    struct stat st;
    if (stat(file_path, &st) == -1) {
        mkdir(file_path, 0755);
    }
}

const char *conf_get_habits_file()
{
    ensure_conf_dir_exists();

    snprintf(file_path, sizeof(file_path), "%s/.local/share/habitc/habits.csv", env_home);

    return file_path;
}

const char *conf_get_activities_file()
{
    ensure_conf_dir_exists();

    snprintf(file_path, sizeof(file_path), "%s/.local/share/habitc/activities.csv", env_home);

    return file_path;
}
