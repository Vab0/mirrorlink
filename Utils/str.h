#ifndef _STR_H
#define _STR_H

typedef char * str_t;

extern void str_append(str_t *str, char *tail);

extern void str_toupper(str_t str);

#endif
