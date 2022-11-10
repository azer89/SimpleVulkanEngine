#ifndef SVE_APP_H
#define SVE_APP_H

#include "SVEWindow.h"

class SVEApp
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	void Run();

private:
	SVEWindow sveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
};

#endif