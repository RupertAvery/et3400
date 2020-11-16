#ifndef GOTO_H
#define GOTO_H

#include "../common/common_defs.h"
#include "../util/label.h"
#include <vector>
#include <QApplication>
#include <QVariant>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QObject>

class GotoDialog : public QDialog
{
	Q_OBJECT

public:
	GotoDialog();
	void setLabels(std::vector<Label>* labels);
	offs_t getSelectedAddress();

private:
	offs_t _selectedAddress;
	QVBoxLayout* mainLayout;
	QListView* listView;
	QDialogButtonBox* buttonBox;

	void setupUi(QDialog* Dialog);
	void doubleClicked(QModelIndex index);

};


#endif // GOTO_H