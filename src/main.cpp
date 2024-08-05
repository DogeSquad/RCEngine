#include "rcengine.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
	try {
		rce::RCEngine rcEngine{};
		rcEngine.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}