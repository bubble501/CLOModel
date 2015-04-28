# How to build the components
All the components are designed to build on all [platforms supported by Qt](http://doc.qt.io/qt-5/supported-platforms.html) however, the excel interaction relies on comdef.h that is a component of Visual C and available on windows only
## Pre-Requisites
### C++11
*Developed using MSVC13*<br/>
To build the code you'll need a compiler conforming to [C++11 standard](http://en.wikipedia.org/wiki/C%2B%2B11).<br/>
The code is compatible with the subset of the standard implemented by MSVC.<br/>
[GCC](https://gcc.gnu.org/), Visual Studio's MSVC and XCode's LLVM are all valid compilers
### Boost
*Developed using version 1.57.0*<br/>
[Download](http://www.boost.org/users/download/) and unpack the library. There is no need to build it.
### Qt
*Developed using version 5.4.1*<br/>
[Download](http://www.qt.io/download-open-source/) and install Qt libraries, the open source version is sufficient.<br/>
If you use versions of the library higher than 5.9, minor changes in the code may be required in order for it to compile successfully.
### zlib
*Developed using version 1.2.8*<br/>
[Download](http://www.zlib.net/) the compiled DLL of the library or download the source code and compile it as a dynamic library following the instructions included with the source.
### Bloomberg
*Developed using version 3.7.5.1*<br/>
[Download](http://www.bloomberglabs.com/api/libraries/) the Bloomberg API library for C++.
### QBbgLib
*Developed using version 0.0.1*<br/>
[Download](https://github.com/VSRonin/QBbgLib) the Qt interface for the Bloomberg API.
Once downloaded, open the solution in visual studio and compile the library in release configuration.
You may need to set, in the project properties, the path to the Bloomberg API library mentioned above under:<ul>
<li>C/C++ &#10142; General &#10142; Additional Include Directories</li>
<li>Linker &#10142; General &#10142; Additional Library Directories</li>
</ul>
A makefile version will be available soon to speed up the build and deploy process
### Quazip
*Developed using version 0.7.1*<br/>
[Download](http://sourceforge.net/projects/quazip/) and build Quazip library using the instructions below
<pre><code>Unpack the source code in a path that does not contain spaces (we'll refer to this path as [path-to-quazip], we'll also refer to the path where you downloaded zlib as [path-to-zlib], no spaces allowed in it either)
Open the Qt terminal as administrator
Complete the environment setup (on MSVC run vcvarsall.bat e.g. "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat")
On the terminal run the following commands (<strong>if you are using MSVC use nmake instead of make</strong>):
cd [path-to-quazip]\quazip
set LIB=[path-to-zlib]\lib;%LIB%
set INCLUDE=[path-to-zlib]\include;%INCLUDE%
md moc
run_moc.bat
qmake "CONFIG+=release" "LIBS+=[path-to-zlib]\lib\zdll.lib" "PREFIX=C:\Quazip"
make
make install
qmake "CONFIG+=debug" "LIBS+=[path-to-zlib]\lib\zdll.lib" "PREFIX=C:\Quazip" "TARGET= quazipd"
make
make install
The library will be installed in C:\Quazip; to change this path change the PREFIX value above.
</code></pre>
### KDChart
*Developed using version 2.5.1*<br/>
[Download](https://customers.kdab.com/download.php) KDChart source code (registration to the website may be required) and build it using the instructions included.
### Simstring
*Developed using version 1.0*<br/>
[Download](http://chokkan.org/software/simstring/) and unpack the simstring library. There is no need to build it
## Set up the environment
The following environmental variables are needed in order to be able to compile the code. All the path should NOT contain any space
    <ul><li>[How to set environment variables in Windows](http://www.computerhope.com/issues/ch000549.htm)</li>
    <li>[How to set environment variables in Mac OS X](http://www.dowdandassociates.com/blog/content/howto-set-an-environment-variable-in-mac-os-x-launchd-plist/) (Versions 10.10 or higher)</li>
    <li>[How to set environment variables on Linux](https://help.ubuntu.com/community/EnvironmentVariables#Persistent_environment_variables)</li></ul>
<ul><li>QBBGLIBPATH - Path to the include folder of the Bloomberg C++ API. (e.g. C:\QBbgLib)</li>
<li>BOOSTPATH - Path to the boost folder of the Boost C++ library. (e.g. C:\Boost\Boost-1.57)</li>
<li>KDCHARTPATH - Path to the include folder of the KDChart library. (e.g. C:\KDAB\KDChart-2.5.1)</li>
<li>QUAZIPPATH - Path to the include folder of the Quazip library. (e.g. C:\Quazip)</li>
<li>ZLIBPATH - Path to the include folder of the zlib library. (e.g. C:\Zlib)</li>
<li>SIMSTRINGPATH - Path to the include folder of the simstring library. (e.g. C:\simstring-1.0)</li></ul>
To build the Installer you'll need to define these additional variables:
<ul><li>QTPATH - Path to the include folder of the Qt Libraries. (e.g. C:\Qt5\5.3\msvc2013)</li></ul>
## Visual Studio
[Download](http://www.qt.io/download-open-source) and install the Qt Visual Studio Addin.<br/>
[Download](http://wixtoolset.org/) and install WiX Toolset<br/>
Open the solution file.<br/>
Build the projects individually (Do not use "Build Solution" as it will incur in a race condition an possibly fail).
## Documentation
To generate the library documentation use the [Doxigen](www.doxygen.org) toolset. Just open the .doxyfile file included using doxywizard.exe.<br/>
You'll need to generate the .Net documentation before building the Installer otherwise it will fail.