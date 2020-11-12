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
	lines->push_back(DisassemblyLine{ address, DisassemblyType::Assembly, opcodes, QString(result.instruction), result.operand, map });
	ptr += result.byteLength;
	address += result.byteLength;
}

void DisassemblyBuilder::build(std::vector<DisassemblyLine> *lines, offs_t start, offs_t end, uint8_t* memory, std::vector<Map>* maps)
{
	lines->clear();
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
				lines->push_back(DisassemblyLine{ address, DisassemblyType::Comment, QString("; %1").arg(map->comment), NULL, NULL,  &(*map) });
				while (address <= map->end)
				{
					offs_t save_address = address;
					// QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");
					i = 0;
					for (; address <= end && address <= map->end && i < 8; i++)
					{
						// data = data.arg(memory[ptr], 2, 16, QChar('0')).toUpper();
						address++;
						ptr++;
					}
					// for (; i < 8; i++)
					// {
					// 	data = data.arg(" ");
					// }
					lines->push_back(DisassemblyLine{ save_address, DisassemblyType::Data, NULL, NULL, NULL, &(*map), i });
					line_count++;
				}

				// if (line_count > 1)
				// {
				//     lines->push_back(DisassemblyLine{address - 1, DisassemblyType::Comment, QString("; END // %1").arg(map->comment), NULL, NULL});
				// }

				break;
			case map_type::ASSEMBLY:
				parentId = id;
				i = 0;
				line_count = 0;
				lines->push_back(DisassemblyLine{ address, DisassemblyType::Assembly, QString("; %1").arg(map->comment), NULL, NULL, &(*map) });
				while (address < map->end) {
					disassemble(lines, memory, ptr, address, id, parentId, &(*map));
				}
				break;
			case map_type::COMMENT:
				lines->push_back(DisassemblyLine{ address, DisassemblyType::Comment, QString("; %1").arg(map->comment), NULL, NULL, &(*map) });
				disassemble(lines, memory, ptr, address, id, parentId, &(*map));
				break;
			}

			map++;
			if (map == maps->end())
			{
				hasMaps = false;
			}
		}
		else
		{
			disassemble(lines, memory, ptr, address, id, -1, NULL);
		}
	}
}
