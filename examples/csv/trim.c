#include <stdio.h> /* printf */

#define CSV_IMPLEMENTATION
#include <csv.h>

int main(void) {
	sv_t sv = sv_cstr("\r \t   Hello, world!\t    ");

	sv = sv_trim(sv, SV_WHITESPACES);
	printf("'" SV_FMT "'\n", SV_ARG(sv));

	return 0;
}
