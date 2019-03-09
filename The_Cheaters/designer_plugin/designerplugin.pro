CONFIG      += plugin debug_and_release

TEMPLATE    = lib

TARGET      = burgermenu

HEADERS     = burgermenuplugin.h \
              ../src/burgermenu.h

SOURCES     = burgermenuplugin.cpp \
              ../src/burgermenu.cpp

INCLUDEPATH += ../src/

RESOURCES   = resource.qrc

QT += widgets

unix|win32: LIBS += -lpyqt5

unix|win32: LIBS += -lqquickwidget

unix|win32: LIBS += -lqwebview

target.path = $$[QT_INSTALL_PLUGINS]/designer

INSTALLS    += target

QWT_CONFIG += QwtDesigner

QT += uitools
QT -= gui
