
#include "SVEApp.h"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
	SVEApp app{};

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}