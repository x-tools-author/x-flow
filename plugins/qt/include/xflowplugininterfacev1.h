/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QWidget>

class XFlowPluginInterfaceV1
{
public:
    virtual QString pluginApiVersion() const = 0;

    virtual QString caption() const = 0;
    virtual QString name() const = 0;
    virtual int inPorts() const = 0;
    virtual int outPorts() const = 0;
    virtual QWidget *newWidget() = 0;
    virtual QByteArray handleData(QByteArray const &data, int const index, QWidget *ui) = 0;

    virtual QJsonObject save(QWidget *ui) const { return QJsonObject(); }
    virtual void load(const QJsonObject &parameters, QWidget *ui) { Q_UNUSED(parameters); }
    virtual void setLanguage(const QString &language) { Q_UNUSED(language); }
};

QT_BEGIN_NAMESPACE
#define XFlowPluginInterfaceV1_iid "xtools.xflow.XFlowPluginInterfaceV1"
Q_DECLARE_INTERFACE(XFlowPluginInterfaceV1, XFlowPluginInterfaceV1_iid)
QT_END_NAMESPACE