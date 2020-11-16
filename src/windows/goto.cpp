#include "goto.h"

GotoDialog::GotoDialog() : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
	setupUi(this);
	_selectedAddress = -1;

	this->setStyleSheet("QLabel { font-size:12px; height: 20px }");

	setFixedSize(QSize(350, 250));
	setWindowTitle("Goto Label");
}

void GotoDialog::setupUi(QDialog* Dialog)
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(10);
	mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	listView = new QListView(this);

	mainLayout->addWidget(listView);

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	mainLayout->addWidget(buttonBox);

	mainLayout->setStretch(0, 5);
	mainLayout->setStretch(1, 1);
	mainLayout->setMargin(20);

	setLayout(mainLayout);

	connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));
	connect(listView, &QListView::doubleClicked, this, &GotoDialog::doubleClicked);
	

	QMetaObject::connectSlotsByName(Dialog);

} // setupUi

void GotoDialog::setLabels(std::vector<Label>* labels)
{

	QStandardItemModel* model = new QStandardItemModel;
	QStandardItem* parentItem = model->invisibleRootItem();

	for (std::vector<Label>::iterator it = labels->begin(); it != labels->end(); ++it)
	{
		QString address = QString("$%1").arg((*it).start, 4, 16, QChar('0')).toUpper();

		QStandardItem* item = new QStandardItem(QString("%1 - %2").arg(address).arg((*it).comment));
		item->setData((*it).start, Qt::UserRole);
		item->setEditable(false);
		parentItem->appendRow(item);
	}

	
	listView->setModel(model);
	listView->setFocus();
	listView->setCurrentIndex(listView->model()->index(0, 0));
}

void GotoDialog::doubleClicked(QModelIndex index)
{
	accept();
}

offs_t GotoDialog::getSelectedAddress()
{
	QModelIndexList list = listView->selectionModel()->selectedIndexes();
	return (uint32_t)list.at(0).data(Qt::UserRole).toInt();
}