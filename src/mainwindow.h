/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QtNodes/Definitions>

#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QSlider>
#include <QToolButton>

namespace Ui {
class MainWindow;
}

#ifndef X_FLOW_STORE
class LicenseManagerUi;
#endif

class Preference;
class NodeEditor;
class LogDockWidget;
class OutputDockWidget;
class NodesDockWidget;
class DataDockWidget;
class NavigatorDockWidget;
class ParametersDockWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void loadSettings();
    void moveToCenter();

    // 不要将Q_INVOKABLE去掉，这两个函数再“调式输出”节点中被调用。
    Q_INVOKABLE void outputBytes(const QString &txt, int channel);
    Q_INVOKABLE void clearOutput(int channel);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    struct
    {
        const QString useSystemProxy{"MainWindow/useSystemProxy"};
        const QString projectName{"MainWindow/projectName"};
        const QString recently{"MainWindow/recently"};
        const QString openPath{"MainWindow/openPath"};

        /// GraphicsView显示相关
        const QString scale{"MainWindow/scale"};
        const QString hBarValue{"MainWindow/hBarValue"};
        const QString vBarValue{"MainWindow/vBarValue"};
        const QString showRuler{"MainWindow/showRuler"};
        const QString showVernier{"MainWindow/showVernier"};
        const QString showGrid{"MainWindow/showGrid"};

        /// DockWidget显示相关
        const QString showDockWidgetNavigator{"MainWindow/showDockWidgetNavigator"};
        const QString showDockWidgetNodes{"MainWindow/showDockWidgetNodes"};
        const QString showDockWidgetOutput{"MainWindow/showDockWidgetOutput"};
        const QString showDockWidgetLog{"MainWindow/showDockWidgetLog"};

        /// DockWidget大小相关
        const QString dockWidgetNavigatorSize{"MainWindow/dockWidgetNavigatorSize"};
        const QString dockWidgetNodesSize{"MainWindow/dockWidgetNodesSize"};
        const QString dockWidgetOutputSize{"MainWindow/dockWidgetOutputSize"};
        const QString dockWidgetLogSize{"MainWindow/dockWidgetLogSize"};

        /// 工具栏显示相关
        const QString showToolBarFile{"MainWindow/showToolBarFile"};
        const QString showToolBarView{"MainWindow/showToolBarView"};
        const QString showToolBarEdit{"MainWindow/showToolBarEdit"};
    } m_settingsKey;

private:
    void init();
    void initFileMenu();
    void initFileMenuRecently(QMenu *fileMenu);
    void initEditMenu();
    void initViewMenu();
    void initHelpMenu();

    void initToolBar();
    void initToolBarFile();
    void initToolBarView();
    void initToolBarViewScale(QToolBar *toolBar);
    void initToolBarEdit();

    void onNewActionTriggered();
    void onOpenActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
    void onScaleChanged(double scale);
    void onAboutActionTriggered();

    // 主题相关接口
    void updateTheme(const QString &theme);
    QJsonObject cookedGraphicsViewStyle(const QJsonObject &style);
    QJsonObject cookedNodeStyle(const QJsonObject &style);
    QJsonObject cookedConnectionStyle(const QJsonObject &style);

    // 配置相关接口
    QString defaultOpenPath();
    void setDefaultOpenPath(const QString &path);
    void addRecentlyFile(const QString &fileName);
    void showPreferenceDialog();

    // 许可相关接口
    bool showTrialLicenseWarning();
    bool isTrialEdition();

    // 其他接口
    void setModified(bool modified);
    void tryToReboot();
    void updateWindowTitle();

private:
    Ui::MainWindow *ui;
    NodeEditor *m_nodeEditor;
    QString m_projectFile;
    bool m_projectModified;
    QLineEdit *m_scaleLineEdit;
    Preference *m_preference;

    // Dock widgets
    OutputDockWidget *m_outputDockWidget;
    NodesDockWidget *m_nodesDockWidget;
    NavigatorDockWidget *m_navigatorDockWidget;
    LogDockWidget *m_logDockWidget;

    // File menu, File toolbar
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QMenu *m_recentlyMenu;
    QAction *m_RecentlyMenuSeparator;
    QActionGroup *m_recentlyActionGroup;

    // Edit menu, Edit toolbar
    QMenu *m_editMenu;

    QToolBar *m_fileToolBar;
    QToolBar *m_viewToolBar;
    QToolBar *m_editToolBar;

#ifndef X_FLOW_STORE
    LicenseManagerUi *m_licenseMgrUi;
#endif
};
