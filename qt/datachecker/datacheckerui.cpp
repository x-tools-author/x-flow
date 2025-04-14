/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "datacheckerui.h"
#include "ui_datacheckerui.h"

DataCheckerUi::DataCheckerUi(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DataCheckerUi)
{
    ui->setupUi(this);
}

DataCheckerUi::~DataCheckerUi() {}

QString DataCheckerUi::referenceText()
{
    QString str = ui->lineEdit->text().trimmed();
    if (str.isEmpty()) {
        str = ui->lineEdit->placeholderText().trimmed();
    }

    return str.toUpper();
}

void DataCheckerUi::setReferenceText(const QString& text)
{
    ui->lineEdit->setText(text);
}

void DataCheckerUi::setResultText(const QString& text)
{
    ui->labelStatus->setText(text);
}