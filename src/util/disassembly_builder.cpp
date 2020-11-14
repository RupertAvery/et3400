#include "disassembly_builder.h"


void DisassemblyBuilder::disassemble(std::vector<DisassemblyLine>* lines, uint8_t* memory, int& ptr, offs_t &address, Label* label)
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
	lines->push_back(DisassemblyLine{ address, DisassemblyType::Assembly, opcodes, QString(result.instruction), result.operand, label });
	ptr += result.byteLength;
	address += result.byteLength;
}

void DisassemblyBuilder::build(std::vector<DisassemblyLine> *lines, offs_t start, offs_t end, uint8_t* memory, std::vector<Label>* labels)
{
	lines->clear();
	offs_t address = start;
	std::vector<Label>::iterator label = labels->begin();
	bool hasLabels = labels->size() > 0;
	//QChar filler = QLatin1Char('0');
	int ptr = 0;
	int i = 0;
	int line_count = 0;

	while (address <= end)
	{
		if (hasLabels && address > label->start)
		{
			while (address > label->start)
			{
				label++;
			}
		}

		if (hasLabels && address == label->start)
		{
			switch (label->type)
			{
			case LabelType::DATA:
				i = 0;
				line_count = 0;
				lines->push_back(DisassemblyLine{ address, DisassemblyType::Comment, QString("; %1").arg(label->comment), NULL, NULL,  &(*label) });
				while (address <= label->end)
				{
					offs_t save_address = address;
					// QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");
					i = 0;
					for (; address <= end && address <= label->end && i < 8; i++)
					{
						// data = data.arg(memory[ptr], 2, 16, QChar('0')).toUpper();
						address++;
						ptr++;
					}
					// for (; i < 8; i++)
					// {
					// 	data = data.arg(" ");
					// }
					lines->push_back(DisassemblyLine{ save_address, DisassemblyType::Data, NULL, NULL, NULL, &(*label), i });
					line_count++;
				}

				// if (line_count > 1)
				// {
				//     lines->push_back(DisassemblyLine{address - 1, DisassemblyType::Comment, QString("; END // %1").arg(label->comment), NULL, NULL});
				// }

				break;
			case LabelType::ASSEMBLY:
				line_count = 0;
				lines->push_back(DisassemblyLine{ address, DisassemblyType::Assembly, QString("; %1").arg(label->comment), NULL, NULL, &(*label) });
				while (address < label->end) {
					disassemble(lines, memory, ptr, address, &(*label));
				}
				break;
			case LabelType::COMMENT:
				lines->push_back(DisassemblyLine{ address, DisassemblyType::Comment, QString("; %1").arg(label->comment), NULL, NULL, &(*label) });
				disassemble(lines, memory, ptr, address, &(*label));
				break;
			}

			label++;
			if (label == labels->end())
			{
				hasLabels = false;
			}
		}
		else
		{
			disassemble(lines, memory, ptr, address, NULL);
		}
	}
}
