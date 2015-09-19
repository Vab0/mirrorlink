#include <stdlib.h>
#include <string.h>

void safe_append(void **ptr, int size)
{
	void *new_ptr = realloc(*ptr, size);
	if (!new_ptr) {
		free(*ptr);
		*ptr = 0;
		return;
	}
	if (!size) {
		*ptr = 0;
		return;
	}
	*ptr = new_ptr;
}

void safe_str_append(char **str, const char *con)
{
	int len = strlen(con);
	int l = *str ? strlen(*str):0;
	if (len + l > 0) {
		safe_append(str, len + l);
		strcpy(*str + l, con);
	}
}

