#ifndef BIGNUMSPIN_H
#define BIGNUMSPIN_H

#include <QDoubleSpinBox>
class BigNumSpin : public QDoubleSpinBox
{
    Q_OBJECT

public:
    BigNumSpin(QWidget *parent=nullptr);
    virtual QString textFromValue(double value) const override;
    virtual double valueFromText(const QString& text) const override;
    virtual QValidator::State validate(QString& text, int& pos)const override;
};

#endif // BIGNUMSPIN_H
