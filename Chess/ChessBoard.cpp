#include "ChessBoard.h"

ChessBoard::ChessBoard()
{
	m_chessBoard.resize(8);
	for (int i = 0; i < 8; i++)
		m_chessBoard[i].resize(8);
}

void ChessBoard::DrawBoard()
{
	float width = 2.25f / 8;
	float heigth = 2.25f / 8;
	int counter = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			float xRation, yRation;
			m_chessBoard[i][j] = VulkanEngine::Get().CreateObject();
			xRation = width / m_chessBoard[i][j]->GetSizeX();
			yRation = heigth / m_chessBoard[i][j]->GetSizeY();
			m_chessBoard[i][j]->SetSize(xRation, yRation);
			m_chessBoard[i][j]->SetPosition(float(i * width), float(j * heigth));
			Color c{};
			if (counter % 2)
			{
				c = Appricot;
			}
			else
			{
				c = Brown;
			}
			counter++;
			m_chessBoard[i][j]->SetColor(c);
		}
		counter++;
	}
}
