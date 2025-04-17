/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "bypass.h"

Bypass::Bypass(QObject* parent)
    : QObject(parent)
{}

Bypass::~Bypass() {}

QString Bypass::pluginApiVersion() const
{
    return QString("1.0");
}

QString Bypass::caption() const
{
    return QString("Bypass(Qt)");
}

QString Bypass::name() const
{
    return QString("x-tools.x-flow.plugin.bypass.qt");
}

int Bypass::inPorts() const
{
    return 1;
}

int Bypass::outPorts() const
{
    return 1;
}

QByteArray Bypass::handleData(QByteArray const& bytes, int const index, QWidget* ui)
{
    Q_UNUSED(ui);
    return bytes;
}

QWidget* Bypass::newWidget()
{
    return nullptr;
}

QJsonObject Bypass::save(QWidget* ui) const
{
    QJsonObject jsonObj;

    return jsonObj;
}

void Bypass::load(const QJsonObject& parameters, QWidget* ui)
{
    Q_UNUSED(ui);
    Q_UNUSED(parameters);
}
