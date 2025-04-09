/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "datachecker.h"
#include "ui_datachecker.h"

DataChecker::DataChecker(QObject* parent)
    : QObject(parent)
    , ui(nullptr)
    , m_widget(nullptr)
{}

DataChecker::~DataChecker()
{
    if (ui) {
        delete ui;
    }
}

QString DataChecker::pluginApiVersion() const
{
    return QString("1.0");
}

QString DataChecker::category() const
{
    return getText(QString("Data Checker"));
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

QByteArray DataChecker::outPortTypes(int index) const
{
    return QByteArray("QByteArray");
}

bool DataChecker::inputBytes(const QByteArray& bytes, int index)
{
    if (ui) {
        QString str = ui->lineEdit->text().trimmed();
        QByteArray reference = str.toUtf8().toHex().toUpper();
        QByteArray input = bytes.toHex().toUpper();

        if (input == reference) {
            ui->labelStatus->setText(QString("OK"));
        } else {
            ui->labelStatus->setText(QString("NG"));
        }
    }

    return false;
}

QByteArray DataChecker::outputBytes(int index) const
{
    return QByteArray("");
}

QWidget* DataChecker::widget()
{
    if (m_widget == nullptr) {
        m_widget = new QWidget();
        ui = new Ui::DataChecker;
        ui->setupUi(m_widget);
    }

    return m_widget;
}

QString DataChecker::version() const
{
    return QString("1.0.0");
}

QString DataChecker::description() const
{
    return QString("Data Checker Plugin for xFlow.");
}

QString DataChecker::author() const
{
    return QString("x-tools-author");
}

QString DataChecker::repository() const
{
    return QString("https://github.com/x-tools-author/x-flow");
}

void DataChecker::setLanguage(const QString& flag)
{
    m_languageFlag = flag;
    if (ui == nullptr) {
        return;
    }

    ui->labelReference->setText(getText("Reference data"));
    ui->labelResult->setText(getText("Checking result"));
}

QJsonObject DataChecker::save() const
{
    QJsonObject jsonObj;

    if (ui) {
        jsonObj["reference"] = ui->lineEdit->text().trimmed();
    } else {
        jsonObj["reference"] = QString("28 6E 75 6C 6C 29");
    }

    return jsonObj;
}

void DataChecker::load(const QJsonObject& parameters)
{
    if (ui) {
        QString str = parameters.value("reference").toString(QString("28 6E 75 6C 6C 29"));
        ui->lineEdit->setText(str.trimmed());
    }
}

QString DataChecker::getText(const QString& str) const
{
    if (str == QString("Data Checker")) {
        if (m_languageFlag == QString("en")) {
            return QString("Data Checker");
        } else if (m_languageFlag == QString("zh_CN")) {
            return QString("数据校验器");
        }
    }

    if (str == QString("Reference data")) {
        if (m_languageFlag == QString("en")) {
            return QString("Reference data");
        } else if (m_languageFlag == QString("zh_CN")) {
            return QString("参考数据");
        }
    }

    if (str == QString("Checking result")) {
        if (m_languageFlag == QString("en")) {
            return QString("Checking result");
        } else if (m_languageFlag == QString("zh_CN")) {
            return QString("校验结果");
        }
    }

    return str;
}