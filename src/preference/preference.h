/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QDialog>

namespace Ui {
class Preference;
}

class Preference : public QDialog
{
    Q_OBJECT
public:
    explicit Preference(QWidget *parent = nullptr);
    ~Preference() override;

    void saveSettings();
    void loadSettings();

signals:
    void themeChanged(const QString &theme);

private:
    Ui::Preference *ui;

private:
    void init();
    void initGeneralSettings();
    void initGeneralSettingsHdpiPolicy();
    void initGeneralSettingsThemes();
    void initGeneralSettingsLanguages();
    void initGeneralSettingsUseSystemConfiguration();
    void initSettingsSettings();
    void initPluginsSettings();
};