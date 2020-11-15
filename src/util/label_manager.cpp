#include "label_manager.h"

LabelManager::LabelManager()
{
	labels = new std::vector<Label>;
}

LabelManager::~LabelManager()
{
	delete labels;
}

std::vector<Label>* LabelManager::getLabels()
{
	return labels;
}

void LabelManager::addLabels(std::vector<Label>* labels)
{
	std::vector<Label>::iterator current = labels->begin();

	while (current != labels->end())
	{
		addLabel(*current);
		current++;
	}
}

void LabelManager::addLabel(Label label)
{
	std::vector<Label>::iterator current = labels->begin();

	bool bInserted = false;
	while (current != labels->end())
	{
		if (current->start == label.start)
		{
			current = labels->erase(current);
			labels->insert(current, label);
			break;
		}
		else if (current->start > label.start)
		{
			labels->insert(current, label);
			bInserted = true;
			break;
		}
		current++;
	}
	if (!bInserted)
	{
		labels->push_back(label);
	}
}

void LabelManager::clearRamLabels()
{
	std::vector<Label>::iterator current = labels->begin();

	while (current != labels->end())
	{
		if (current->start < 0x0400)
		{
			current = labels->erase(current);
		}
		current++;
	}
}

void LabelManager::removeLabel(Label* label)
{
	std::vector<Label>::iterator current = labels->begin();

	while (current != labels->end())
	{
		if (&(*current) == label)
		{
			labels->erase(current);
			break;
		}
		current++;
	}
}

void LabelManager::loadLabels(QString path, bool& success)
{
	std::vector<Label>* newlabels = LabelReader::Read(path, success);
	if (success)
	{
		addLabels(newlabels);
	}
	delete newlabels;
}


void LabelManager::saveLabels(QString path, uint32_t start, uint32_t end, bool& success)
{
	std::vector<Label> filteredLabels;
	std::vector<Label>::iterator current = labels->begin();

	while (current != labels->end())
	{
		if ((*current).start >= start && (*current).end <= end)
		{
			filteredLabels.push_back((*current));
			break;
		}
		current++;
	}

	LabelReader::Write(path, &filteredLabels, success);
}
