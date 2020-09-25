#include "logger.h"

int main() {

	infof("This is an info message: %s\n", "INFO");
	warnf("This is a warning message: %s\n", "WARNING");
	errorf("This is an error message: %s\n", "ERROR");
	panicf("This is a panic message: %s\n", "PANIC");
    return 0;
}
