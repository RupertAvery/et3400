#ifndef LABEL_MANAGER_H
#define LABEL_MANAGER_H

#include "../common/common_defs.h"
#include "label.h"
#include <vector>

class LabelManager
{
public:
    LabelManager();
    ~LabelManager();
    std::vector<Label> *getLabels();
    void addLabel(Label label);
    void addLabels(std::vector<Label> *labels);
    void removeLabel(Label *label);
    std::vector<Label> getLabels(uint32_t start, uint32_t end);
    void clearRamLabels();
    void clearLabels();
    void clearLabels(uint32_t start, uint32_t end);
    bool getIsDirty();
    bool isValid(offs_t start, offs_t end);
    bool hasCollision(Label *label, offs_t start, offs_t end);

private:
    std::vector<Label> *_labels;
    bool _isDirty;
};

#endif // LABEL_MANAGER_H
