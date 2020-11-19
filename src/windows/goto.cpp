#include "goto.h"
#include "../common/util.h"

GotoDialog::GotoDialog() : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
	setupUi(this);
	_selectedAddress = -1;

	this->setStyleSheet("QLabel { font-size:12px; height: 20px }");

	setFixedSize(QSize(350, 250));
	setWindowTitle("Goto Address");
}

void GotoDialog::setupUi(QDialog *Dialog)
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(10);
	mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	address = new QLineEdit(this);
	mainLayout->addWidget(address);

	listView = new QListView(this);

	mainLayout->addWidget(listView);

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	mainLayout->addWidget(buttonBox);

	mainLayout->setStretch(0, 1);
	mainLayout->setStretch(1, 5);
	mainLayout->setStretch(2, 1);
	mainLayout->setMargin(20);

	setLayout(mainLayout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &GotoDialog::validate);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &GotoDialog::reject);

	connect(listView, &QListView::doubleClicked, this, &GotoDialog::labelDoubleClicked);
	connect(listView, &QListView::clicked, this, &GotoDialog::labelSelected);
	connect(address, &QLineEdit::returnPressed, this, &GotoDialog::addressEntered);

	QMetaObject::connectSlotsByName(Dialog);

} // setupUi

void GotoDialog::setLabels(std::vector<Label> *labels)
{

	QStandardItemModel *model = new QStandardItemModel;
	QStandardItem *parentItem = model->invisibleRootItem();

	for (std::vector<Label>::iterator it = labels->begin(); it != labels->end(); ++it)
	{
		QString address = QString("$%1").arg((*it).start, 4, 16, QChar('0')).toUpper();

		QStandardItem *item = new QStandardItem(QString("%1 - %2").arg(address).arg((*it).comment));
		item->setData((*it).start, Qt::UserRole);
		item->setEditable(false);
		parentItem->appendRow(item);
	}

	address->setText("$");
	listView->setModel(model);
	address->setFocus();
	//listView->setCurrentIndex(listView->model()->index(0, 0));
}

void GotoDialog::labelDoubleClicked(const QModelIndex &index)
{
	//QModelIndexList list = listView->selectionModel()->selectedIndexes();
	//list->at(0)
	_selectedAddress = (uint32_t)index.data(Qt::UserRole).toInt();
	address->setText(QString("$%1").arg(_selectedAddress, 4, 16, QChar('0')));
	validate();
}

void GotoDialog::labelSelected(const QModelIndex &index)
{
	_selectedAddress = (uint32_t)index.data(Qt::UserRole).toInt();
	address->setText(QString("$%1").arg(_selectedAddress, 4, 16, QChar('0')));
}

void GotoDialog::validate()
{
    bool ok;
    _selectedAddress = toInt(address, ok);

    if (ok)
    {
        accept();
    }
}

void GotoDialog::addressEntered()
{
	bool ok;
	_selectedAddress = (offs_t)toInt(address, ok);
	if (ok)
	{
		accept();
	}
}

offs_t GotoDialog::getSelectedAddress()
{
	return _selectedAddress;
}