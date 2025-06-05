/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "thememanager.h"

#include <QApplication>

#if defined(_MSC_VER)
#include <dwmapi.h>

#include <QColor>
#include <QSysInfo>
#include <QWindow>
#endif

#ifdef Q_OS_MACOS
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#endif

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_style(nullptr)
{
    m_style = new oclero::qlementine::QlementineStyle();
    m_style->setAnimationsEnabled(true);
    m_style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
    qApp->setStyle(m_style);

    QIcon::setThemeName("qlementine");
    m_themeManager = new oclero::qlementine::ThemeManager(m_style);
    m_themeManager->loadDirectory(":/res/themes");
}

ThemeManager::~ThemeManager() {}

ThemeManager &ThemeManager::singleton()
{
    static ThemeManager instance;
    return instance;
}

QList<QPair<QString, QString>> ThemeManager::supportedThemes() const
{
    QList<QPair<QString, QString>> themes;
    themes.append(qMakePair(tr("Light"), QString("Light")));
    themes.append(qMakePair(tr("Dark"), QString("Dark")));
    return themes;
}

const oclero::qlementine::Theme &ThemeManager::currentTheme() const
{
    return m_style->theme();
}

void ThemeManager::setCurrentTheme(const QString &themeName)
{
    m_themeManager->setCurrentTheme(themeName);
}

void ThemeManager::updateWindowTitleArea(QWidget *widget)
{
#if defined(_MSC_VER)
    if (QSysInfo::productVersion().contains("11")) {
        auto currentTheme = ThemeManager::singleton().currentTheme();
        // Change color of the caption
        //const DWORD attribute = 35; // DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR
        QWindow *window = widget->windowHandle();
        if (window) {
            QColor c = currentTheme.backgroundColorMain1;
            COLORREF colorref = c.red() | (c.green() << 8) | (c.blue() << 16);
            DwmSetWindowAttribute((HWND) window->winId(),
                                  DWMWA_CAPTION_COLOR,
                                  &colorref,
                                  sizeof(colorref));
#if 0
            c = currentTheme.primaryColor;
            colorref = c.red() | (c.green() << 8) | (c.blue() << 16);
            DwmSetWindowAttribute((HWND) window->winId(),
                                  DWMWA_BORDER_COLOR,
                                  &colorref,
                                  sizeof(colorref));
#endif
        }
    }
#endif

#if defined(Q_OS_LINUX)
    // Nothing to do yet...
#endif

#if defined(Q_OS_MACOS)
    NSView *view = reinterpret_cast<NSView *>(winId());
    NSWindow *w = [view window];

    [w setStyleMask:([w styleMask] | NSWindowStyleMaskFullSizeContentView)];
    [w setTitlebarAppearsTransparent:YES];
    [w setTitleVisibility:NSWindowTitleHidden];

    NSButton *zoomButton = [w standardWindowButton:NSWindowZoomButton];
    [zoomButton setEnabled:YES];
#endif
}