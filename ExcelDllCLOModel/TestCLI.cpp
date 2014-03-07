// TestCLI.cpp : main project file.

#include "ManWatFalPrior.h"
using namespace System;
using namespace ManagedCLO;
int main(array<System::String ^> ^args)
{
	ManWatFalPrior^ a = gcnew ManWatFalPrior();
	a->RedemptionGroup=1;
	a->RedemptionShare=0.5;
	a->GroupTarget=1;
}
