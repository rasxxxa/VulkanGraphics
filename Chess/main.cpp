#include <iostream>
#include "../VulkanGraphics/VulkanEngine.h"
#include "Table.h"

int main(int argc, char** argv)
{
	VulkanEngine::Get().Init();
	Table::GetInstance()->CreateTable();
	VulkanEngine::Get().Run();
	VulkanEngine::Get().CleanUp();
	return 0;
}