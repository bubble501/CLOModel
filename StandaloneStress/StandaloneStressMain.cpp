#include "StandaloneStress.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StandaloneStress TempObj;
    TempObj.show();
    return a.exec();
}


/*
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QProgressDialog>
#include "MtgCalculator.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QProgressDialog Prog;
    Prog.setRange(0, 100);
    Prog.setWindowTitle("Progress");
    Prog.setLabelText("Calculating Loans");
    QFile file("C:/Temp/ModelInputs");
    file.open(QIODevice::ReadOnly);
    qint32 VersionChecker;
    QDataStream out(&file);
    out.setVersion(StreamVersionUsed);
    out >> VersionChecker;
    if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
        file.close();
        return 1;
    }
    Waterfall TempWtf, TemCallWtf;
    MtgCalculator TempPool;
    TempWtf.SetLoadProtocolVersion(VersionChecker);
    out >> TempPool >> TempWtf;
    file.close();
    TempPool.connect(&TempPool, &MtgCalculator::ProgressPct, &Prog, &QProgressDialog::setValue);
    TempPool.connect(&TempPool, &MtgCalculator::Calculated, &Prog, &QProgressDialog::hide);
    TempPool.connect(&TempPool, &MtgCalculator::Calculated, [&TempWtf, &TempPool, &TemCallWtf]()
    {
        TempWtf.ResetMtgFlows();
        TempWtf.AddMortgagesFlows(*TempPool.GetAggregatedResults());
        TempPool.ClearResults();
        TempWtf.SetUseCall(false);
        if (TempWtf.ReadyToCalculate().isEmpty()) {
            TemCallWtf = TempWtf;
            TemCallWtf.SetUseCall(true);
            TempWtf.CalculateTranchesCashFlows();
            TemCallWtf.CalculateTranchesCashFlows();
            QDir UnifiedDir(GetFromConfig("Folders", "UnifiedResultsFolder"));
            if (UnifiedDir.exists()) {
                QString AdjDealName = TempWtf.GetDealName();
                if (AdjDealName.isEmpty() && TempWtf.GetTranchesCount() > 0) {
                    AdjDealName = TempWtf.GetTranche(0)->GetTrancheName();
                }
                QFile UnifiedFile(UnifiedDir.absoluteFilePath(AdjDealName + ".clom"));
                if (UnifiedFile.open(QIODevice::WriteOnly)) {
                    QDataStream out(&UnifiedFile);
                    out.setVersion(StreamVersionUsed);
                    out
                        << qint32(ModelVersionNumber)
                        << QDate()
                        << false
                        << true
                        << TempWtf
                        << TemCallWtf
                        << TempPool
                        << ConstantBaseRateTable()
                        << ForwardBaseRateTable()
                        ;
                    UnifiedFile.close();
                }
            }
            QMessageBox::information(0, "Finished", "Calculation Finished");
        }
        else {
            QMessageBox::critical(0, "Error", "Invalid Tranche Input");
        }
    });
    if (TempPool.ReadyToCalculate().isEmpty()) {
        Prog.show();
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        TempPool.StartCalculation();
    }
    else {
        QMessageBox::critical(0, "Error", "Invalid Loan Input");
    }
    return a.exec();
}*/


