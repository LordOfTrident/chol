#include <stdio.h> /* printf */

#define CHOL_SV_IMPLEMENTATION
#include <sv.h>

int main(void) {
	sv_t sv = sv_cstr("\r \t   Hello, world!\t    ");

	sv = sv_trim(sv, SV_WHITESPACES);
	printf("'" SV_FMT "'\n", SV_ARG(sv));

	return 0;
}
