/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "xflowpluginchecker.h"
#include "ui_xflowpluginchecker.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfoList>
#include <QFunctionPointer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLibrary>
#include <QMessageBox>

xFlowPluginChecker::xFlowPluginChecker(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::xFlowPluginChecker)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("qrc:/res/icons/logo.svg"));

    connect(ui->pushButtonBrowse,
            &QPushButton::clicked,
            this,
            &xFlowPluginChecker::onBrowseButtonClicked);
    connect(ui->pushButtonCheck,
            &QPushButton::clicked,
            this,
            &xFlowPluginChecker::onCheckButtonClicked);
}

xFlowPluginChecker::~xFlowPluginChecker()
{
    delete ui;
}

void xFlowPluginChecker::onBrowseButtonClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Select Plugin File"), QString());
    if (!fileName.isEmpty()) {
        ui->lineEdit->setText(fileName);
    }
}

void xFlowPluginChecker::onCheckButtonClicked()
{
    ui->textBrowser->clear();

    if (ui->lineEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a plugin directory first."));
        return;
    }

    QString pluginRootPath = ui->lineEdit->text();
    if (!checkDirectory(pluginRootPath)) {
        return;
    }

    PluginInfo info;
    if (!checkPluginInfo(pluginRootPath, info)) {
        return;
    }
}

bool xFlowPluginChecker::checkDirectory(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        return false;
    }

    QStringList infos = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    if (infos.contains("conf") && infos.contains("lib")) {
        return true;
    }

    outputText(tr("The path must have 'conf' and 'lib' directories."), QtWarningMsg);
    return false;
}

bool xFlowPluginChecker::checkPluginInfo(const QString &pluginRootPath, PluginInfo &info)
{
    QString fileName = pluginRootPath + QString("/conf/conf.json");
    if (!QFile::exists(fileName)) {
        outputText(tr("The file(%1) does not exist.").arg(fileName), QtWarningMsg);
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QString errStr = file.errorString();
        outputText(tr("Failed to open the file(%1): %2").arg(fileName, errStr), QtWarningMsg);
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        QString errStr = jsonError.errorString();
        outputText(tr("Failed to parse the file(%1): %2").arg(fileName, errStr), QtWarningMsg);
        return false;
    }

    if (!doc.isObject()) {
        outputText(tr("The file(%1) is not a valid JSON object.").arg(fileName), QtWarningMsg);
        return false;
    }

    const QString canNotBeEmpty = tr("Can not be empty");
    const QString pass = tr("Pass");
    info = loadPluginInfo(doc.object());
    // clang-format off
    outputText(tr("Plugin ID:          %1[%2]").arg(info.id, info.id.isEmpty() ? canNotBeEmpty :pass), info.id.isEmpty() ? QtWarningMsg : QtInfoMsg);
    outputText(tr("Plugin API Prefix:  %1[%2]").arg(info.apiPrefix, info.id.isEmpty() ? canNotBeEmpty :pass), info.apiPrefix.isEmpty() ? QtWarningMsg : QtInfoMsg);
    outputText(tr("Plugin Category:    %1[%2]").arg(info.category, info.id.isEmpty() ? canNotBeEmpty :pass), info.category.isEmpty() ? QtWarningMsg : QtInfoMsg);
    outputText(tr("Plugin Name:        %1[%2]").arg(info.name, info.id.isEmpty() ? canNotBeEmpty :pass), info.name.isEmpty() ? QtWarningMsg : QtInfoMsg);
    outputText(tr("Plugin Version:     %1[%2]").arg(info.version, info.id.isEmpty() ? canNotBeEmpty :pass), info.version.isEmpty() ? QtWarningMsg : QtInfoMsg);
    outputText(tr("Plugin API Version: %1[%2]").arg(info.apiVersion, info.id.isEmpty() ? canNotBeEmpty :pass), info.apiVersion.isEmpty() ? QtWarningMsg : QtInfoMsg);
    outputText(tr("Plugin Lib Name:    %1[%2]").arg(info.libName, info.id.isEmpty() ? canNotBeEmpty :pass), info.libName.isEmpty() ? QtWarningMsg : QtInfoMsg);
    outputText(tr("Plugin Author:      %1[%2]").arg(info.author, pass), info.author.isEmpty() ? QtDebugMsg : QtInfoMsg);
    outputText(tr("Plugin Repository:  %1[%2]").arg(info.repository, pass), info.repository.isEmpty() ? QtDebugMsg : QtInfoMsg);
    outputText(tr("Plugin Description: %1[%2]").arg(info.description, pass), info.description.isEmpty() ? QtDebugMsg : QtInfoMsg);
    // clang-format on

    if (info.id.isEmpty() || info.apiPrefix.isEmpty() || info.category.isEmpty()
        || info.name.isEmpty() || info.version.isEmpty() || info.apiVersion.isEmpty()) {
        return false;
    }

    return checkPluginApi(pluginRootPath, info);
}

bool xFlowPluginChecker::checkPluginApi(const QString &pluginRootPath, PluginInfo &info)
{
    QString libPath = pluginRootPath + QString("/lib/");
    QDir dir(libPath);
    if (!dir.exists()) {
        outputText(tr("The directory(%1) does not exist.").arg(libPath), QtWarningMsg);
        return false;
    }

    QStringList libs = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
    if (libs.isEmpty()) {
        outputText(tr("The directory(%1) does not have any files.").arg(libPath), QtWarningMsg);
        return false;
    }

    if (libs.size() > 1) {
        outputText(tr("The directory(%1) has more than one file.").arg(libPath), QtWarningMsg);
        return false;
    }

    QString libFile = libPath + libs.first();
    outputText(tr("The lib path is: %1").arg(libFile), QtDebugMsg);

    QString apiName = QString("%1_%2").arg(info.apiPrefix, QString("malloc_parameters"));
    typedef void *(*MallocParametersFunc)();
    const char *api = apiName.toLatin1().constData();
    QLibrary lib(libFile);
    auto mallocParametersFunc = (MallocParametersFunc) lib.resolve(api);
    if (!mallocParametersFunc) {
        outputText(tr("Failed to resolve the function(%1):%2").arg(apiName, lib.errorString()),
                   QtWarningMsg);
        return false;
    }

    void *params = mallocParametersFunc();
    if (!params) {
        outputText(tr("Failed to create the parameters."), QtWarningMsg);
        return false;
    }

    typedef void (*DeleteParameters)(void *);
    apiName = QString("%1_%2").arg(info.apiPrefix, QString("free_parameters"));
    api = apiName.toLatin1().constData();
    auto freeParametersFunc = (DeleteParameters) QLibrary::resolve(libFile, api);
    if (!freeParametersFunc) {
        outputText(tr("Failed to resolve the function(%1).").arg(apiName), QtWarningMsg);
        return false;
    }

    freeParametersFunc(params);
    outputText(tr("The plugin(%1) is valid.").arg(info.name), QtInfoMsg);
    return true;
}

void xFlowPluginChecker::outputText(const QString &text, int msgType = QtDebugMsg)
{
    // QtWarningMsg: red, QtInfoMsg: green, other: black
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString color = "black";
    if (msgType == QtWarningMsg) {
        color = "red";
    } else if (msgType == QtInfoMsg) {
        color = "green";
    }

    QString msg = QString("<font color=silver>[%1]</font><font color=%2> %3</font>")
                      .arg(time)
                      .arg(color)
                      .arg(text);
    ui->textBrowser->append(msg);
}

PluginInfo xFlowPluginChecker::loadPluginInfo(const QJsonObject &conf)
{
    PluginInfo info;
    PluginInfoKeys keys;

    info.id = conf.value(keys.id).toString();
    info.apiPrefix = conf.value(keys.apiPrefix).toString();
    info.category = conf.value(keys.category).toString();
    info.name = conf.value(keys.name).toString();
    info.version = conf.value(keys.version).toString();
    info.apiVersion = conf.value(keys.apiVersion).toString();
    info.author = conf.value(keys.author).toString();
    info.repository = conf.value(keys.repository).toString();
    info.description = conf.value(keys.description).toString();
    info.libName = conf.value(keys.libName).toString();

    return info;
}