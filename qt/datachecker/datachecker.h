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

#include "xflowplugininterface.h"

namespace Ui {
class DataChecker;
}

class DataChecker : public QObject, XFlowPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID XFlowPluginInterface_iid FILE "datachecker.json")
    Q_INTERFACES(XFlowPluginInterface)
public:
    DataChecker(QObject* parent = nullptr);
    ~DataChecker() override;

public:
    QString pluginApiVersion() const override;

    QString category() const override;
    QString name() const override;
    int inPorts() const override;
    int outPorts() const override;
    QString inPortNames(int index) const override;
    QString outPortNames(int index) const override;
    QByteArray inPortTypes(int index) const override;
    QByteArray outPortTypes(int index) const override;
    bool inputBytes(const QByteArray& bytes, int index) override;
    QByteArray outputBytes(int index) const override;
    QWidget* widget() override;

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
