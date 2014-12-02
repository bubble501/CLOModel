#include "TriggerStructHelperWidget.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QHeaderView>
TriggerStructHelperWidget::TriggerStructHelperWidget(const QHash<quint32, QSharedPointer<AbstractTrigger> >& AvailableTriggers, QWidget *parent)
	: QWidget(parent)
{
	QLabel* TriggersLabel = new QLabel(this);
	TriggersLabel->setText(tr("Available Triggers"));

	TriggersModel = new QStandardItemModel(this);
	TriggersModel->setColumnCount(3);
	TriggersModel->setHorizontalHeaderLabels(QStringList() << "ID" << "Label" << "Description");
	TriggersModel->setRowCount( 2);
	TriggersModel->setData(TriggersModel->index(0, 0), "T");
	TriggersModel->setData(TriggersModel->index(0, 1), "True");
	TriggersModel->setData(TriggersModel->index(0, 2), "Trigger that Always Passes");
	TriggersModel->setData(TriggersModel->index(1, 0), "F");
	TriggersModel->setData(TriggersModel->index(1, 1), "False");
	TriggersModel->setData(TriggersModel->index(1, 2), "Trigger that Never Passes");
	

	TriggerTable = new QTableView(this);
	TriggerTable->setModel(TriggersModel);
	TriggerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	TriggerTable->setSelectionMode(QAbstractItemView::SingleSelection);
	TriggerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	TriggerTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	TriggerTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	TriggerTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	TriggerTable->verticalHeader()->hide();
	connect(TriggerTable, &QTableView::doubleClicked, this, &TriggerStructHelperWidget::InsertTrigger);

	QLabel* Resultlabel = new QLabel(this);
	Resultlabel->setText(tr("Result", "Resulting trigger Structure from editor"));
	EncriptedTriggers = new QLineEdit(this);
	EncriptedTriggers->setValidator(new QRegExpValidator(QRegExp("(?:\\*|\\+|/|-|T|F|!|\\(|\\))*"), EncriptedTriggers));
	connect(EncriptedTriggers, &QLineEdit::textChanged, this, &TriggerStructHelperWidget::DecriptTriggers);
	DecriptedTriggers = new QLineEdit(this);
	DecriptedTriggers->setReadOnly(true);

	NegateButton = new QPushButton(this);
	NegateButton->setText(tr("Negate"));
	connect(NegateButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::Negate);
	AndButton = new QPushButton(this);
	AndButton->setText(tr("AND"));
	OrButton = new QPushButton(this);
	OrButton->setText(tr("OR"));
	NandButton = new QPushButton(this);
	NandButton->setText(tr("NAND"));
	NorButton = new QPushButton(this);
	NorButton->setText(tr("NOR"));
	connect(AndButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::InsertOperator);
	connect(OrButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::InsertOperator);
	connect(NandButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::InsertOperator);
	connect(NorButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::InsertOperator);
	OpenParButton = new QPushButton(this);
	OpenParButton->setText("(");
	connect(OpenParButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::OpenParenthesis);
	CloseParButton = new QPushButton(this);
	CloseParButton->setText(")");
	connect(CloseParButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::CloseParenthesis);
	DelButton = new QPushButton(this);
	DelButton->setText(tr("DEL","DEL button in calculator"));
	connect(DelButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::Del);
	ClearButton = new QPushButton(this);
	ClearButton->setText(tr("Clear", "AC button in calculator"));
	connect(ClearButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::Clear);

	QGridLayout* Mainlay = new QGridLayout(this);
	Mainlay->addWidget(TriggersLabel, 0, 0);
	Mainlay->addWidget(TriggerTable, 1, 0, 6, 1);
	Mainlay->addWidget(NegateButton, 1, 1, 1, 2);
	Mainlay->addWidget(AndButton, 2, 1);
	Mainlay->addWidget(OrButton, 2, 2);
	Mainlay->addWidget(NandButton, 3, 1);
	Mainlay->addWidget(NorButton, 3, 2);
	Mainlay->addWidget(OpenParButton, 4, 1);
	Mainlay->addWidget(CloseParButton, 4, 2);
	Mainlay->addWidget(DelButton, 5, 1);
	Mainlay->addWidget(ClearButton, 5, 2);
	Mainlay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 6, 1, 1, 2);
	Mainlay->addWidget(Resultlabel, 7, 0, 1, 3);
	Mainlay->addWidget(EncriptedTriggers, 8, 0, 1, 3);
	Mainlay->addWidget(DecriptedTriggers, 9, 0, 1, 3);
	SetAvailableTriggers(AvailableTriggers);
	EnableOperators(false);
}

void TriggerStructHelperWidget::SetAvailableTriggers(const QHash<quint32, QSharedPointer<AbstractTrigger> >& AvailableTriggers) {
	TriggersModel->setRowCount(AvailableTriggers.count()+2);
	QString RegExpString = "(?:\\*|\\+|/|-|T|F";
	int RowCount = 2;
	QRegExp LabelRegExp("Label: .+\n");
	LabelRegExp.setMinimal(true);
	for (auto i = AvailableTriggers.constBegin(); i != AvailableTriggers.constEnd(); ++i, ++RowCount) {
		TriggersModel->setData(TriggersModel->index(RowCount, 0), i.key());
		TriggersModel->setData(TriggersModel->index(RowCount, 1), i.value()->GetTriggerLabel());
		QString TempString = i.value()->ToString(); TempString.replace(LabelRegExp, ""); TempString.replace("\n", " - ");
		TriggersModel->setData(TriggersModel->index(RowCount, 2), TempString);
		TriggersModel->setData(TriggersModel->index(RowCount, 2), TempString,Qt::ToolTipRole);
		RegExpString += '|' + QString::number(i.key());
	}
	TriggersModel->sort(0);
	RegExpString += "|!|\\(|\\))*";
	EncriptedTriggers->setValidator(new QRegExpValidator(QRegExp(RegExpString), EncriptedTriggers));
}

void TriggerStructHelperWidget::InsertOperator() {
	if (!sender()) return;
	if (sender() == AndButton) EncriptedTriggers->insert("*");
	if (sender() == OrButton) EncriptedTriggers->insert("+");
	if (sender() == NandButton) EncriptedTriggers->insert("/");
	if (sender() == NorButton) EncriptedTriggers->insert("-");
}

void TriggerStructHelperWidget::OpenParenthesis() {
	EncriptedTriggers->insert("()");
	EncriptedTriggers->cursorBackward(false, 1);
}

void TriggerStructHelperWidget::CloseParenthesis() {
	if (EncriptedTriggers->cursorPosition() >= EncriptedTriggers->text().size()) EncriptedTriggers->insert(")");
	else
	{
		if (EncriptedTriggers->text().at(EncriptedTriggers->cursorPosition()) == ')')
			EncriptedTriggers->cursorForward(false, 1);
		else
			EncriptedTriggers->insert(")");
	}
}

void TriggerStructHelperWidget::InsertTrigger(const QModelIndex & index) {
	EncriptedTriggers->insert(TriggersModel->index(index.row(), 0).data().toString());
}

void TriggerStructHelperWidget::Del() {
	if (EncriptedTriggers->cursorPosition()==0) return;
	QChar lastChar = EncriptedTriggers->text().at(EncriptedTriggers->cursorPosition() - 1);
	switch (lastChar.toLatin1()) {
	case '+':
	case '-':
	case '*':
	case '/':
	case '!':
	case '(':
	case ')':
		EncriptedTriggers->cursorBackward(true, 1);
		EncriptedTriggers->insert("");
		return;
	default: break;
	}
	do{
		EncriptedTriggers->cursorBackward(true, 1);
		EncriptedTriggers->insert("");
		if (EncriptedTriggers->cursorPosition() == 0 || lastChar == '!') break;
		lastChar = EncriptedTriggers->text().at(EncriptedTriggers->cursorPosition() - 1);
	} while (lastChar.isDigit() || lastChar == '!' || lastChar == 'T' || lastChar == 'F');
}

void TriggerStructHelperWidget::Clear() {
	EncriptedTriggers->setText("");
}

void TriggerStructHelperWidget::EnableOperators(bool a /*= true*/) {
	TriggerTable->setEnabled(!a);
	AndButton->setEnabled(a);
	OrButton->setEnabled(a);
	NandButton->setEnabled(a);
	NorButton->setEnabled(a);
	OpenParButton->setEnabled(!a);
	CloseParButton->setEnabled(a && EncriptedTriggers->text().count('(')>0);
}

void TriggerStructHelperWidget::Negate() {
	int CountBackPos = 0;
	if (EncriptedTriggers->cursorPosition() == 0) return EncriptedTriggers->setText('!' + EncriptedTriggers->text());
	QChar lastChar = EncriptedTriggers->text().at(EncriptedTriggers->cursorPosition() - 1);
	while ( lastChar.isDigit() || lastChar == 'T' || lastChar == 'F') {
		EncriptedTriggers->cursorBackward(false, 1);
		++CountBackPos;
		if (EncriptedTriggers->cursorPosition() == 0) {
			EncriptedTriggers->setText('!' + EncriptedTriggers->text());
			EncriptedTriggers->cursorForward(false, CountBackPos);
			return;
		}
		lastChar = EncriptedTriggers->text().at(EncriptedTriggers->cursorPosition() - 1);
	}
	if (lastChar == '!') {
		EncriptedTriggers->cursorBackward(true, 1);
		EncriptedTriggers->insert("");
	}
	else EncriptedTriggers->insert("!");
	EncriptedTriggers->cursorForward(false, CountBackPos);
}

void TriggerStructHelperWidget::DecriptTriggers(QString encr) {
	if (encr.isEmpty()) {
		DecriptedTriggers->setText(QString());
		DecriptedTriggers->setStyleSheet(QString());
		EnableOperators(false);
	}
	else {
		QChar lastChar = encr.at(encr.size() - 1);
		EnableOperators(lastChar.isDigit() || lastChar == 'T' || lastChar == 'F');
		if (InfixToPostfix(encr).isEmpty()) {
			DecriptedTriggers->setStyleSheet("color: red;");
			DecriptedTriggers->setText("Invalid Trigger Structure");
		}
		else {
			for (int i = 0; i < TriggersModel->rowCount(); ++i) {
				encr.replace(TriggersModel->index(i, 0).data().toString(), TriggersModel->index(i, 1).data().toString());
			}
			encr.replace("*", " AND ");
			encr.replace("+", " OR ");
			encr.replace("-", " NOR ");
			encr.replace("/", " NAND ");
			encr.replace("!", "NOT ");
			DecriptedTriggers->setText(encr);
			DecriptedTriggers->setStyleSheet(QString());
		}
		
	}
	EncriptedTriggers->activateWindow();
	EncriptedTriggers->deselect();
}

void TriggerStructHelperWidget::SetCurrentStructure(const QString& a) {
	EncriptedTriggers->setText(NormaliseTriggerStructure(a));
}

QString TriggerStructHelperWidget::GetResult() const {
	return EncriptedTriggers->text();
}

