#include "label_manager.h"
#include "log.h"

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

bool LabelManager::isValid(offs_t start, offs_t end)
{
	return end >= start;
}

bool LabelManager::hasCollision(Label *label, offs_t start, offs_t end)
{
	std::vector<Label>::iterator current = _labels->begin();

	while (current != _labels->end())
	{
		if (start <= current->end && end >= current->start)
		{
			if (label == nullptr)
			{
				return true;
			}
			else
			{
				if (&(*current) != label)
				{
					return true;
				}
			}
		}
		current++;
	}

	return false;
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

void LabelManager::clearLabels()
{
	_labels->clear();
	_isDirty = true;
}

void LabelManager::clearLabels(uint32_t start, uint32_t end)
{
	auto it = _labels->begin();
	while (it != _labels->end())
	{
		if (it->start >= start && it->start <= end)
			it = _labels->erase(it);
		else
			++it;
	}
	_isDirty = true;
}

void LabelManager::clearRamLabels()
{
	std::vector<Label>::iterator current = _labels->begin();

	while (current != _labels->end())
	{
		if (current->start < 0x07FF)
			current = _labels->erase(current);
		else
			++current;
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

std::vector<Label> LabelManager::getLabels(uint32_t start, uint32_t end)
{
	std::vector<Label> filteredLabels;
	std::vector<Label>::iterator current = _labels->begin();

	while (current != _labels->end())
	{
		// LOG_DEBUG << "Label: " << (*current).comment << "type:" << (*current).type << "start:" << (*current).start << "end:" << (*current).end;
		if ((*current).start >= start && (*current).end <= end)
		{
			filteredLabels.push_back((*current));
		}
		current++;
	}

	_isDirty = false;

	return filteredLabels;
}

bool LabelManager::getIsDirty()
{
	return _isDirty;
}
