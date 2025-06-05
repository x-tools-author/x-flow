/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "application.h"
#include "mainwindow.h"

#ifdef X_MAGIC
#include <QStyleHints>
#endif

#ifdef X_MS_STORE
#include "microsoftstore.h"
#endif

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <QAbstractNativeEventFilter>
//https://blog.csdn.net/omg_orange/article/details/116779492
//QTBUG-77974, 开启有道词典时，会导致程序卡死
class NativeEventFilter : public QAbstractNativeEventFilter
{
public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override
    {
        MSG *msg = (MSG *) message;
        if (msg->message == WM_GETOBJECT) {
            return true;
        }

        return false;
    }
};
#endif

int main(int argc, char *argv[])
{
    Application::setOrganizationName("xTools");
    Application::setApplicationName("xFlow");
    Application::installLog(argv[0]);
    Application::setupHdpi();
    Application app(argc, argv);
#ifdef Q_OS_WIN
    app.installNativeEventFilter(new NativeEventFilter());
#endif
    app.setupTheme();
    app.setupLanguage();
#ifndef X_MAGIC
    app.showSplashScreenMessage(QObject::tr("Application is booting..."));
#endif
    app.setupColorScheme();
    app.setupPlugins();

    MainWindow window;
#if defined(X_MS_STORE)
    MicrosoftStore::singleton().initialize(reinterpret_cast<void *>(window.winId()));
#endif

    QSplashScreen *splash = app.splashScreen();
    splash->finish(&window);

#ifdef X_MAGIC
    QStyleHints *styleHints = app.styleHints();
    styleHints->setColorScheme(Qt::ColorScheme::Dark);
    window.setWindowOpacity(0.4);

    qApp->processEvents();
    window.resize(1280, 720);
    window.show();
    window.moveToCenter();
#else
#if 0
    window.resize(1280, 720);
    window.show();
    window.moveToCenter();
#else
    window.showMaximized();
#endif
#endif

    window.loadSettings();
    int ret = app.exec();
    Application::uninstallLog();
    return ret;
}
