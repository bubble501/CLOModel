#ifndef TRIGGERHELPERDIALOG_H
#define TRIGGERHELPERDIALOG_H

#include <QDialog>
#include "clomodellib_global.h"
class TriggerHelperDialogPrivate;
class CLOMODELLIB_EXPORT TriggerHelperDialog : public QDialog
{
	Q_OBJECT
protected:
    TriggerHelperDialogPrivate* d_ptr;
    Q_DECLARE_PRIVATE(TriggerHelperDialog)
    TriggerHelperDialog(TriggerHelperDialogPrivate* d, QWidget *parent = nullptr);
public:
    TriggerHelperDialog(QWidget *parent = nullptr);
    virtual ~TriggerHelperDialog();
	QString GetParameters() const;
	void SetCurrentPars(const QString& pars);
private slots:
	void ClearParameters();
	void CheckOkEnabled(int index);
	void SetParameter(int parIdx, const QString& parVal);
signals:
	void ImportParam(int parIdx, const QString& parVal);
};

#endif // TRIGGERHELPERDIALOG_H
