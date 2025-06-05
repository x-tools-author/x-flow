﻿/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "device.h"

#include <QDebug>

Device::Device(QObject *parent)
    : QThread(parent)
{}

Device::~Device()
{
    if (isRunning()) {
        closeDevice();
    }
}

void Device::openDevice()
{
    if (isRunning()) {
        closeDevice();
    }

    start();
}

void Device::closeDevice()
{
    exit();
    wait();
}

void Device::writeBytes(const QByteArray &bytes)
{
    if (isRunning()) {
        emit invokeWriteBytes(bytes);
    }
}

QJsonObject Device::save() const
{
    m_parametersMutex.lock();
    QJsonObject tmp = m_parameters;
    m_parametersMutex.unlock();

    return tmp;
}

void Device::load(const QJsonObject &parameters)
{
    m_parametersMutex.lock();
    m_parameters = parameters;
    m_parametersMutex.unlock();
}

void Device::run()
{
    QObject *obj = initDevice();
    if (!obj) {
        emit closed();
        return;
    }

    connect(this, &Device::invokeWriteBytes, obj, [this](const QByteArray &bytes) {
        writeActually(bytes);
    });

    emit opened();
    exec();

    deinitDevice();
    emit closed();
}
