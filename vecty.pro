QT += quick
QT += quickwidgets
QT += widgets 
QT += opengl
QT += gui
QT += gui-private
QT += svg
QT += svg-private

CONFIG += c++17
CONFIG += optimize_full
LIBS += -L"/usr/local/lib"
LIBS += -L"/usr/local/opt/libxml2/lib"
LIBS += -lxml2
INCLUDEPATH += $$PWD/third_party
INCLUDEPATH += $$PWD/src
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /usr/local/opt/libxml2/include

# Remove -Wall and -Wextra flag
QMAKE_CFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CFLAGS_WARN_ON -= -Wextra
QMAKE_CXXFLAGS_WARN_ON -= -Wextra

QMAKE_CXXFLAGS += -Wall
# QMAKE_CXXFLAGS += -Wextra
QMAKE_CXXFLAGS += -Wno-unused-local-typedef
QMAKE_CXXFLAGS += -Wno-unused-variable
QMAKE_CXXFLAGS += -Wno-reorder-ctor
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
QMAKE_CXXFLAGS += -ferror-limit=1
QMAKE_CXXFLAGS += -ftemplate-backtrace-limit=12

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/canvas/layer.cpp \
        src/canvas/scene.cpp \
        $$files(src/canvas/controls/*.cpp) \
        $$files(src/shape/*.cpp) \
        src/widgets/canvas_text_edit.cpp \
        src/widgets/layer_list_item.cpp \
        src/widgets/layer_params_panel.cpp \
        src/widgets/transform_panel.cpp \
        src/window/main.cpp \
        src/window/mainwindow.cpp \
        src/canvas/vcanvas.cpp \
        src/parser/svgpp_parser.cpp \
        src/parser/svgpp_impl.cpp \
        src/parser/svgpp_external_impl.cpp \
        src/gcode/toolpath_exporter.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    vecty_zh_TW.ts
CONFIG += lrelease
CONFIG += embed_translations

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/canvas/layer.h \
    src/canvas/document.h \
    $$files(src/canvas/controls/*.h) \
    $$files(src/shape/*.h) \
    src/widgets/canvas_text_edit.h \
    src/widgets/layer_list_item.h \
    src/widgets/layer_params_panel.h \
    src/widgets/transform_panel.h \
    src/window/mainwindow.h \
    src/window/osxwindow.h \
    src/canvas/vcanvas.h \
    src/parser/svgpp_common.h \
    src/parser/svgpp_parser.h \
    src/parser/svgpp_color_factory.h

win32:CONFIG(release, debug|release): LIBS += -LC:/cygwin64/lib/ -lboost_system
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/cygwin64/lib/ -lboost_systemd

INCLUDEPATH += C:/cygwin64/usr/include
DEPENDPATH += C:/cygwin64/usr/include

DESTDIR = /Users/simon/Dev/vecty/build
OBJECTS_DIR = /Users/simon/Dev/vecty/build/.obj
MOC_DIR = /Users/simon/Dev/vecty/build/.moc
RCC_DIR = /Users/simon/Dev/vecty/build/.rcc
UI_DIR = /Users/simon/Dev/vecty/build/.ui

FORMS += \
    src/widgets/layer_list_item.ui \
    src/widgets/layer_params_panel.ui \
    src/widgets/transform_panel.ui \
    src/window/mainwindow.ui

OBJECTIVE_SOURCES += src/window/osxwindow.mm
