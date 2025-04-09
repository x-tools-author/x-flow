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

    virtual QString category() const = 0;
    virtual QString name() const = 0;
    virtual int inPorts() const = 0;
    virtual int outPorts() const = 0;
    virtual QString inPortNames(int index) const = 0;
    virtual QString outPortNames(int index) const = 0;
    virtual QByteArray inPortTypes(int index) const = 0;
    virtual QByteArray outPortTypes(int index) const = 0;
    virtual bool inputBytes(const QByteArray& bytes, int index) = 0;
    virtual QByteArray outputBytes(int index) const = 0;
    virtual QWidget* widget() = 0;

    // Optional: This function is used to get the plugin version.
    virtual QString version() const { return QString(""); }
    virtual QString description() const { return QString(""); }
    virtual QString author() const { return QString(""); }
    virtual QString repository() const { return QString(""); }
    virtual void setLanguage(const QString& flag) {}

    virtual QJsonObject save() const { return QJsonObject(); }
    virtual void load(const QJsonObject& parameters) { Q_UNUSED(parameters); }
};

QT_BEGIN_NAMESPACE
#define XFlowPluginInterfaceV1_iid "xtools.xflow.XFlowPluginInterfaceV1"
Q_DECLARE_INTERFACE(XFlowPluginInterfaceV1, XFlowPluginInterfaceV1_iid)
QT_END_NAMESPACE