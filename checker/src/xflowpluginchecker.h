/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QDialog>

namespace Ui {
class xFlowPluginChecker;
};

struct PluginInfoKeys
{
    const QString id{"plugin-id"};
    const QString apiPrefix{"plugin-api-prefix"};
    const QString category{"plugin-category"};
    const QString name{"plugin-name"};
    const QString version{"plugin-version"};
    const QString apiVersion{"plugin-api-version"};
    const QString libName{"plugin-lib-name"};
    const QString author{"plugin-author"};
    const QString repository{"plugin-repository"};
    const QString description{"plugin-description"};
    const QString parameters{"plugin-parameters"};
};

struct PluginInfo
{
    QString id;
    QString apiPrefix;
    QString category;
    QString name;
    QString version;
    QString apiVersion;
    QString libName;
    QString author;
    QString repository;
    QString description;
};

class xFlowPluginChecker : public QDialog
{
    Q_OBJECT
public:
    explicit xFlowPluginChecker(QWidget *parent = nullptr);
    ~xFlowPluginChecker() override;

private:
    void onBrowseButtonClicked();
    void onCheckButtonClicked();

    bool checkDirectory(const QString &dirPath);
    bool checkPluginInfo(const QString &pluginRootPath, PluginInfo &info);
    bool checkPluginApi(const QString &pluginRootPath, PluginInfo &info);
    void outputText(const QString &text, int msgType);
    PluginInfo loadPluginInfo(const QJsonObject &conf);

private:
    Ui::xFlowPluginChecker *ui;
};