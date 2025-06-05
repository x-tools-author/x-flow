/***************************************************************************************************
 * Copyright 2018-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "application.h"

#include <glog/logging.h>

#include <QDir>
#include <QEventLoop>
#include <QLocale>
#include <QMessageBox>
#include <QPainter>
#include <QProcess>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTimer>
#include <QTranslator>

#include "common/common.h"
#include "dockwidgets/log/logdockwidgetcontentmodel.h"
#include "nodeeditor/plugin/pluginmanager.h"
#include "nodeeditor/plugin/plugins/abstractplugin.h"
#include "theme/thememanager.h"

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
#ifdef X_FLOW_LATEST_GIT_TAG
    QString tmp(X_FLOW_LATEST_GIT_TAG);
    if (tmp.contains("v")) {
        tmp.remove("v");
    }
    Application::setApplicationVersion(tmp);
#else
    Application::setApplicationVersion("0.0.0");
#endif

    // 使用微软雅黑字体
    QFont font = qApp->font();
    font.setFamily("Microsoft YaHei UI");
}

Application::~Application() {}

void qtLogToGoogleLog(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toUtf8();
    const char *file = context.file ? context.file : "";
    const int line = context.line;

    switch (type) {
    case QtWarningMsg:
        google::LogMessage(file, line, google::GLOG_WARNING).stream() << localMsg.data();
        break;
    case QtCriticalMsg:
        google::LogMessage(file, line, google::GLOG_ERROR).stream() << localMsg.data();
        break;
    case QtFatalMsg:
        google::LogMessage(file, line, google::GLOG_FATAL).stream() << localMsg.data();
        break;
    default:
        google::LogMessage(file, line, google::GLOG_INFO).stream() << localMsg.data();
        break;
    }

    LogDockWidgetContentModel::singleton().addLog(type, msg);
}

void Application::installLog(char *argv0)
{
#if !defined(QT_RELEASE)
    // Google log is just for release edition.
    return;
#endif

    // Redirect the log message to Qt log.
    qInstallMessageHandler(qtLogToGoogleLog);

    // Google log initializing...
    QString logPath = settingsPath();
    logPath += "/log";
    QDir dir(logPath);
    if (!dir.exists(logPath) && !dir.mkpath(logPath)) {
        qWarning() << "Make log directory failed";
    }

    auto keep = std::chrono::minutes(30 * 24 * 60);
    google::SetLogFilenameExtension(".log");     // The suffix of log file.
    google::EnableLogCleaner(keep);              // Keep the log file for 30 days.
    google::SetApplicationFingerprint("xTools"); // (It seem to be no use.)

    fLB::FLAGS_logtostdout = false;
    fLB::FLAGS_logtostderr = false;
    fLS::FLAGS_log_dir = logPath.toUtf8().data(); // The path of log.
    fLI::FLAGS_logbufsecs = 0;                    //
    fLU::FLAGS_max_log_size = 10;                 // The max size(MB) of log file.
    fLB::FLAGS_stop_logging_if_full_disk = true;  //

    google::InitGoogleLogging(argv0);
    qInfo() << "The logging path is:" << qPrintable(logPath);
}

void Application::uninstallLog()
{
#if defined(QT_RELEASE)
    google::ShutdownGoogleLogging();
#endif
}

void Application::setupHdpi()
{
    QSettings *settings = Application::settings();
    int defaultPolicy = static_cast<int>(QGuiApplication::highDpiScaleFactorRoundingPolicy());
    int policy = settings->value(SettingsKey().hdpi, defaultPolicy).toInt();

    const auto cookedPolicy = static_cast<Qt::HighDpiScaleFactorRoundingPolicy>(policy);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(cookedPolicy);
    qInfo() << "The current high dpi policy is:" << cookedPolicy;
}

QSettings *Application::settings()
{
    QStandardPaths::StandardLocation type = QStandardPaths::AppConfigLocation;
    QString path = QStandardPaths::writableLocation(type);
    QString fileName = QString("%1/%2_v3.ini").arg(path, applicationName());
    static QSettings settings(fileName, QSettings::IniFormat);

    return &settings;
}

QString Application::settingsPath()
{
    QSettings *settings = Application::settings();
    QString path = settings->fileName();
    return path.left(path.lastIndexOf('/'));
}

void Application::setupAppStyle()
{
    QSettings *settings = Application::settings();
    if (!qApp || !Application::style()) {
        return;
    }

    QString defaultStyle = xDefaultStyleName;
    QString style = settings->value(SettingsKey().style, defaultStyle).toString();
    if (!QStyleFactory::keys().contains(style)) {
        qWarning() << "The style is not supported:" << style;
        return;
    }

    Application::setStyle(QStyleFactory::create(style));
    qInfo() << "The current application style is:" << style;
}

void Application::execMs(int ms)
{
    QTimer tmp;
    QEventLoop loop;
    tmp.setSingleShot(true);
    QObject::connect(&tmp, &QTimer::timeout, &loop, &QEventLoop::quit);
    tmp.start(ms);
    loop.exec();
}

void setupLanguage(const QString &qmFile)
{
    QTranslator *translator = new QTranslator();
    if (!translator->load(qmFile)) {
        auto info = QString("The language file(%1) can not be loaded, English will be used.")
                        .arg(qmFile);
        qWarning() << info;
        return;
    }

    if (!qApp->installTranslator(translator)) {
        qWarning() << "The language has been setup, English will be used.";
    } else {
        qInfo() << "The language has been setup, current language file is:" << qmFile;
    }
}

void Application::setupLanguage()
{
    QSettings *settings = Application::settings();
    QString defaultLanguage = QLocale::system().name();
    QString language = settings->value(SettingsKey().language, defaultLanguage).toString();

    QString appPath = QApplication::applicationDirPath();
    QString qtQmFile = QString("%1/translations/qt_%2.qm").arg(appPath, language);
    ::setupLanguage(qtQmFile);

    QString xToolsQmFile = QString(":/res/translations/xFlow_%1.qm").arg(language);
    ::setupLanguage(xToolsQmFile);

    // 设置插件语言
    AbstractPlugin::setLanguage(language);
}

void Application::setupTheme()
{
    QSettings *settings = Application::settings();
    QString theme = settings->value(SettingsKey().theme, xDarkTheme).toString();
    ThemeManager::singleton().setCurrentTheme(theme);

    if (theme == xDarkTheme) {
        qApp->styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    } else {
        qApp->styleHints()->setColorScheme(Qt::ColorScheme::Light);
    }
}

void Application::setupColorScheme()
{
#if xEnableColorScheme
    QSettings *settings = Application::settings();
    auto def = qApp->styleHints()->colorScheme();
    int colorScheme = settings->value(SettingsKey().colorScheme, static_cast<int>(def)).toInt();
    qApp->styleHints()->setColorScheme(static_cast<Qt::ColorScheme>(colorScheme));
    qInfo() << "The current color scheme is:" << qApp->styleHints()->colorScheme();
#endif
}

void Application::setupPlugins()
{
    SettingsKey keys;
    QString defaultPath = applicationDirPath() + "/plugins";
    QString pluginsPath = Application::settings()->value(keys.pluginsPath, defaultPath).toString();
    if (pluginsPath.isEmpty()) {
        pluginsPath = defaultPath;
    }

    qInfo() << "The plugins path is:" << pluginsPath;
    PluginManager::singleton().loadPlugins(pluginsPath);
}

QSplashScreen *Application::splashScreen()
{
    if (!qApp) {
        return Q_NULLPTR;
    }

    static QSplashScreen *splashScreen = Q_NULLPTR;
    if (!splashScreen) {
        QFont font = qApp->font();
        font.setPixelSize(52);

        QFontMetrics fontMetrics(font);
        const QString displayName = applicationName();
        int width = fontMetrics.boundingRect(displayName).width() * 1.2;

        QPixmap pixMap(width < 600 ? 600 : width, 260);
        pixMap.fill(QColor(0x1f1f1f));

        QPainter painter(&pixMap);
        painter.setPen(QColor(Qt::white));
        painter.setFont(font);
        painter.drawText(pixMap.rect(), Qt::AlignHCenter | Qt::AlignVCenter, displayName);
        painter.setPen(QColor(0x1f2c9f));
        painter.drawRect(pixMap.rect() - QMargins(1, 1, 1, 1));

        splashScreen = new QSplashScreen(pixMap);
        splashScreen->setDisabled(true);
    }

    return splashScreen;
}

void Application::showSplashScreenMessage(const QString &msg)
{
    QSplashScreen *splashScreen = Application::splashScreen();
    if (!splashScreen) {
        return;
    }

#if defined(QT_OS_ANDROID)
    splashScreen->showFullScreen();
#else
    splashScreen->show();
#endif
    splashScreen->showMessage(msg, Qt::AlignBottom | Qt::AlignLeft, Qt::white);
    QApplication::processEvents();
}

QString Application::left2rightTips() const
{
    return tr("Transfer rx bytes with right page");
}

QString Application::right2leftTips() const
{
    return tr("Transfer rx bytes with left page");
}

QString Application::saveImagePath() const
{
    QSettings *settings = Application::settings();
    QString path = settings->value(SettingsKey().saveImagePath, "").toString();
    if (path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    }

    return path;
}

void Application::setSaveImagePath(const QString &path)
{
    QSettings *settings = Application::settings();
    settings->setValue(SettingsKey().saveImagePath, path);
    settings->sync();
}

void Application::tryToReboot()
{
    QString title = tr("Need to Reboot");
    QString text = tr("The operation need to reboot to effected, reboot the application now?");
    int ret = QMessageBox::information(nullptr, title, text, QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        QProcess::startDetached(QApplication::applicationFilePath(), QStringList());
        xApp->execMs(100);
        qApp->closeAllWindows();
    }
}
