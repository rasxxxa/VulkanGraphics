#include "../VulkanGraphics/VulkanEngine.h"
import Helper;
class ChessBoard
{
	std::vector<std::vector<DrawObject>> m_chessBoard;
public:
	ChessBoard();
	void DrawBoard();
};