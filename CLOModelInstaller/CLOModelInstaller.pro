TEMPLATE = aux

INSTALLER = installer

INPUT = $$PWD/config/config.xml $$PWD/packages
buildinstaller.input = INPUT
buildinstaller.output = $$INSTALLER
buildinstaller.commands = ../../bin/binarycreator -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}
buildinstaller.CONFIG += target_predeps no_link combine

QMAKE_EXTRA_COMPILERS += buildinstaller

