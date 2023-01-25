// VulkanGraphics.cpp : Defines the entry point for the application.
//

#include "VulkanGraphics.h"
#include "VulkanEngine.h"
using namespace std;

int main(int argc, char** argv)
{
	VulkanEngine engine;
	engine.Init();
	engine.Run();
	engine.CleanUp();
	return 0;
}
