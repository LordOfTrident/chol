#define CLOG_IMPLEMENTATION
#include <clog.h>

int main(void) {
	log_into(stdout); /* on default, clog logs into stderr */
	log_set_flags(LOG_LOC | LOG_TIME);
	/* LOG_LOC includes the location in the source code where the log was called
	   LOG_TIME includes the time at which the log was written */

	LOG_INFO("Hello, world!");
	LOG_WARN("I am a warning");
	LOG_CUSTOM("HELLO", "Hello, i am a custom greeting!");
	LOG_ERROR("Uh-oh, i am an error");
	LOG_FATAL("Oh no, i am a fatal error"); /* LOG_FATAL also exits the program with EXIT_FAILURE */
	LOG_INFO("This will not be printed");

	return 0;
}
