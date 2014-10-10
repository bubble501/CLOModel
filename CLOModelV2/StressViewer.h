
#include <QWidget>
#include "Waterfall.h"
#include "StressTest.h"
#include <QPropertyAnimation>
class QTableWidget;
class QLabel;
class QComboBox;
class QDoubleSpinBox;
class StressViewer : public QWidget{
	Q_OBJECT
public:
	StressViewer(QWidget* parent=0);
	bool LoadStress(const QString& filename);
	QString GetFirstName() const { return (*(StressTarget->GetResults().begin()))->GetDealName(); }
private:
	StressTest* StressTarget;
	
protected:
	void closeEvent(QCloseEvent *event);

//signals:
	//void StressLevelChanged(Waterfall);
	//void Closing();
};