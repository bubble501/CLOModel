#include "BigNumDelegate.h"
#include "BigNumSpin.h"
#include <QLabel>
#include <QPainter>
BigNumDelegate::BigNumDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_adjustPadding(true)
{
    m_baseSpinn = new BigNumSpin;
    m_baseSpinn->setMinimum(-10000000000.0);
    m_baseSpinn->setMaximum(10000000000.0);
    m_baseSpinn->setSingleStep(1000.0);
    m_baseSpinn->setDecimals(2);
    m_label = new QLabel;
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

BigNumDelegate::~BigNumDelegate()
{
    m_baseSpinn->deleteLater();
    m_label->deleteLater();
}

QWidget * BigNumDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    BigNumSpin* result = new BigNumSpin(parent);
    result->setMinimum(minimum());
    result->setMaximum(maximum());
    result->setDecimals(decimals());
    result->setSingleStep(singleStep());
    result->setPrefix(prefix());
    result->setSuffix(suffix());
    return result;
}
void BigNumDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.model()->data(index, Qt::EditRole).isNull())
        qobject_cast<BigNumSpin*>(editor)->clear();
    else
        qobject_cast<BigNumSpin*>(editor)->setValue(index.model()->data(index, Qt::EditRole).toDouble());
}

void BigNumDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto spin = qobject_cast<BigNumSpin*>(editor);
    spin->interpretText();
    if (spin->cleanText().isEmpty())
        model->setData(index, QVariant(), Qt::EditRole);
    else
        model->setData(index, spin->value(), Qt::EditRole);
}

void BigNumDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

QSize BigNumDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    /*m_baseSpinn->blockSignals(true);
    m_baseSpinn->setValue(index.model()->data(index, Qt::DisplayRole).toDouble());
    m_baseSpinn->blockSignals(false);*/
    return m_baseSpinn->sizeHint();
}

void BigNumDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool selected = option.state & QStyle::State_Selected;
    bool mouseOver = option.state & QStyle::State_MouseOver;
    QPalette palette(option.palette);
    if (selected){
        palette.setBrush(QPalette::Active, QPalette::Window, option.palette.highlight());
        palette.setBrush(QPalette::Active, QPalette::WindowText, option.palette.highlightedText());
    }
    else if (mouseOver) {
        palette.setBrush(QPalette::Active, QPalette::Window, option.palette.button());
        palette.setBrush(QPalette::Active, QPalette::WindowText, option.palette.buttonText());
    }
    else{
        palette.setBrush(QPalette::Active, QPalette::Window, option.palette.base());
        palette.setBrush(QPalette::Active, QPalette::WindowText, option.palette.text());
    }
    QRect labelRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height());
    if (m_adjustPadding)
        labelRect.adjust(LabelPadding, LabelPadding, -(2 * LabelPadding), -(2 * LabelPadding));
    m_label->setPalette(palette);
    m_label->setFixedSize(qMax(0, labelRect.width()), labelRect.height());
    if (index.model()->data(index, Qt::DisplayRole).isNull()) {
        m_label->clear();
    }
    else {
        m_label->setText(
            (index.model()->data(index, CustomStyleRole).toString().contains("%1") ? index.model()->data(index, CustomStyleRole).toString() : QString("%1"))
            .arg(prefix() + m_label->locale().toString(index.model()->data(index, Qt::DisplayRole).toDouble(), 'f', decimals()) + suffix())
        );
    }
    if (index.model()->data(index, Qt::TextAlignmentRole).isNull())
        m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    else
        m_label->setAlignment(Qt::Alignment(index.model()->data(index, Qt::TextAlignmentRole).toInt()));
    QPixmap pixmap(m_label->size());
    m_label->render(&pixmap);
    painter->drawPixmap(labelRect, pixmap);
}

void BigNumDelegate::setMinimum(double val)
{
    m_baseSpinn->setMinimum(val);
}

void BigNumDelegate::setMaximum(double val)
{
    m_baseSpinn->setMaximum(val);
}

void BigNumDelegate::setRange(double min, double max)
{
    setMinimum(min);
    setMaximum(max);
}

double BigNumDelegate::minimum() const
{
    return m_baseSpinn->minimum();
}
double BigNumDelegate::maximum() const
{
    return m_baseSpinn->maximum();
}

int BigNumDelegate::decimals() const
{
    return m_baseSpinn->decimals();
}

void BigNumDelegate::setDecimals(int val)
{
    m_baseSpinn->setDecimals(val);
}

double BigNumDelegate::singleStep() const
{
    return m_baseSpinn->singleStep();
}

void BigNumDelegate::setSingleStep(double val)
{
    m_baseSpinn->setSingleStep(val);
}

QString BigNumDelegate::suffix() const
{
    return m_baseSpinn->suffix();
}

QString BigNumDelegate::prefix() const
{
    return m_baseSpinn->prefix();
}

void BigNumDelegate::setSuffix(const QString& val)
{
    m_baseSpinn->setSuffix(val);
}

void BigNumDelegate::setPrefix(const QString& val)
{
    m_baseSpinn->setPrefix(val);
}

bool BigNumDelegate::adjustPadding() const
{
    return m_adjustPadding;
}

void BigNumDelegate::setAdjustPadding(bool val)
{
    m_adjustPadding = val;
}
