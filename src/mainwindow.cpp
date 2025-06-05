/***************************************************************************************************
 * Copyright 2023-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtNodes/AbstractGraphModel>
#include <QtNodes/NodeData>

#include <QActionGroup>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkProxyFactory>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>
#include <QTextBrowser>
#include <QToolBar>
#include <QToolButton>

#ifdef Q_OS_WIN
#ifdef X_MS_STORE
#include "microsoftstore.h"
#endif
#endif

#if !defined(X_FLOW_STORE)
#include "license/licensemanager.h"
#include "license/licensemanagerui.h"
#include "license/payjsapi.h"
#endif

#include "dockwidgets/log/logdockwidget.h"
#include "dockwidgets/navigator/navigatordockwidget.h"
#include "dockwidgets/nodes/nodesdockwidget.h"
#include "dockwidgets/output/outputdockwidget.h"

#include "application.h"
#include "common/common.h"
#include "nodeeditor/nodeeditor.h"
#include "nodeeditor/nodeeditorview.h"
#include "preference/preference.h"
#include "theme/thememanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
    , ui(new Ui::MainWindow)
    , m_preference(nullptr)
{
    ui->setupUi(this);
#if defined(Q_OS_WIN)
    winId();
#endif
    m_nodeEditor = new NodeEditor(ui->widgetHRuler, ui->widgetVRuler, this);
    ui->widgetHRuler->setup(m_nodeEditor, Qt::Horizontal);
    ui->widgetVRuler->setup(m_nodeEditor, Qt::Vertical);
    ui->widgetHRuler->setContentsMargins(0, 0, 0, 0);
    ui->widgetVRuler->setContentsMargins(0, 0, 0, 0);
    ui->nodeEditor->setLayout(new QHBoxLayout());
    ui->nodeEditor->setContentsMargins(0, 0, 0, 0);
    ui->nodeEditor->layout()->setContentsMargins(0, 0, 0, 0);
    ui->nodeEditor->layout()->addWidget(m_nodeEditor);
    if (layout()) {
        layout()->setContentsMargins(0, 0, 0, 0);
    }

    // 数据发生变化时，设置为已修改（标题会加一个‘*’）
    NodeEditorView *view = m_nodeEditor->view();
    connect(m_nodeEditor, &NodeEditor::scaleChanged, this, &MainWindow::onScaleChanged);
    connect(view, &NodeEditorView::sceneLoaded, this, [=, this]() { setModified(false); });
    connect(view, &NodeEditorView::connectionCreated, this, [=, this]() { setModified(true); });
    connect(view, &NodeEditorView::connectionDeleted, this, [=, this]() { setModified(true); });
    connect(view, &NodeEditorView::nodeCreated, this, [=, this]() { setModified(true); });
    connect(view, &NodeEditorView::nodeDeleted, this, [=, this]() { setModified(true); });
    connect(view, &NodeEditorView::nodeUpdated, this, [=, this]() { setModified(true); });
    connect(view, &NodeEditorView::nodePositionUpdated, this, [=, this]() { setModified(true); });
    connect(view, &NodeEditorView::modelReset, this, [=, this]() { setModified(true); });

#if defined(X_MS_STORE)
    connect(&MicrosoftStore::singleton(),
            &MicrosoftStore::licenseStateChangedCallbackFunction,
            this,
            [=, this]() { updateWindowTitle(); });
#endif

    // 重新排序DockWidget位置，顺序
    m_outputDockWidget = new OutputDockWidget(this);
    m_nodesDockWidget = new NodesDockWidget(m_nodeEditor->view(), this);
    m_navigatorDockWidget = new NavigatorDockWidget(m_nodeEditor, this);
    m_logDockWidget = new LogDockWidget(this);

    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_navigatorDockWidget);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_nodesDockWidget);
    addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, m_outputDockWidget);
    addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, m_logDockWidget);

    // 获取上次打开的项目文件名称
    QSettings *settings = xApp->settings();
    m_projectFile = settings->value(m_settingsKey.projectName).toString();
    if (m_projectFile.isEmpty() || !QFile::exists(m_projectFile)) {
        m_projectFile = xApp->settingsPath() + QString("/default_v3.flow");
        qInfo() << "The project file is:" << m_projectFile;
    }

    // 其他一些配置
    updateWindowTitle();
    view->loadProject(m_projectFile);
    setWindowIcon(QIcon(":/res/icons/logo.svg"));
    setObjectName("MainWindow");
    setModified(false);

#if !defined(X_FLOW_STORE)
#if defined(QT_RELEASE)
    PayJsApi::singleton().setPrice(168 * 100);
#else
    PayJsApi::singleton().setPrice(1 * 100);
#endif
    m_licenseMgrUi = new LicenseManagerUi(systemDateFormat(), systemTimeFormat());
    m_licenseMgrUi->setAttribute(Qt::WA_ShowModal, true);

#endif

    // 初始化菜单栏，工具栏
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    NodeEditorView *view = m_nodeEditor->view();
    QSettings *settings = xApp->settings();
    qreal scale = settings->value(m_settingsKey.scale, 1.0).toDouble();
    int hBarValue = settings->value(m_settingsKey.hBarValue, 0).toInt();
    int vBarValue = settings->value(m_settingsKey.vBarValue, 0).toInt();

    m_nodeEditor->setScale(scale);
    m_nodeEditor->horizontalScrollBar()->setValue(hBarValue);
    m_nodeEditor->verticalScrollBar()->setValue(vBarValue);

    /// DockWidget显示状态
#if 1
    bool showNavigator = settings->value(m_settingsKey.showDockWidgetNavigator, true).toBool();
    bool showNodes = settings->value(m_settingsKey.showDockWidgetNodes, true).toBool();
    bool showOutput = settings->value(m_settingsKey.showDockWidgetOutput, true).toBool();
    bool showLog = settings->value(m_settingsKey.showDockWidgetLog, true).toBool();
    m_navigatorDockWidget->setVisible(showNavigator);
    m_nodesDockWidget->setVisible(showNodes);
    m_outputDockWidget->setVisible(showOutput);
    m_logDockWidget->setVisible(showLog);
#endif

    /// DockWidget大小
#if 0
    QSize size = settings->value(m_settingsKey.dockWidgetNavigatorSize, QSize(200, 400)).toSize();
    m_navigatorDockWidget->resize(size);
    size = settings->value(m_settingsKey.dockWidgetNodesSize, QSize(200, 400)).toSize();
    m_nodesDockWidget->resize(size);
    size = settings->value(m_settingsKey.dockWidgetOutputSize, QSize(200, 400)).toSize();
    m_outputDockWidget->resize(size);
    size = settings->value(m_settingsKey.dockWidgetLogSize, QSize(200, 400)).toSize();
    m_logDockWidget->resize(size);
#endif

    /// ToolBar显示状态
    bool showed = settings->value(m_settingsKey.showToolBarFile, true).toBool();
    m_fileToolBar->setVisible(showed);
    showed = settings->value(m_settingsKey.showToolBarEdit, true).toBool();
    m_editToolBar->setVisible(showed);
    showed = settings->value(m_settingsKey.showToolBarView, true).toBool();
    m_viewToolBar->setVisible(showed);

    setModified(false);
    updateWindowTitle();
    centralWidget()->setFocus();

    Application::SettingsKey keys;
    QString theme = xApp->settings()->value(keys.theme, xDarkTheme).toString();
    updateTheme(theme);

    if (!isTrialEdition()) {
        view->loadProject(m_projectFile);
    }
}

void MainWindow::moveToCenter()
{
    QRect screenRect = QApplication::primaryScreen()->geometry();
    bool tooWidth = (width() > screenRect.width());
    bool tooHeight = (height() > screenRect.height());
    if (tooWidth || tooHeight) {
        showMaximized();
        qInfo() << "The screen is too small.";
    } else {
        move((screenRect.width() - width()) / 2, (screenRect.height() - height()) / 2);
    }
}

void MainWindow::outputBytes(const QString &txt, int channel)
{
    m_outputDockWidget->outputBytes(txt, channel);
}

void MainWindow::clearOutput(int channel)
{
    m_outputDockWidget->clearOutput(channel);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    NodeEditorView *view = m_nodeEditor->view();
    QSettings *settings = xApp->settings();

    /// Node Editor相关功能
    qreal scale = view->getScale();
    settings->setValue(m_settingsKey.scale, scale);
    settings->setValue(m_settingsKey.hBarValue, m_nodeEditor->horizontalScrollBar()->value());
    settings->setValue(m_settingsKey.vBarValue, m_nodeEditor->verticalScrollBar()->value());

    /// 保存DockWidget显示状态
    bool checked = m_navigatorDockWidget->toggleViewAction()->isChecked();
    settings->setValue(m_settingsKey.showDockWidgetNavigator, checked);
    checked = m_nodesDockWidget->toggleViewAction()->isChecked();
    settings->setValue(m_settingsKey.showDockWidgetNodes, checked);
    checked = m_outputDockWidget->toggleViewAction()->isChecked();
    settings->setValue(m_settingsKey.showDockWidgetOutput, checked);
    checked = m_logDockWidget->toggleViewAction()->isChecked();
    settings->setValue(m_settingsKey.showDockWidgetLog, checked);

#if 0
    /// 保存DockWidget大小
    settings->setValue(m_settingsKey.dockWidgetNavigatorSize, m_navigatorDockWidget->size());
    settings->setValue(m_settingsKey.dockWidgetNodesSize, m_nodesDockWidget->size());
    settings->setValue(m_settingsKey.dockWidgetOutputSize, m_outputDockWidget->size());
    settings->setValue(m_settingsKey.dockWidgetLogSize, m_logDockWidget->size());
#endif

    /// 保存工具栏显示状态
    settings->setValue(m_settingsKey.showToolBarFile, m_fileToolBar->isVisible());
    settings->setValue(m_settingsKey.showToolBarEdit, m_editToolBar->isVisible());
    settings->setValue(m_settingsKey.showToolBarView, m_viewToolBar->isVisible());
    settings->sync();

    // 保存工程（试用版无法保存）
    if (!isTrialEdition()) {
        onSaveActionTriggered();
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::init()
{
    initFileMenu();
    initEditMenu();

    initToolBar();
    initViewMenu();
    initHelpMenu();
}

void MainWindow::initFileMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    m_newAction = new QAction(tr("&New"));
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setIcon(QIcon(":/res/icons/draft.svg"));
    fileMenu->addAction(m_newAction);
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNewActionTriggered);

    m_openAction = new QAction(tr("&Open"));
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setIcon(QIcon(":/res/icons/file_open.svg"));
    connect(m_openAction, &QAction::triggered, this, [this]() {
        if (!showTrialLicenseWarning()) {
            onOpenActionTriggered();
        }
    });
    fileMenu->addAction(m_openAction);

    m_saveAction = new QAction(tr("&Save"));
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setIcon(QIcon(":/res/icons/save.svg"));
    connect(m_saveAction, &QAction::triggered, this, [this]() {
        if (!showTrialLicenseWarning()) {
            onSaveActionTriggered();
        }
    });
    fileMenu->addAction(m_saveAction);

    m_saveAsAction = new QAction(tr("Save As"));
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setIcon(QIcon(":/res/icons/save_as.svg"));
    connect(m_saveAsAction, &QAction::triggered, this, [this]() {
        if (!showTrialLicenseWarning()) {
            onSaveAsActionTriggered();
        }
    });
    fileMenu->addAction(m_saveAsAction);

    fileMenu->addSeparator();
    initFileMenuRecently(fileMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Preference"), this, &MainWindow::showPreferenceDialog);
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon(":/res/icons/close.svg"), tr("&Exit"), this, &MainWindow::close);
}

void MainWindow::initFileMenuRecently(QMenu *fileMenu)
{
    m_recentlyMenu = new QMenu(tr("Recently Project"));
    fileMenu->addMenu(m_recentlyMenu);
    m_recentlyActionGroup = new QActionGroup(this);
    m_RecentlyMenuSeparator = m_recentlyMenu->addSeparator();
    m_recentlyMenu->addAction(tr("Clear Recently"), this, [=, this]() {
        while (!this->m_recentlyActionGroup->actions().isEmpty()) {
            auto ret = this->m_recentlyActionGroup->actions();
            QAction *action = ret.first();
            m_recentlyMenu->removeAction(action);
            this->m_recentlyActionGroup->removeAction(action);
            action->deleteLater();
        }
    });

    QSettings *settings = xApp->settings();
    auto fileNameList = settings->value(m_settingsKey.recently).toStringList();
    for (QString &fileName : fileNameList) {
        QAction *action = new QAction(fileName);
        m_recentlyActionGroup->addAction(action);
        connect(action, &QAction::triggered, this, [=, this]() {
            if (!QFile::exists(action->text())) {
                return;
            }
            this->m_projectFile = action->text();
            updateWindowTitle();
            NodeEditorView *view = m_nodeEditor->view();
            view->loadProject(m_projectFile);
        });
        m_recentlyMenu->insertAction(m_RecentlyMenuSeparator, action);
    }
}

void MainWindow::initEditMenu()
{
    m_editMenu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(m_editMenu);
}

void MainWindow::initViewMenu()
{
    auto viewMenu = new QMenu(tr("&View"), this);
    QSettings *settings = xApp->settings();
    menuBar()->addMenu(viewMenu);

    /// 读入Dock Widget显示状态
    viewMenu->addAction(m_navigatorDockWidget->toggleViewAction());
    viewMenu->addAction(m_nodesDockWidget->toggleViewAction());
    viewMenu->addAction(m_outputDockWidget->toggleViewAction());
    viewMenu->addAction(m_logDockWidget->toggleViewAction());

    /// 读入工具栏显示状态
    viewMenu->addSeparator();
    viewMenu->addAction(m_fileToolBar->toggleViewAction());
    viewMenu->addAction(m_viewToolBar->toggleViewAction());
    viewMenu->addAction(m_editToolBar->toggleViewAction());
    viewMenu->addSeparator();

    QAction *action = viewMenu->addAction(tr("Show Ruler"));
    action->setCheckable(true);
    bool showRuler = settings->value(m_settingsKey.showRuler, true).toBool();
    action->setChecked(showRuler);
    NodeEditorView *view = m_nodeEditor->view();
    m_nodeEditor->setRulerVisible(showRuler);
    connect(action, &QAction::triggered, this, [=, this](bool checked) {
        settings->setValue(this->m_settingsKey.showRuler, checked);
        m_nodeEditor->setRulerVisible(checked);
    });

    action = viewMenu->addAction(tr("Show Vernier"));
    action->setCheckable(true);
    bool showVernier = settings->value(m_settingsKey.showVernier, true).toBool();
    action->setChecked(showVernier);
    ui->widgetHRuler->setShowVernier(showVernier);
    ui->widgetVRuler->setShowVernier(showVernier);
    connect(action, &QAction::triggered, this, [=, this](bool checked) {
        settings->setValue(this->m_settingsKey.showVernier, checked);
        ui->widgetHRuler->setShowVernier(checked);
        ui->widgetVRuler->setShowVernier(checked);
    });

    action = viewMenu->addAction(tr("Show Grid"));
    action->setCheckable(true);
    bool showGrid = settings->value(m_settingsKey.showGrid, true).toBool();
    action->setChecked(showGrid);
    view->setGridVisible(showGrid);
    connect(action, &QAction::triggered, this, [=, this](bool checked) {
        settings->setValue(this->m_settingsKey.showGrid, checked);
        view->setCacheMode(QGraphicsView::CacheNone);
        view->setGridVisible(checked);
        view->update();
        view->setCacheMode(QGraphicsView::CacheBackground);
    });
}

void MainWindow::initHelpMenu()
{
    auto helpMenu = menuBar()->addMenu(tr("&Help"));
    auto qtAction = helpMenu->addAction(tr("About Qt"), qApp, &QApplication::aboutQt);
    qtAction->setIcon(QIcon(":/res/icons/qt.svg"));
    auto aboutAction = helpMenu->addAction(tr("About") + " " + QApplication::applicationName());
    aboutAction->setIcon(QIcon(":/res/icons/logo.svg"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutActionTriggered);

#if !defined(QT_RELEASE)
    helpMenu->addAction(QIcon(":/res/icons/crop.svg"), tr("Screenshot"), this, [=, this]() {
        QPixmap pix = this->grab();
        // copy to clipboard
        QApplication::clipboard()->setPixmap(pix);
    });
#endif

#if defined(Q_OS_WIN)
    helpMenu->addSeparator();
    helpMenu->addAction(QIcon(":/res/icons/storefront.svg"), tr("Buy from Store"), this, [=, this]() {
#if 0
        QDesktopServices::openUrl(QUrl("https://apps.microsoft.com/detail/9pjk7d83p014"));
#else
        QStringList arguments;
        arguments << QString("start") << QString("/C") << QString("start")
                  << QString("ms-windows-store://pdp/?productid=9pjk7d83p014");
        QProcess::startDetached(QString("cmd"), arguments);
#endif
    });
#endif

    helpMenu->addAction(QIcon(":/res/icons/developer_guide.svg"), tr("Online Manual"), this, []() {
        QDesktopServices::openUrl(QUrl("https://x-tools-author.github.io/x-flow/"));
    });
    helpMenu->addAction(QIcon(":/res/icons/extension.svg"), tr("Plugin Developing"), this, []() {
        QDesktopServices::openUrl(QUrl("https://github.com/x-tools-author/x-flow/"));
    });
    helpMenu->addSeparator();

    // clang-format off
    QList<QPair<QString, QString>> ctxs;
    ctxs.append(qMakePair(QString("glog"), QString("https://github.com/google/glog")));
    ctxs.append(qMakePair(QString("lua"), QString("https://github.com/lua/lua")));
    ctxs.append(qMakePair(QString("nodeeditor"), QString("https://github.com/paceholder/nodeeditor")));
    ctxs.append(qMakePair(QString("qlementine"), QString("https://github.com/oclero/qlementine")));
    ctxs.append(qMakePair(QString("Qt-AES"), QString("https://github.com/bricke/Qt-AES")));
    // clang-format on

#if !defined(X_FLOW_STORE)
    helpMenu->addSeparator();
    helpMenu->addAction(QIcon(":/res/icons/license.svg"), tr("Register"), this, [=, this]() {
        m_licenseMgrUi->show();
        m_licenseMgrUi->raise();
    });
    helpMenu->addSeparator();
#endif

    QMenu *menu = helpMenu->addMenu(tr("Third Party Open Source"));
    menu->menuAction()->setIcon(QIcon(":/res/icons/github.svg"));
    for (auto &ctx : ctxs) {
        menu->addAction(ctx.first, this, [ctx]() { QDesktopServices::openUrl(QUrl(ctx.second)); });
    }
}

void MainWindow::initToolBar()
{
    initToolBarFile();
    initToolBarView();
    initToolBarEdit();
}

void MainWindow::initToolBarFile()
{
    m_fileToolBar = this->addToolBar(tr("File Tool Bar"));
    m_fileToolBar->setMovable(true);
    m_fileToolBar->addAction(m_newAction);
    m_fileToolBar->addAction(m_openAction);
    m_fileToolBar->addAction(m_saveAction);
    m_fileToolBar->addAction(m_saveAsAction);
}

void MainWindow::initToolBarView()
{
    m_viewToolBar = this->addToolBar(tr("View Tool Bar"));
    m_viewToolBar->setMovable(true);
    QIcon resetIcon = QIcon(":/res/icons/restart_alt.svg");
    m_viewToolBar->addAction(resetIcon, tr("Reset Scale"), this, [=, this]() {
        m_nodeEditor->setScale(1.0);
    });

    QIcon zoomOutIcon = QIcon(":/res/icons/zoom_in_map.svg");
    m_viewToolBar->addAction(zoomOutIcon, tr("Zoom Out"), this, [=, this]() {
        m_nodeEditor->zoomOut();
    });

    initToolBarViewScale(m_viewToolBar);

    QIcon zoomInIcon = QIcon(":/res/icons/zoom_out_map.svg");
    m_viewToolBar->addAction(zoomInIcon, tr("Zoom In"), this, [=, this]() {
        m_nodeEditor->zoomIn();
    });
}

void MainWindow::initToolBarViewScale(QToolBar *toolBar)
{
    int cookedScale = m_nodeEditor->scale() * 100;

    m_scaleLineEdit = new QLineEdit(this);
    m_scaleLineEdit->setText(QString::number(cookedScale) + "%");
    m_scaleLineEdit->setAlignment(Qt::AlignCenter);
    m_scaleLineEdit->setFixedWidth(60);
    toolBar->addWidget(m_scaleLineEdit);
    connect(m_scaleLineEdit, &QLineEdit::editingFinished, this, [=, this]() {
        QString txt = m_scaleLineEdit->text();
        // Remove all non-digit characters
        static QRegularExpression re("[^\\d]");
        txt.remove(re);
        qreal scale = txt.toDouble() / 100.0;
        if (scale < NodeEditor::minScale()) {
            scale = NodeEditor::minScale();
        }
        if (scale > NodeEditor::maxScale()) {
            scale = NodeEditor::maxScale();
        }

        m_nodeEditor->setScale(scale);
        m_scaleLineEdit->setText(QString::number(scale * 100) + QString("%"));
    });
}

void MainWindow::initToolBarEdit()
{
    m_editToolBar = new QToolBar(tr("Edit Tool Bar"));
    m_editToolBar->setMovable(true);
    addToolBar(m_editToolBar);

    struct Ctx
    {
        QString icon;
        QString text;
        int type;
    };

    QList<Ctx> ctxList;
    // clang-format off
    ctxList.append(Ctx{"align_vertical_top.svg", tr("Align to Top"), NodeEditorView::AlignTypeTop});
    ctxList.append(Ctx{"align_vertical_bottom.svg", tr("Align to Bottom"), NodeEditorView::AlignTypeBottom});
    ctxList.append(Ctx{"align_horizontal_left.svg", tr("Align to Left"), NodeEditorView::AlignTypeLeft});
    ctxList.append(Ctx{"align_horizontal_right.svg", tr("Align to Right"), NodeEditorView::AlignTypeRight});
    ctxList.append(Ctx{"align_vertical_center.svg", tr("Align to Vertical Center"), NodeEditorView::AlignTypeVCenter});
    ctxList.append(Ctx{"align_horizontal_center.svg", tr("Align to Horizontal Center"), NodeEditorView::AlignTypeHCenter});
    ctxList.append(Ctx{"align_justify_space_even.svg", tr("Align Justify Space Even"), NodeEditorView::AlignTypeHEquidistance});
    ctxList.append(Ctx{"align_space_even.svg", tr("Align Justify Space Even"), NodeEditorView::AlignTypeVEquidistance});
    // clang-format on

    NodeEditorView *view = m_nodeEditor->view();
    for (Ctx const &ctx : ctxList) {
        QString icon = QString(":/res/icons/%1").arg(ctx.icon);
        QAction *action = new QAction(QIcon(icon), ctx.text, this);
        m_editMenu->addAction(action);
        m_editToolBar->addAction(action);
        connect(action, &QAction::triggered, this, [=, this]() { view->alignNodes(ctx.type); });
    }

    m_editToolBar->addSeparator();
    QIcon selectAllNodesIcon = QIcon(":/res/icons/select_all.svg");
    QAction *selectAllNodesAction = new QAction(selectAllNodesIcon, tr("Select All Nodes"), this);
    selectAllNodesAction->setShortcut(QKeySequence::SelectAll);
    m_editToolBar->addAction(selectAllNodesAction);
    m_editMenu->addAction(selectAllNodesAction);
    connect(selectAllNodesAction, &QAction::triggered, this, [=, this]() {
        view->selectAllNodes();
    });

    QIcon deleteSelectedNodesIcon = QIcon(":/res/icons/delete.svg");
    QAction *deleteSelectedNodesAction = new QAction(deleteSelectedNodesIcon,
                                                     tr("Delete Selected Nodes"),
                                                     this);
    m_editToolBar->addAction(deleteSelectedNodesAction);
    m_editMenu->addAction(deleteSelectedNodesAction);
    connect(deleteSelectedNodesAction, &QAction::triggered, this, [=, this]() {
        auto ret = QMessageBox::warning(this,
                                        tr("Delete All Selected Nodes"),
                                        tr("All selected nodes will be delete, are you sure?"),
                                        QMessageBox::No | QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
            view->deleteSelectedNodes();
        }
    });

    QIcon clearNodesIcon = QIcon(":/res/icons/mop.svg");
    QAction *clearNodesAction = new QAction(clearNodesIcon, tr("Clear All Nodes"), this);
    m_editToolBar->addAction(clearNodesAction);
    m_editMenu->addAction(clearNodesAction);
    connect(clearNodesAction, &QAction::triggered, this, [=, this]() {
        auto ret = QMessageBox::warning(this,
                                        tr("Clear All Nodes"),
                                        tr("All nodes will be delete, are you sure?"),
                                        QMessageBox::No | QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
            view->clearAllNodes();
        }
    });
}

void MainWindow::onNewActionTriggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("New Project"),
                                                    defaultOpenPath(),
                                                    tr("Project (*.flow)"));
    if (fileName.isEmpty()) {
        return;
    }

    NodeEditorView *view = m_nodeEditor->view();
    view->clearAllNodes();
    m_projectFile = fileName;
    onSaveActionTriggered();
    setDefaultOpenPath(fileName.left(fileName.lastIndexOf('/')));
}

void MainWindow::onOpenActionTriggered()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Open Flow Scene"),
                                                    defaultOpenPath(),
                                                    tr("Flow Scene Files (*.flow)"));
    if (fileName.isEmpty()) {
        return;
    }

    NodeEditorView *view = m_nodeEditor->view();
    QAction *action = new QAction(fileName);
    m_recentlyActionGroup->addAction(action);
    m_recentlyMenu->insertAction(m_RecentlyMenuSeparator, action);
    m_projectFile = fileName;
    connect(action, &QAction::triggered, this, [=, this]() { view->loadProject(fileName); });
    view->loadProject(fileName);
    setModified(false);
    setDefaultOpenPath(fileName.left(fileName.lastIndexOf('/')));
    addRecentlyFile(fileName);
}

void MainWindow::onSaveActionTriggered()
{
    NodeEditorView *view = m_nodeEditor->view();
    view->saveProject(m_projectFile);
    setModified(false);
}

void MainWindow::onSaveAsActionTriggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Project"),
                                                    defaultOpenPath(),
                                                    tr("Project (*.flow);;All file (*.*)"));
    if (fileName.isEmpty()) {
        return;
    }

    NodeEditorView *view = m_nodeEditor->view();
    view->saveProject(fileName);
    m_projectModified = false;
    m_projectFile = fileName;
    updateWindowTitle();
    xApp->settings()->setValue(m_settingsKey.projectName, fileName);
    setDefaultOpenPath(fileName.left(fileName.lastIndexOf('/')));
    addRecentlyFile(fileName);
}

void MainWindow::onScaleChanged(double scale)
{
    // 保留1位小数
    scale = qRound(scale * 10) / 10.0;

    QString txt = QString::number(scale * 100, 'f', 0) + QString("%");
    m_scaleLineEdit->setText(txt);
    xApp->settings()->setValue(m_settingsKey.scale, scale);
}

void MainWindow::onAboutActionTriggered()
{
    QString buildDateTimeFormat = systemDateFormat();
    buildDateTimeFormat += " ";
    buildDateTimeFormat += systemTimeFormat();
    QString dtString = buildDateTimeString(buildDateTimeFormat);
    QString year = buildDateTimeString("yyyy");

    // X_FLOW_GIT_COMMIT_TIME(Tue Mar 4 11:33:30 2025 +0800) to QDateTime
    // X_FLOW_GIT_COMMIT_TIME is defined in CMakeLists.txt
#if defined(X_FLOW_GIT_COMMIT_TIME)
    QString commitTime = QString(X_FLOW_GIT_COMMIT_TIME);
    commitTime = commitTime.left(commitTime.lastIndexOf(" "));
    QDateTime commitDateTime = QDateTime::fromString(commitTime, "ddd MMM d hh:mm:ss yyyy");
    commitTime = commitDateTime.toString(buildDateTimeFormat);
#else
    QString commitTime = tr("Unknown");
#endif

    QString info;
    info += qApp->applicationName() + " v" + qApp->applicationVersion() + "\n\n";
#ifdef X_FLOW_GIT_COMMIT
    info += tr("Commit hash") + ": " + X_FLOW_GIT_COMMIT + "\n\n";
#endif
    info += tr("Commit time") + ": " + commitTime + "\n\n";
    info += tr("Build Date") + ": " + dtString + "\n\n";
    info += tr("Copyright") + QString(" 2018-%1 x-tools-author(x-tools@outlook.com).").arg(year);
    info += tr("All rights reserved.");
    QMessageBox::about(this, tr("About") + QString(" ") + xApp->applicationName(), info);
}

void MainWindow::updateTheme(const QString &theme)
{
    Application::SettingsKey settingKeys;
    xApp->settings()->setValue(settingKeys.theme, theme);
    xApp->setupTheme();

    auto currentTheme = ThemeManager::singleton().currentTheme();
    m_nodeEditor->setRulerColor(currentTheme.borderColor);
    ui->widgetHRuler->setVernierColor(currentTheme.primaryColor);
    ui->widgetVRuler->setVernierColor(currentTheme.primaryColor);

    QFile file(":/res/files/nodeeditorstyle.json");
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open nodeeditorstyle.json: " << file.errorString();
    }

    QByteArray json = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject style = doc.object();
    style["GraphicsViewStyle"] = cookedGraphicsViewStyle(style["GraphicsViewStyle"].toObject());
    style["NodeStyle"] = cookedNodeStyle(style["NodeStyle"].toObject());
    style["ConnectionStyle"] = cookedConnectionStyle(style["ConnectionStyle"].toObject());

    NodeEditorView *view = m_nodeEditor->view();
    view->setStylesheet(style);
    ThemeManager::singleton().updateWindowTitleArea(this);
}

QJsonObject MainWindow::cookedGraphicsViewStyle(const QJsonObject &style)
{
    QJsonObject graphicsViewStyle = style;
    auto currentTheme = ThemeManager::singleton().currentTheme();
    QColor backgroundColor = currentTheme.backgroundColorMain1;
    QJsonArray rgb;
    rgb.append(backgroundColor.red());
    rgb.append(backgroundColor.green());
    rgb.append(backgroundColor.blue());
    QJsonDocument rgbDoc;
    rgbDoc.setArray(rgb);
    graphicsViewStyle["BackgroundColor"] = rgb;

    QColor fineGridColor = currentTheme.backgroundColorMain4;
    rgb[0] = fineGridColor.red();
    rgb[1] = fineGridColor.green();
    rgb[2] = fineGridColor.blue();
    graphicsViewStyle["FineGridColor"] = rgb;

    QColor coarseGridColor = currentTheme.backgroundColorMain4;
    rgb[0] = coarseGridColor.red();
    rgb[1] = coarseGridColor.green();
    rgb[2] = coarseGridColor.blue();
    graphicsViewStyle["CoarseGridColor"] = rgb;

    return graphicsViewStyle;
}

QJsonObject MainWindow::cookedNodeStyle(const QJsonObject &style)
{
    QJsonObject nodeStyle = style;
    auto currentTheme = ThemeManager::singleton().currentTheme();
    QColor backgroundColor = currentTheme.backgroundColorMain2;
    QJsonArray rgb;
    rgb.append(backgroundColor.red());
    rgb.append(backgroundColor.green());
    rgb.append(backgroundColor.blue());
    nodeStyle["GradientColor0"] = backgroundColor.name();
    nodeStyle["GradientColor1"] = rgb;
    nodeStyle["GradientColor2"] = rgb;
    nodeStyle["GradientColor3"] = rgb;

    QColor normalBoundaryColor = currentTheme.borderColor;
    rgb[0] = normalBoundaryColor.red();
    rgb[1] = normalBoundaryColor.green();
    rgb[2] = normalBoundaryColor.blue();
    nodeStyle["NormalBoundaryColor"] = rgb;

    QColor selectedBoundaryColor = currentTheme.primaryColor;
    rgb[0] = selectedBoundaryColor.red();
    rgb[1] = selectedBoundaryColor.green();
    rgb[2] = selectedBoundaryColor.blue();
    nodeStyle["SelectedBoundaryColor"] = rgb;

    QColor shadowColor = currentTheme.shadowColor1;
    rgb[0] = shadowColor.red();
    rgb[1] = shadowColor.green();
    rgb[2] = shadowColor.blue();
    nodeStyle["ShadowColor"] = rgb;

    QColor filledConnectionPointColor = currentTheme.primaryColor;
    rgb[0] = filledConnectionPointColor.red();
    rgb[1] = filledConnectionPointColor.green();
    rgb[2] = filledConnectionPointColor.blue();
    nodeStyle["FilledConnectionPointColor"] = rgb;

    QColor connectionPointColor = currentTheme.borderColor;
    rgb[0] = connectionPointColor.red();
    rgb[1] = connectionPointColor.green();
    rgb[2] = connectionPointColor.blue();
    nodeStyle["ConnectionPointColor"] = rgb;

    QColor fontColor = currentTheme.secondaryColor;
    nodeStyle["FontColor"] = fontColor.name();

#if 0
    QColor fontColorFaded = currentTheme.secondaryColorDisabled;
    nodeStyle["FontColorFaded"] = fontColorFaded.name();
#endif

    return nodeStyle;
}

QJsonObject MainWindow::cookedConnectionStyle(const QJsonObject &style)
{
    QJsonObject connectionStyle = style;
    auto currentTheme = ThemeManager::singleton().currentTheme();
    QColor normalColor = currentTheme.borderColor;
    QJsonArray rgb;
    rgb.append(normalColor.red());
    rgb.append(normalColor.green());
    rgb.append(normalColor.blue());
    connectionStyle["NormalColor"] = rgb;
    connectionStyle["ConstructionColor"] = rgb;

    QColor selectedColor = currentTheme.primaryColor;
    rgb[0] = selectedColor.red();
    rgb[1] = selectedColor.green();
    rgb[2] = selectedColor.blue();
    connectionStyle["SelectedColor"] = rgb;

    return connectionStyle;
}

QString MainWindow::defaultOpenPath()
{
    auto desktopLocatioin = QStandardPaths::DesktopLocation;
    QString desktopPath = QStandardPaths::writableLocation(desktopLocatioin);

    QSettings *settings = xApp->settings();
    return settings->value(m_settingsKey.openPath, desktopPath).toString();
}

void MainWindow::setDefaultOpenPath(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    xApp->settings()->setValue(m_settingsKey.openPath, path);
}

bool MainWindow::showTrialLicenseWarning()
{
    if (!isTrialEdition()) {
        return false;
    }

    QMessageBox::warning(this,
                         tr("Trial Edition Limitation"),
                         tr("This is a trial edition, please register to use all features."),
                         QMessageBox::Ok);
    return true;
}

bool MainWindow::isTrialEdition()
{
#if !defined(X_FLOW_STORE)
    if (LicenseManager::singleton().isRegistered()) {
        return false;
    }
#else
#if defined(X_MS_STORE)
    if (MicrosoftStore::singleton().isFull()) {
        return false;
    }
#endif
#endif
    return true;
}

void MainWindow::addRecentlyFile(const QString &fileName)
{
    QStringList tmp = xApp->settings()->value(m_settingsKey.recently).toStringList();
    if (!tmp.contains(fileName)) {
        tmp.append(fileName);
    }
    while (tmp.length() > 16) {
        tmp.removeFirst();
    }
    xApp->settings()->setValue(m_settingsKey.recently, tmp);
}

void MainWindow::showPreferenceDialog()
{
    if (m_preference == nullptr) {
        m_preference = new Preference(this);
        connect(m_preference, &Preference::themeChanged, this, &MainWindow::updateTheme);
    }

    ThemeManager::singleton().updateWindowTitleArea(m_preference);
    m_preference->setModal(true);
    m_preference->show();
    m_preference->raise();
    m_preference->exec();
}

void MainWindow::setModified(bool modified)
{
    m_projectModified = modified;
    updateWindowTitle();

    QWidget *w = m_navigatorDockWidget->widget();
    if (!w) {
        return;
    }

    m_navigatorDockWidget->updateNavigator();
}

void MainWindow::tryToReboot()
{
    xApp->tryToReboot();
}

void MainWindow::updateWindowTitle()
{
    const QString trialEdition = tr("Trial Edition");
    QString title = QString("Data Flow Studio");
#if !defined(X_FLOW_STORE)
    if (LicenseManager::singleton().isTrial()) {
        title += QString("(%1)").arg(trialEdition);
    }
#else
#if defined(X_MS_STORE)
    if (!MicrosoftStore::singleton().isFull()) {
        title += QString("(%1)").arg(tr("Trial Edition"));
    }
#endif
#endif
    title += " v";
    title += qApp->applicationVersion();
    title += " ";
    title += m_projectModified ? "*" : " ";
    title += QString(" (%1)").arg(m_projectFile);
    setWindowTitle(title);
}
