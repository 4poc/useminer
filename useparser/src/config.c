#include "config.h"

config_pair_t *config = NULL;

bool config_load(const char *config_file)
{
    FILE *fd;
    char *buffer, *p, *line, *tmp;
    size_t config_size;

    fd = fopen(config_file, "r");
    if(!fd) {
        ERROR("Unable to read configuration file: %s\n", config_file);
        return false;
    }

    config_size = fdsize(fd);
    DEBUG("configuration file size: %d bytes.\n", config_size);

    buffer = malloc(config_size + 1);
    if(!buffer) {
        ERROR("unable to allocate memory for config file!\n");
        return false;
    }

    if(fread(buffer, sizeof(char), config_size, fd) !=
        config_size && ferror(fd) != 0) {
        ERROR("fread error occured!\n");
        FREE(buffer);
        return false;
    }
    buffer[config_size-1] = '\0';

    char *key, *value;

    for(p = line = buffer; p < buffer+config_size; p++) {
        if(*p == '\n') {
            *p++ = '\0';

            if((tmp = strchr(line, '#'))) {
                *tmp = '\0';
            }
            if((tmp = strchr(line, '='))) {
                *tmp = '\0';
                key = trim_string(line);
                value = trim_string(tmp + 1);

                config_pair_t *pair = malloc(sizeof(config_pair_t));
                if(!pair) {
                    ERROR("unable to allocate memory for config pair\n");
                    continue;
                }

                pair->key = copy_string(key);


                /* string value */
                if(strchr(value, '"')) {
                    value++; 
                    *(value + strlen(value) - 1) = '\0';
                    pair->value = new_config_value_string(value); 
                    //    copy_string(value);
                
                }
                else { /* integer value */
                    //pair->value.integer = copy_integer();
                    pair->value = new_config_value_integer(atoi(value)); 
                }

                new_config_pair(&config, pair);

                DEBUG("new config pair '%s':'%s'\n", key, value);
            }

            line = p;
        }
    }

    fclose(fd);
    FREE(buffer);
    return true;
} /* end config_load */

void config_unload()
{
    DEBUG("unload configuration\n");
    free_config_pair(&config);
}

void new_config_pair(config_pair_t **config, config_pair_t *pair)
{
    if(*config == NULL) {
        pair->next = NULL;
        *config = pair;
    }
    else {
        new_config_pair(&((*config)->next), pair);
    }
}

void free_config_pair(config_pair_t **config)
{
    if(!*config) {
        return;
    }
    if((*config)->next) {
        free_config_pair(&((*config)->next));
    }
    FREE((*config)->key);

    FREE((*config)->value->string); /* free the contents of the value */
    FREE((*config)->value); /* free the pointer to the value union */
    FREE(*config);
}

config_value_t *new_config_value_string(char *string)
{
    config_value_t *value = malloc(sizeof(config_value_t));
    if(!value) {
        ERROR("unable to allocate memory for config value!");
        return NULL;
    }
    value->string = copy_string(string);
    return value;
}

config_value_t *new_config_value_integer(int integer)
{
    config_value_t *value = malloc(sizeof(config_value_t));
    if(!value) {
        ERROR("unable to allocate memory for config value!");
        return NULL;
    }
    value->integer = copy_integer(integer);
    return value;
}

config_value_t *search_config(config_pair_t *config, const char *key)
{
    if(!config) {
        return NULL;
    }
    else if(strcmp(config->key, key) == 0) {
        return config->value;
    }
    else {
        return search_config(config->next, key);
    }
}

char *config_string(const char *key)
{
    config_value_t *value;
    if((value = search_config(config, key))) {
        return value->string;
    }
    return NULL;
}

int *config_integer(const char *key)
{
    config_value_t *value;
    if((value = search_config(config, key))) {
        return value->integer;
    }
    return NULL;
}


