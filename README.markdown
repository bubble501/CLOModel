# Collateralised Loan Obligations Cash Flow Modeller
## ![TwentyFour Asset management](http://www.twentyfouram.com/sites/all/themes/twentyfourasset/images/logo.jpg)
This project integrates cash flow modelling tool for CLOs usable entirely from Excel.

## Dependencies
The project is based on the [Qt Libraries](http://qt-project.org/) version 4.8 and will need them to compile.

In addition to those base libraries it makes use of:

- [The Bloomberg Api](http://www.openbloomberg.com/open-api/) (A Bloomberg login is required only if you wish to use Bloomberg functionalities)
- [zlib](http://www.zlib.net/) to manage compressed files (this also require that the **QUAZIP_BUILD** simbol is defined)
- [Microsoft Office SDK](http://www.microsoft.com/en-us/download/developer-tools.aspx) to handle COM items used to comunicate with excel
- [KD Chart](http://www.kdab.com/kdab-products/kd-chart/) to display charts


### Linked libraries
- blpapi3_32.dll
- kdchart2.dll
- QtCore4.dll
- QtGui4.dll
- QtSvg4.dll
- zlib1.dll