#ifndef StressViewer_h__
#define StressViewer_h__
#include <QWidget>
#include "clomodellib_global.h"
class Waterfall;
class StressViewerPrivate;
class CLOMODELLIB_EXPORT StressViewer : public QWidget
{
	Q_OBJECT
protected:
    void closeEvent(QCloseEvent *event);
    Q_DECLARE_PRIVATE(StressViewer)
    StressViewer(StressViewerPrivate* d, QWidget *parent = nullptr);
    StressViewerPrivate* d_ptr;
public:
	StressViewer(QWidget* parent=nullptr);
    virtual ~StressViewer();
	bool LoadStress(const QString& filename);
	QString GetFirstName() const;
protected slots:
	void UpdateCombos();
	void UpdateTable();
	void SetPriceChange();
	void SetStressLevel(int r, int c);
signals:
	void StressLevelChanged(const Waterfall&);
	void Closing();
};
#endif // StressViewer_h__