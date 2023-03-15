#include <stdio.h> /* stdout, printf */

#define CHOL_COLORER_IMPLEMENTATION
#include <colorer.h>

int main(void) {
	color_init(); /* initialize the colorer library */

	color_fg(stdout, COLOR_YELLOW);
	color_bg(stdout, COLOR_BLUE);
	printf("Hello, world!");

	color_reset(stdout); /* reset the color back to default */
	printf("\nBye.\n");

	return 0;
}
