# Collateralised Loan Obligations Cash Flow Modeller
## ![TwentyFour Asset management](http://www.twentyfouram.com/sites/all/themes/twentyfourasset/images/logo.jpg)
This project integrates cash flow modelling tool for CLOs usable entirely from Excel.
#### The project is divided into 3 parts:
- The actual model written in C++ and compiled into a DLL that can be called from excel
- The VBA modules that take care of interfacing with the library providing input and output as well as implementing some functionality that can be used directly from excel and need no external library
- The Installer, a C++ executable that makes sure the required libraryes are installed on the user PC

## Dependencies
The project is based on the [Qt Libraries](http://qt-project.org/) version 4.8 and will need them to compile.

In addition to those base libraries it makes use of:

- [The Bloomberg Api](http://www.openbloomberg.com/open-api/) (A Bloomberg login is required only if you wish to use Bloomberg functionalities)
- [zlib](http://www.zlib.net/) to manage compressed files
- [Microsoft Office SDK](http://www.microsoft.com/en-us/download/developer-tools.aspx) to handle COM items used to comunicate with excel
- [KD Chart](http://www.kdab.com/kdab-products/kd-chart/) to display charts

Nightly builds also require:

- [Boost Libraries](http://www.boost.org/)
- [Eigen Libraries](http://eigen.tuxfamily.org/)

### Linked libraries
- blpapi3_32.dll
- kdchart2.dll
- QtCore4.dll
- QtGui4.dll
- QtSvg4.dll
- zlib1.dll

## License
 This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).