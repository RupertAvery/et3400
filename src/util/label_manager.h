#ifndef LABEL_MANAGER_H
#define LABEL_MANAGER_H

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
    void loadLabels(QString path, bool &success);
    void saveLabels(QString path, uint32_t start, uint32_t end, bool &success);

private:
    std::vector<Label> *labels;
};

#endif // LABEL_MANAGER_H
