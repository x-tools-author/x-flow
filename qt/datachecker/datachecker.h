/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QObject>
#include <QtPlugin>

#include "xflowplugininterfacev1.h"

namespace Ui {
class DataChecker;
}

class DataChecker : public QObject, XFlowPluginInterfaceV1
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID XFlowPluginInterfaceV1_iid FILE "datachecker.json")
    Q_INTERFACES(XFlowPluginInterfaceV1)
public:
    DataChecker(QObject* parent = nullptr);
    ~DataChecker() override;

public:
    QString pluginApiVersion() const override;

    QString caption() const override;
    QString name() const override;
    int inPorts() const override;
    int outPorts() const override;
    QWidget* widget() override;
    QByteArray handleData(QByteArray const& bytes, int const index) override;

    QString version() const override;
    QString description() const override;
    QString author() const override;
    QString repository() const override;
    void setLanguage(const QString& flag) override;

    QJsonObject save() const override;
    void load(const QJsonObject& parameters) override;

private:
    QString getText(const QString& str) const;

private:
    Ui::DataChecker* ui;
    QWidget* m_widget;
    QString m_languageFlag;
};
