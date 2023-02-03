#pragma once
#include <array>
#include <vector>
#include <deque>
#include <memory>
#include "Chess.h"
#include "Player.h"
#include "PlayerWhite.h"
#include "PlayerBlack.h"
#include <iostream>
#include "../VulkanGraphics/VulkanEngine.h"

using namespace constants;
class Table
{
private:
	std::deque<std::unique_ptr<Player>> m_players;
	static std::unique_ptr<Table> m_instance;
	std::array<std::array<std::weak_ptr<Piece>, TableSize>, TableSize> m_WholeTable;
public:
	Table();
	virtual ~Table() {};
	static std::array<std::array<bool, TableSize>, TableSize> elements;
	void CreateTable();
	static Table* GetInstance();
	friend std::ostream& operator<<(std::ostream& os, const Table& table);
	bool DoMove();
};

