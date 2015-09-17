#include <stdlib.h>

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
