#include "disassembly_builder.h"


void DisassemblyBuilder::disassemble(std::vector<DisassemblyLine>* lines, uint8_t* memory, int& ptr, offs_t &address, int& id, int parentId, Map* map)
{
	DasmResult result = Disassembler::disassemble(&memory[ptr], address);
	QString opcodes = QString("%1 %2 %3");
	int i = 0;
	for (; i < result.byteLength; i++)
	{
		opcodes = opcodes.arg(memory[ptr + i], 2, 16, QChar('0')).toUpper();
	}
	for (; i < 3; i++)
	{
		opcodes = opcodes.arg("  ");
	}
	lines->push_back(DisassemblyLine{ id++, address, DisassemblyType::Assembly, opcodes, QString(result.instruction), result.operand, false, false, false, parentId, map });
	ptr += result.byteLength;
	address += result.byteLength;
}

std::vector<DisassemblyLine>* DisassemblyBuilder::build(offs_t start, offs_t end, uint8_t* memory, std::vector<Map>* maps)
{
	std::vector<DisassemblyLine>* lines = new std::vector<DisassemblyLine>;
	offs_t address = start;
	std::vector<Map>::iterator map = maps->begin();
	bool hasMaps = maps->size() > 0;
	//QChar filler = QLatin1Char('0');
	int ptr = 0;
	int id = 0;
	int parentId = -1;
	int i = 0;
	int line_count = 0;

	while (address <= end)
	{
		if (hasMaps && address > map->start)
		{
			while (address > map->start)
			{
				map++;
			}
		}

		if (hasMaps && address == map->start)
		{
			switch (map->type)
			{
			case map_type::DATA:
				parentId = id;
				i = 0;
				line_count = 0;
				lines->push_back(DisassemblyLine{ id++, address, DisassemblyType::Comment, QString("; %1").arg(map->comment), NULL, NULL, false, false, false, -1, &(*map) });
				while (address < map->end)
				{
					offs_t save_address = address;
					QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");
					i = 0;
					for (; address <= end && address <= map->end && i < 8; i++)
					{
						data = data.arg(memory[ptr], 2, 16, QChar('0')).toUpper();
						address++;
						ptr++;
					}
					for (; i < 8; i++)
					{
						data = data.arg(" ");
					}
					lines->push_back(DisassemblyLine{ id++, save_address, DisassemblyType::Data, data, NULL, NULL, false, false, false, parentId, &(*map) });
					line_count++;
				}

				// if (line_count > 1)
				// {
				//     lines->push_back(DisassemblyLine{address - 1, DisassemblyType::Comment, QString("; END // %1").arg(map->comment), NULL, NULL});
				// }

				map++;
				if (map == maps->end())
				{
					hasMaps = false;
				}
				break;
			case map_type::ASSEMBLY:
				parentId = id;
				i = 0;
				line_count = 0;
				lines->push_back(DisassemblyLine{ id++, address, DisassemblyType::Assembly, QString("; %1").arg(map->comment), NULL, NULL, false, false, false, -1, &(*map) });
				while (address < map->end) {
					disassemble(lines, memory, ptr, address, id, parentId, &(*map));
				}
				break;
			case map_type::COMMENT:
				lines->push_back(DisassemblyLine{ id++, address, DisassemblyType::Comment, QString("; %1").arg(map->comment), NULL, NULL, false, false, false, -1, &(*map) });
				break;
			}
		}
		else
		{
			disassemble(lines, memory, ptr, address, id, -1, NULL);
			//DasmResult result = Disassembler::disassemble(&memory[ptr], address);
			//QString opcodes = QString("%1 %2 %3");
			//int i = 0;
			//for (; i < result.byteLength; i++)
			//{
			//	opcodes = opcodes.arg(memory[ptr + i], 2, 16, QChar('0')).toUpper();
			//}
			//for (; i < 3; i++)
			//{
			//	opcodes = opcodes.arg("  ");
			//}
			//lines->push_back(DisassemblyLine{ id++, address, DisassemblyType::Assembly, opcodes, QString(result.instruction), result.operand, false, false, false, -1, NULL });
			//ptr += result.byteLength;
			//address += result.byteLength;
		}
	}
	return lines;
}
