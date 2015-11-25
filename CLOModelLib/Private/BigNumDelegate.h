#ifndef BIGNUMDELEGATE_H
#define BIGNUMDELEGATE_H


#include <QStyledItemDelegate>
class QLabel;
class BigNumSpin;
class BigNumDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    BigNumDelegate(QObject *parent);
    ~BigNumDelegate();
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;   
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setMinimum(double val);
    virtual void setMaximum(double val);
    virtual void setRange(double min,double max);
    virtual double minimum() const;
    virtual double maximum() const;
    virtual int decimals() const;
    virtual void setDecimals(int val);
    virtual double singleStep() const;
    virtual void setSingleStep(double val);
    virtual QString suffix() const;
    virtual QString prefix() const;
    virtual void setSuffix(const QString& val);
    virtual void setPrefix(const QString& val);
    virtual bool adjustPadding() const;
    virtual void setAdjustPadding(bool val);
    enum { CustomStyleRole = Qt::UserRole + 129 };
protected:
    enum { LabelPadding = 1 };
    BigNumSpin* m_baseSpinn;
    QLabel* m_label;
    bool m_adjustPadding;
    
};

#endif // BIGNUMDELEGATE_H
