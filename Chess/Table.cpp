#include "Table.h"

std::array<std::array<bool, TableSize>, TableSize> Table::elements = { {false} };

Table::Table()
{
}

void Table::CreateTable()
{
	if (!m_instance)
	{
		std::cout << "Table Not Created";
		return;
	}

	float width = 2.0f / 8.0f;
	float height = 2.0f / 8.0f;
	int counter = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			float xRation, yRation;
			auto field = VulkanEngine::Get().CreateObject();
			xRation = width / 0.2f;
			yRation = height / 0.2f;
			field->SetSize(xRation, yRation);
			field->SetPosition(float(i * width), float(j * height));
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
			field->SetColor(c);
		}
		counter++;
	}

	m_players.push_back(std::make_unique<PlayerWhite>());
	m_players.push_back(std::make_unique<PlayerBlack>());

	for (const auto& player : m_players)
	{
		const auto& pieces = player->GetPieces();
		for (const auto& piece : pieces)
		{
			elements[piece->GetPosition().x][piece->GetPosition().y] = true;
			m_WholeTable[piece->GetPosition().x][piece->GetPosition().y] = piece;
		}
	}
}

Table* Table::GetInstance()
{
	if (!m_instance)
	{
		m_instance = std::make_unique<Table>();
	}

	return m_instance.get();
}

bool Table::DoMove()
{
	system("CLS");
	std::cout << *this;
	return true;
}

std::ostream& operator<<(std::ostream& os, const Table& table)
{
	os << "------ TABLE -----" << std::endl << std::endl;
	for (uchar fieldX = 0; fieldX < TableSize; fieldX++)
	{
		for (uchar fieldY = 0; fieldY < TableSize; fieldY++)
		{
			if (table.m_WholeTable[fieldX][fieldY].lock())
			{
				os << " " << (*table.m_WholeTable[fieldX][fieldY].lock()) << " ";
			}
			else
			{
				os << " ";
			}
			
		}
		os << std::endl;
	}
	os << std::endl;
	return os;
}

std::unique_ptr<Table> Table::m_instance = nullptr;