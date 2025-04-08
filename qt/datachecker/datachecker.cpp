/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "datachecker.h"

QString DataChecker::pluginApiVersion() const
{
    return QString("1.0");
}

QString DataChecker::category() const
{
    return tr("Data Checker");
}

QString DataChecker::name() const
{
    return QString("DataChecker");
}

int DataChecker::inPorts() const
{
    return 1;
}

int DataChecker::outPorts() const
{
    return 0;
}

QString DataChecker::inPortNames(int index) const
{
    return QString("[In]");
}

QString DataChecker::outPortNames(int index) const
{
    return QString("[Out]");
}

QByteArray DataChecker::inPortTypes(int index) const
{
    return QByteArray("QByteArray");
}

QByteArray DataChecker::outPortTypes(int index) const { return QByteArray("QByteArray"); }

bool DataChecker::inputBytes(const QByteArray& bytes, int index)
{
    return true;
}

QByteArray DataChecker::outputBytes(int index) const
{
    return QByteArray();
}

QWidget* DataChecker::widget()
{
    return nullptr;
}

QString DataChecker::version() const
{
    return QString("1.0.0");
}

QString DataChecker::description() const
{
    return tr("Data Checker Plugin for xFlow.");
}

QString DataChecker::author() const
{
    return QString("x-tools-author");
}

QString DataChecker::repository() const
{
    return QString("");
}

void DataChecker::translateUi(const QString& flag) { }