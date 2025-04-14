/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "datachecker.h"
#include "datacheckerui.h"

DataChecker::DataChecker(QObject* parent)
    : QObject(parent)
{}

DataChecker::~DataChecker() {}

QString DataChecker::pluginApiVersion() const
{
    return QString("1.0");
}

QString DataChecker::caption() const
{
    return QString("Data Checker");
}

QString DataChecker::name() const
{
    return QString("xtools.xflow.plugin.datachecker");
}

int DataChecker::inPorts() const
{
    return 1;
}

int DataChecker::outPorts() const
{
    return 0;
}

QByteArray DataChecker::handleData(QByteArray const& bytes, int const index, QWidget* ui)
{
    DataCheckerUi* dataCheckerUi = qobject_cast<DataCheckerUi*>(ui);
    if (dataCheckerUi) {
        QString reference = dataCheckerUi->referenceText();
        QString input = QString(bytes.toHex(' ').toUpper());

        if (input == reference) {
            dataCheckerUi->setResultText(QString("OK"));
        } else {
            dataCheckerUi->setResultText(QString("NG"));
        }
    }

    return QByteArray();
}

QWidget* DataChecker::newWidget()
{
    return new DataCheckerUi();
}

QJsonObject DataChecker::save(QWidget* ui) const
{
    QJsonObject jsonObj;

    DataCheckerUi* dataCheckerUi = qobject_cast<DataCheckerUi*>(ui);
    if (dataCheckerUi) {
        jsonObj.insert("reference", dataCheckerUi->referenceText());
    }

    return jsonObj;
}

void DataChecker::load(const QJsonObject& parameters, QWidget* ui)
{
    DataCheckerUi* dataCheckerUi = qobject_cast<DataCheckerUi*>(ui);
    if (dataCheckerUi) {
        QString defaultReference = QString("28 6E 75 6C 6C 29");
        QString reference = parameters.value("reference").toString(defaultReference);
        dataCheckerUi->setReferenceText(reference.trimmed());
    }
}
