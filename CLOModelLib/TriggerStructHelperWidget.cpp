#include "TriggerStructHelperWidget.h"
#include "Private/TriggerStructHelperWidget_p.h"
#include "Private/InternalItems.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QHeaderView>
TriggerStructHelperWidgetPrivate::~TriggerStructHelperWidgetPrivate(){}
TriggerStructHelperWidgetPrivate::TriggerStructHelperWidgetPrivate(TriggerStructHelperWidget *q)
    :q_ptr(q)
{}

TriggerStructHelperWidget::~TriggerStructHelperWidget()
{
    delete d_ptr;
}

TriggerStructHelperWidget::TriggerStructHelperWidget(const QHash<quint32, std::shared_ptr<AbstractTrigger> >& AvailableTriggers, QWidget *parent)
    : TriggerStructHelperWidget(parent)
{
    SetAvailableTriggers(AvailableTriggers);
}
TriggerStructHelperWidget::TriggerStructHelperWidget(QWidget *parent)
    : TriggerStructHelperWidget(new TriggerStructHelperWidgetPrivate(this), parent)
{}
TriggerStructHelperWidget::TriggerStructHelperWidget(TriggerStructHelperWidgetPrivate* d, QWidget *parent)
	: QWidget(parent)
    ,d_ptr(d)
{
	QLabel* TriggersLabel = new QLabel(this);
	TriggersLabel->setText(tr("Available Triggers"));

	d->TriggersModel = new QStandardItemModel(this);
    d->TriggersModel->setColumnCount(3);
    d->TriggersModel->setHorizontalHeaderLabels(QStringList() << "ID" << "Label" << "Description");
    d->TriggersModel->setRowCount(2);
    d->TriggersModel->setData(d->TriggersModel->index(0, 0), "T");
    d->TriggersModel->setData(d->TriggersModel->index(0, 1), "True");
    d->TriggersModel->setData(d->TriggersModel->index(0, 2), "Trigger that Always Passes");
    d->TriggersModel->setData(d->TriggersModel->index(1, 0), "F");
    d->TriggersModel->setData(d->TriggersModel->index(1, 1), "False");
    d->TriggersModel->setData(d->TriggersModel->index(1, 2), "Trigger that Never Passes");
	

    d->TriggerTable = new QTableView(this);
    d->TriggerTable->setModel(d->TriggersModel);
    d->TriggerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->TriggerTable->setSelectionMode(QAbstractItemView::SingleSelection);
    d->TriggerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->TriggerTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    d->TriggerTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    d->TriggerTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    d->TriggerTable->verticalHeader()->hide();
    connect(d->TriggerTable, &QTableView::doubleClicked, this, &TriggerStructHelperWidget::InsertTrigger);

	QLabel* Resultlabel = new QLabel(this);
	Resultlabel->setText(tr("Result", "Resulting trigger Structure from editor"));
    d->EncriptedTriggers = new QLineEdit(this);
    d->EncriptedTriggers->setValidator(new QRegularExpressionValidator(QRegularExpression("(?:\\*|\\+|/|-|T|F|!|\\(|\\))*"), d->EncriptedTriggers));
    connect(d->EncriptedTriggers, &QLineEdit::textChanged, this, &TriggerStructHelperWidget::DecriptTriggers);
    d->DecriptedTriggers = new QLineEdit(this);
    d->DecriptedTriggers->setReadOnly(true);

    d->NegateButton = new QPushButton(this);
    d->NegateButton->setText(tr("Negate"));
    connect(d->NegateButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::Negate);
    d->AndButton = new QPushButton(this);
    d->AndButton->setText(tr("AND"));
    d->OrButton = new QPushButton(this);
    d->OrButton->setText(tr("OR"));
    d->NandButton = new QPushButton(this);
    d->NandButton->setText(tr("NAND"));
    d->NorButton = new QPushButton(this);
    d->NorButton->setText(tr("NOR"));
    connect(d->AndButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::InsertOperator);
    connect(d->OrButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::InsertOperator);
    connect(d->NandButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::InsertOperator);
    connect(d->NorButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::InsertOperator);
    d->OpenParButton = new QPushButton(this);
    d->OpenParButton->setText("(");
    connect(d->OpenParButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::OpenParenthesis);
    d->CloseParButton = new QPushButton(this);
    d->CloseParButton->setText(")");
    connect(d->CloseParButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::CloseParenthesis);
    d->DelButton = new QPushButton(this);
    d->DelButton->setText(tr("DEL", "DEL button in calculator"));
    d->DelButton->setEnabled(false);
    connect(d->DelButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::Del);
    d->ClearButton = new QPushButton(this);
    d->ClearButton->setText(tr("Clear", "AC button in calculator"));
    d->ClearButton->setEnabled(false);
    connect(d->ClearButton, &QPushButton::clicked, this, &TriggerStructHelperWidget::Clear);

	QGridLayout* Mainlay = new QGridLayout(this);
	Mainlay->addWidget(TriggersLabel, 0, 0);
    Mainlay->addWidget(d->TriggerTable, 1, 0, 6, 1);
    Mainlay->addWidget(d->NegateButton, 1, 1, 1, 2);
    Mainlay->addWidget(d->AndButton, 2, 1);
    Mainlay->addWidget(d->OrButton, 2, 2);
    Mainlay->addWidget(d->NandButton, 3, 1);
    Mainlay->addWidget(d->NorButton, 3, 2);
    Mainlay->addWidget(d->OpenParButton, 4, 1);
    Mainlay->addWidget(d->CloseParButton, 4, 2);
    Mainlay->addWidget(d->DelButton, 5, 1);
    Mainlay->addWidget(d->ClearButton, 5, 2);
	Mainlay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 6, 1, 1, 2);
	Mainlay->addWidget(Resultlabel, 7, 0, 1, 3);
    Mainlay->addWidget(d->EncriptedTriggers, 8, 0, 1, 3);
    Mainlay->addWidget(d->DecriptedTriggers, 9, 0, 1, 3);
    d->EnableOperators(false);
}

void TriggerStructHelperWidget::SetAvailableTriggers(const QHash<quint32, std::shared_ptr<AbstractTrigger> >& AvailableTriggers)
{
    Q_D(TriggerStructHelperWidget);
    d->TriggersModel->setRowCount(AvailableTriggers.count() + 2);
	QString RegExpString = "(?:\\*|\\+|/|-|T|F";
	int RowCount = 2;
	QRegularExpression LabelRegExp("Label: .+?\n");
	for (auto i = AvailableTriggers.constBegin(); i != AvailableTriggers.constEnd(); ++i, ++RowCount) {
        d->TriggersModel->setData(d->TriggersModel->index(RowCount, 0), i.key());
        d->TriggersModel->setData(d->TriggersModel->index(RowCount, 1), i.value()->GetTriggerLabel());
		QString TempString = i.value()->ToString(); TempString.replace(LabelRegExp, ""); TempString.replace("\n", " - ");
        d->TriggersModel->setData(d->TriggersModel->index(RowCount, 2), TempString);
        d->TriggersModel->setData(d->TriggersModel->index(RowCount, 2), TempString, Qt::ToolTipRole);
		RegExpString += '|' + QString::number(i.key());
	}
    d->TriggersModel->sort(0);
	RegExpString += "|!|\\(|\\))*";
    d->EncriptedTriggers->setValidator(new QRegularExpressionValidator(QRegularExpression(RegExpString, QRegularExpression::CaseInsensitiveOption), d->EncriptedTriggers));
}

void TriggerStructHelperWidget::InsertOperator()
{
    Q_D(TriggerStructHelperWidget);
	if (!sender())
        return;
    if (sender() == d->AndButton) d->EncriptedTriggers->insert("*");
    if (sender() == d->OrButton) d->EncriptedTriggers->insert("+");
    if (sender() == d->NandButton) d->EncriptedTriggers->insert("/");
    if (sender() == d->NorButton) d->EncriptedTriggers->insert("-");
}

void TriggerStructHelperWidget::OpenParenthesis()
{
    Q_D(TriggerStructHelperWidget);
    d->EncriptedTriggers->insert("()");
    d->EncriptedTriggers->cursorBackward(false, 1);
}

void TriggerStructHelperWidget::CloseParenthesis()
{
    Q_D(TriggerStructHelperWidget);
    if (d->EncriptedTriggers->cursorPosition() >= d->EncriptedTriggers->text().size())  d->EncriptedTriggers->insert(")");
	else
	{
        if (d->EncriptedTriggers->text().at(d->EncriptedTriggers->cursorPosition()) == ')')
            d->EncriptedTriggers->cursorForward(false, 1);
		else
            d->EncriptedTriggers->insert(")");
	}
}

void TriggerStructHelperWidget::InsertTrigger(const QModelIndex & index)
{
    Q_D(TriggerStructHelperWidget);
    d->EncriptedTriggers->insert(d->TriggersModel->index(index.row(), 0).data().toString());
}

void TriggerStructHelperWidget::Del()
{
    Q_D(TriggerStructHelperWidget);
    if (d->EncriptedTriggers->cursorPosition() == 0)
        return;
    QChar lastChar = d->EncriptedTriggers->text().at(d->EncriptedTriggers->cursorPosition() - 1);
	switch (lastChar.toLatin1()) {
	case '+':
	case '-':
	case '*':
	case '/':
	case '!':
	case '(':
	case ')':
        d->EncriptedTriggers->cursorBackward(true, 1);
        d->EncriptedTriggers->insert("");
		return;
	default: break;
	}
	do{
        d->EncriptedTriggers->cursorBackward(true, 1);
        d->EncriptedTriggers->insert("");
        if (d->EncriptedTriggers->cursorPosition() == 0 || lastChar == '!') break;
        lastChar = d->EncriptedTriggers->text().at(d->EncriptedTriggers->cursorPosition() - 1);
	} while (lastChar.isDigit() || lastChar == '!' || lastChar == 'T' || lastChar == 'F');
}

void TriggerStructHelperWidget::Clear()
{
    Q_D(TriggerStructHelperWidget);
	d->EncriptedTriggers->clear();
}

void TriggerStructHelperWidgetPrivate::EnableOperators(bool a)
{
	TriggerTable->setEnabled(!a);
	AndButton->setEnabled(a);
	OrButton->setEnabled(a);
	NandButton->setEnabled(a);
	NorButton->setEnabled(a);
	OpenParButton->setEnabled(!a);
	CloseParButton->setEnabled(a && EncriptedTriggers->text().count('(')>0);
}

void TriggerStructHelperWidget::Negate()
{
    Q_D(TriggerStructHelperWidget);
	int CountBackPos = 0;
    if (d->EncriptedTriggers->cursorPosition() == 0) 
        return  d->EncriptedTriggers->setText('!' + d->EncriptedTriggers->text());
    QChar lastChar = d->EncriptedTriggers->text().at(d->EncriptedTriggers->cursorPosition() - 1);
	while ( lastChar.isDigit() || lastChar == 'T' || lastChar == 'F') {
        d->EncriptedTriggers->cursorBackward(false, 1);
		++CountBackPos;
        if (d->EncriptedTriggers->cursorPosition() == 0) {
            d->EncriptedTriggers->setText('!' + d->EncriptedTriggers->text());
            d->EncriptedTriggers->cursorForward(false, CountBackPos);
			return;
		}
        lastChar = d->EncriptedTriggers->text().at(d->EncriptedTriggers->cursorPosition() - 1);
	}
	if (lastChar == '!') {
        d->EncriptedTriggers->cursorBackward(true, 1);
        d->EncriptedTriggers->insert("");
	}
    else  d->EncriptedTriggers->insert("!");
    d->EncriptedTriggers->cursorForward(false, CountBackPos);
}

void TriggerStructHelperWidget::DecriptTriggers(QString encr)
{
    Q_D(TriggerStructHelperWidget);
	if (encr.isEmpty()) {
        d->DecriptedTriggers->setText(QString());
        d->DecriptedTriggers->setStyleSheet(QString());
        d->EnableOperators(false);
	}
	else {
		QChar lastChar = encr.at(encr.size() - 1);
        d->EnableOperators(lastChar.isDigit() || lastChar.toUpper() == 'T' || lastChar.toUpper() == 'F');
		if (InfixToPostfix(encr).isEmpty()) {
            d->DecriptedTriggers->setStyleSheet("color: red;");
            d->DecriptedTriggers->setText("Invalid Trigger Structure");
		}
		else {
			for (int i = 0; i < 2; ++i) {
				//Decrypt True and False
                encr.replace(d->TriggersModel->index(i, 0).data().toString(), d->TriggersModel->index(i, 1).data().toString(), Qt::CaseInsensitive);
			}
			encr.replace("*", " AND ");
			encr.replace("+", " OR ");
			encr.replace("-", " NOR ");
			encr.replace("/", " NAND ");
			encr.replace("!", "NOT ");
            for (int i = 2; i < d->TriggersModel->rowCount(); ++i) {
                encr.replace(d->TriggersModel->index(i, 0).data().toString(), d->TriggersModel->index(i, 1).data().toString(), Qt::CaseInsensitive);
			}
            d->DecriptedTriggers->setText(encr);
            d->DecriptedTriggers->setStyleSheet(QString());
		}
		
	}
    d->DelButton->setEnabled(!d->EncriptedTriggers->text().isEmpty());
    d->ClearButton->setEnabled(!d->EncriptedTriggers->text().isEmpty());
    d->EncriptedTriggers->activateWindow();
    d->EncriptedTriggers->deselect();
}

void TriggerStructHelperWidget::SetCurrentStructure(const QString& a)
{
    Q_D(TriggerStructHelperWidget);
    d->EncriptedTriggers->setText(NormaliseTriggerStructure(a));
}

QString TriggerStructHelperWidget::GetResult() const
{
    Q_D(const TriggerStructHelperWidget);
	return d->EncriptedTriggers->text();
}

