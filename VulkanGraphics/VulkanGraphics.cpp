// VulkanGraphics.cpp : Defines the entry point for the application.
//

#include "VulkanGraphics.h"
#include "VulkanEngine.h"
using namespace std;

int main(int argc, char** argv)
{
	VulkanEngine::Get().Init();
	VulkanEngine::Get().Run();
	VulkanEngine::Get().CleanUp();
	return 0;
}
