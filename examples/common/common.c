#include <stdio.h> /* printf */

#define CHOL_COMMON_IMPLEMENTATION
#include <common.h>

int main(int argc, char **argv) {
	UNUSED(argv);

	if (argc > 1)
		TODO("Parse command line args");

	const char *text[] = {
		"Hello, there!",
		"This is a very nice",
		"Array of lines",
	};

	printf("Testing " TO_STR(FOREACH_IN_ARRAY) "\n");

	FOREACH_IN_ARRAY(text, const char*, ARRAY_SIZE(text), line, {
		printf("%s\n", *line);
	});

	printf("\n");

	int   *numbers;
	size_t count = 5;
	if (SALLOC(numbers, int, count) == NULL)
		FATAL_FUNC_FAIL("SALLOC");

	printf("numbers = {");
	int num = 0;
	FOREACH_IN_ARRAY(numbers, int, count, number, {
		*number = num;
		num = (num + 1) * 2;

		if (number != numbers)
			printf(", ");

		printf("%i", *number);
	});
	printf("}\n");

	if (SFREE(numbers) != 0)
		UNREACHABLE("'numbers' is NULL");

	return 0;
}
