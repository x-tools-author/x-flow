/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QWidget>

namespace Ui {
class DataCheckerUi;
}

class DataCheckerUi : public QWidget
{
    Q_OBJECT
public:
    DataCheckerUi(QWidget* parent = nullptr);
    ~DataCheckerUi() override;

    QString referenceText();
    void setReferenceText(const QString& text);
    void setResultText(const QString& text);
    void updateLanguage(const QString& language);

private:
    Ui::DataCheckerUi* ui;

    Q_SIGNAL void setResultTextThreadSafe(const QString& text);
};
