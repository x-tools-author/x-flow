/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>

#include <QObject>
#include <QWidget>

#define xDarkTheme QString("Dark")
#define xLightTheme QString("Light")

class ThemeManager : public QObject
{
    Q_OBJECT
private:
    ThemeManager(QObject *parent = nullptr);
    ThemeManager(const ThemeManager &) = delete;
    ThemeManager &operator=(const ThemeManager &) = delete;

public:
    ~ThemeManager();

    static ThemeManager &singleton();

    QList<QPair<QString, QString>> supportedThemes() const;
    const oclero::qlementine::Theme &currentTheme() const;
    void setCurrentTheme(const QString &themeName);
    void updateWindowTitleArea(QWidget *widget);

private:
    oclero::qlementine::QlementineStyle *m_style;
    oclero::qlementine::ThemeManager *m_themeManager;
};
