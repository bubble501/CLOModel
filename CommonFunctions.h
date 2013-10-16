class QDate;
class QString;
#include <QList>
int MonthDiff(const QDate& a,const QDate& b);
bool ValidAnnuityVector(const QString& AnnVect);
bool ValidBloombergVector(const QString& BloombergVector);
QList<double> UnpackVect(QString Vect, int PaymFreq=1, bool AdjustFreq=true);
QString ShiftBloombergVector(const QString& OriginalVect, int ByMonths);
QList<QString> UnpackAnnuityVect(QString Vect);
double RoundUp(double a);
template<class T> bool LessThanPoint(T* a,T* b){return *a<*b;}
QString Commarize(double num,unsigned int precision=0);
double CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, int Daycount=360, double Guess=0.05,int precision=3);
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, int Daycount=360);
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate,int Daycount=360, double Guess=0.05,int precision=3);