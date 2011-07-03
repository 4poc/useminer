#include "file.h"

void newsgroup_insert(
        struct s_newsgroup **newsgroup_list,
        char *name)
{
    while(*newsgroup_list) { /* skip to the end */
        newsgroup_list = &((*newsgroup_list)->next);
    }

    /* create new newsgroup */
    *newsgroup_list = malloc(sizeof(s_newsgroup));
    if(!*newsgroup_list) {
        ERROR("unable to allocate memory for s_newsgroup\n");
        return;
    }
    (*newsgroup_list)->name = copy_string(name);
    (*newsgroup_list)->next = NULL;
}

void newsgroup_free(struct s_newsgroup *newsgroup_list)
{
    struct s_newsgroup *newsgroup;
    while(newsgroup_list) {
        newsgroup = newsgroup_list;
        newsgroup_list = newsgroup_list->next;
        FREE(newsgroup->name);
        FREE(newsgroup);
    }
}

bool newsgroup_search(struct s_newsgroup *newsgroups, char *name)
{
}

