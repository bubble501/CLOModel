#ifndef StandaloneViewer_h__
#define StandaloneViewer_h__
#include <QMainWindow>
#include "Waterfall.h"
#define MaxRecentFiles 10
class QMenu;
class QAction;
class SummaryView;
class StressViewer;
class StandaloneViewer : public QMainWindow{
	Q_OBJECT
public:
	StandaloneViewer(QWidget *parent=0);
public slots:
	void AdjustTableSizes();
	void LoadFile(const QString& fileName);
private slots:
	void open();
	void openStress();
	void openRecentFile();
	void closeFile();
	void HandleStressChange(Waterfall a);
private:
	SummaryView* TheViewer;
	StressViewer* StressWindow;
	QStringList recentFiles;
	QStringList recentNames;
	QMenu *fileMenu;
	QAction *recentFileActions[MaxRecentFiles];
	QAction *openAction;
	QAction *openStressAction;
	QAction *exitAction;
	QAction *CloseFileAction;
	QAction *separatorAction;
	void createActions();
	void createMenus();
	bool loadStressFile(const QString &fileName);
	void updateRecentFileActions();
protected:
	void closeEvent(QCloseEvent *event);
};
#endif // StandaloneViewer_h__