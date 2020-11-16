#include "label_manager.h"

LabelManager::LabelManager()
{
	_labels = new std::vector<Label>;
}

LabelManager::~LabelManager()
{
	delete _labels;
}

std::vector<Label> *LabelManager::getLabels()
{
	return _labels;
}

void LabelManager::addLabels(std::vector<Label> *labels)
{
	std::vector<Label>::iterator current = labels->begin();

	while (current != labels->end())
	{
		addLabel(*current);
		current++;
	}
	_isDirty = true;
}

void LabelManager::addLabel(Label label)
{
	std::vector<Label>::iterator current = _labels->begin();

	bool bInserted = false;
	while (current != _labels->end())
	{
		if (current->start == label.start)
		{
			current = _labels->erase(current);
			_labels->insert(current, label);
			break;
		}
		else if (current->start > label.start)
		{
			_labels->insert(current, label);
			bInserted = true;
			break;
		}
		current++;
	}
	if (!bInserted)
	{
		_labels->push_back(label);
	}

	_isDirty = true;
}

void LabelManager::clearRamLabels()
{
	std::vector<Label>::iterator current = _labels->begin();

	while (current != _labels->end())
	{
		if (current->start < 0x0400)
		{
			current = _labels->erase(current);
		}
		current++;
	}

	_isDirty = true;
}

void LabelManager::removeLabel(Label *label)
{
	std::vector<Label>::iterator current = _labels->begin();

	while (current != _labels->end())
	{
		if (&(*current) == label)
		{
			_labels->erase(current);
			break;
		}
		current++;
	}

	_isDirty = true;
}

void LabelManager::loadLabels(QString path, bool &success)
{
	std::vector<Label> *labels = LabelReader::Read(path, success);
	if (success)
	{
		addLabels(labels);
	}
	delete labels;

	_isDirty = true;
}

void LabelManager::saveLabels(QString path, uint32_t start, uint32_t end, bool &success)
{
	std::vector<Label> filteredLabels;
	std::vector<Label>::iterator current = _labels->begin();

	while (current != _labels->end())
	{
		if ((*current).start >= start && (*current).end <= end)
		{
			filteredLabels.push_back((*current));
			break;
		}
		current++;
	}

	LabelReader::Write(path, &filteredLabels, success);

	_isDirty = false;
}

bool LabelManager::getIsDirty()
{
	return _isDirty;
}

