#include "DominusEngine.h"
#include <stdexcept>
#include <iostream>

int main() {
	Dominus de;

	try {
		de.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}