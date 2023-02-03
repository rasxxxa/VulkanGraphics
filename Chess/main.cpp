#include <iostream>
#include "../VulkanGraphics/VulkanEngine.h"
#include "ChessBoard.h"

int main(int argc, char** argv)
{
	ChessBoard board;
	VulkanEngine::Get().Init();
	board.DrawBoard();
	VulkanEngine::Get().Run();
	VulkanEngine::Get().CleanUp();
	return 0;
}