#include "str.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

void str_append(str_t *str, char *tail)
{
	uint32_t lo = *str ? strlen(*str):0;
	uint32_t ln = strlen(tail);
	char *buf = calloc(1, lo + ln + 1);
	if (lo) {
		strcpy(buf, *str);
		free(*str);
	}
	strcpy(buf + lo, tail);
	*str = buf;
}

void str_toupper(str_t str)
{
	str_t tmp = str;
	if (0 == str) {
		return;
	}
	while (*tmp != 0) {
		if (islower(*tmp)) {
			*tmp = toupper(*tmp);
		}
		tmp++;
	}
}
