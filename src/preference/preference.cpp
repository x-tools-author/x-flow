/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "preference.h"
#include "ui_preference.h"

#include <QDesktopServices>
#include <QFileDialog>

#include "application.h"
#include "common/common.h"
#include "nodeeditor/plugin/pluginmanager.h"
#include "theme/thememanager.h"

Preference::Preference(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Preference)
{
    ui->setupUi(this);
    winId();
    init();

    connect(ui->pushButtonClose, &QPushButton::clicked, this, &Preference::close);
}

Preference::~Preference()
{
    delete ui;
}

void Preference::saveSettings()
{
    auto keys = Application::SettingsKey();
    xApp->settings()->setValue(keys.hdpi, ui->comboBoxGeneralHdpi->currentData().toInt());
    xApp->settings()->setValue(keys.theme, ui->comboBoxGeneralTheme->currentData().toString());
    xApp->settings()->setValue(keys.language, ui->comboBoxGeneralLanguage->currentData().toString());
    xApp->settings()->setValue(keys.useSystemProxy, ui->checkBoxGeneralUseSystemProxy->isChecked());
}

void Preference::loadSettings()
{
    auto keys = Application::SettingsKey();

    int defaultHdpi = static_cast<int>(xApp->highDpiScaleFactorRoundingPolicy());
    int hdpi = xApp->settings()->value(keys.hdpi, defaultHdpi).toInt();
    QString theme = xApp->settings()->value(keys.theme).toString();
    QString language = xApp->settings()->value(keys.language).toString();
    bool useSystemProxy = xApp->settings()->value(keys.useSystemProxy).toBool();

    ui->comboBoxGeneralHdpi->setCurrentIndex(ui->comboBoxGeneralHdpi->findData(hdpi));
    ui->comboBoxGeneralTheme->setCurrentIndex(ui->comboBoxGeneralTheme->findData(theme));
    ui->comboBoxGeneralLanguage->setCurrentIndex(ui->comboBoxGeneralLanguage->findData(language));
    ui->checkBoxGeneralUseSystemProxy->setChecked(useSystemProxy);
}

void Preference::init()
{
    initGeneralSettings();
    initSettingsSettings();
    initPluginsSettings();
}

void Preference::initGeneralSettings()
{
    initGeneralSettingsHdpiPolicy();
    initGeneralSettingsThemes();
    initGeneralSettingsLanguages();
    initGeneralSettingsUseSystemConfiguration();
}

void Preference::initGeneralSettingsHdpiPolicy()
{
    typedef Qt::HighDpiScaleFactorRoundingPolicy Policy;
    static QMap<Policy, QString> policyMap;
    if (policyMap.isEmpty()) {
        policyMap.insert(Policy::Round, QObject::tr("Round up for .5 and above"));
        policyMap.insert(Policy::Ceil, QObject::tr("Always round up"));
        policyMap.insert(Policy::Floor, QObject::tr("Always round down"));
        policyMap.insert(Policy::RoundPreferFloor, QObject::tr("Round up for .75 and above"));
        policyMap.insert(Policy::PassThrough, QObject::tr("Don't round"));
    }

    ui->comboBoxGeneralHdpi->clear();
    for (auto it = policyMap.begin(); it != policyMap.end(); ++it) {
        QString name = it.value();
        int policy = static_cast<int>(it.key());
        ui->comboBoxGeneralHdpi->addItem(name, policy);
    }

    connect(ui->comboBoxGeneralHdpi, xComboBoxActivated, this, [=, this]() {
        int policy = ui->comboBoxGeneralHdpi->currentData().toInt();
        Application::SettingsKey settingKeys;
        xApp->settings()->setValue(settingKeys.hdpi, policy);
    });

    int policy = static_cast<int>(xApp->highDpiScaleFactorRoundingPolicy());
    int index = ui->comboBoxGeneralHdpi->findData(policy);
    if (index != -1) {
        ui->comboBoxGeneralHdpi->setCurrentIndex(index);
    } else {
        qWarning() << "The policy is not supported:" << policy;
    }
}

void Preference::initGeneralSettingsThemes()
{
    ui->comboBoxGeneralTheme->clear();
    QList<QPair<QString, QString>> themes = ThemeManager::singleton().supportedThemes();
    for (QPair<QString, QString> &theme : themes) {
        ui->comboBoxGeneralTheme->addItem(theme.first, theme.second);
    }

    connect(ui->comboBoxGeneralTheme, xComboBoxActivated, this, [=, this]() {
        QString theme = ui->comboBoxGeneralTheme->currentData().toString();
        Application::SettingsKey settingKeys;
        xApp->settings()->setValue(settingKeys.theme, theme);
        emit themeChanged(theme);
    });

    QString currentTheme = xApp->settings()->value("Application/theme", xDarkTheme).toString();
    int index = ui->comboBoxGeneralTheme->findData(currentTheme);
    if (index != -1) {
        ui->comboBoxGeneralTheme->setCurrentIndex(index);
    } else {
        qWarning() << "The theme is not supported:" << currentTheme;
    }
}

void Preference::initGeneralSettingsLanguages()
{
    ui->comboBoxGeneralLanguage->clear();
    typedef QMap<QString, QString> LanguageMap;
    LanguageMap supportedLanguages = {
        {"en", "English"},
        {"zh_CN", "简体中文"},
    };

    for (auto it = supportedLanguages.begin(); it != supportedLanguages.end(); ++it) {
        QString lang = it.key();
        QString name = it.value();
        ui->comboBoxGeneralLanguage->addItem(name, lang);
    }

    QString defaultLanguage = QLocale::system().name();
    int index = ui->comboBoxGeneralLanguage->findData(defaultLanguage);
    if (index != -1) {
        ui->comboBoxGeneralLanguage->setCurrentIndex(index);
    } else {
        qWarning() << "The language is not supported:" << defaultLanguage;
    }

    connect(ui->comboBoxGeneralLanguage, xComboBoxActivated, this, [=, this]() {
        QString lang = ui->comboBoxGeneralLanguage->currentData().toString();
        xApp->settings()->setValue(Application::SettingsKey().language, lang);
        xApp->tryToReboot();
    });
}

void Preference::initGeneralSettingsUseSystemConfiguration()
{
    auto keys = Application::SettingsKey();
    bool useSystemProxy = xApp->settings()->value(keys.useSystemProxy, false).toBool();
    ui->checkBoxGeneralUseSystemProxy->setChecked(useSystemProxy);
    connect(ui->checkBoxGeneralUseSystemProxy, &QCheckBox::clicked, this, [=, this]() {
        bool checked = ui->checkBoxGeneralUseSystemProxy->isChecked();
        xApp->settings()->setValue(keys.useSystemProxy, checked);
    });
}

void Preference::initSettingsSettings()
{
    connect(ui->pushButtonSettingsOpen, &QPushButton::clicked, this, [=, this]() {
        QDesktopServices::openUrl(xApp->settingsPath());
    });
    connect(ui->checkBoxSettingsClear, &QCheckBox::clicked, this, [=, this]() {
        auto keys = Application::SettingsKey();
        xApp->settings()->setValue(keys.clearSettings, ui->checkBoxSettingsClear->isChecked());
    });
}

void Preference::initPluginsSettings()
{
    connect(ui->pushButtonPluginsBrowse, &QPushButton::clicked, this, [=, this]() {
        QString tmp = ui->lineEditPluginsPath->text();
        QString path = QFileDialog::getExistingDirectory(this, tr("Select Plugins Path"), tmp);
        if (path.isEmpty()) {
            return;
        }

        ui->lineEditPluginsPath->setText(path);
        xApp->settings()->setValue(Application::SettingsKey().pluginsPath, path);
    });

    connect(ui->pushButtonPluginsReset, &QPushButton::clicked, this, [=, this]() {
        QString path = QCoreApplication::applicationDirPath() + "/plugins";
        xApp->settings()->setValue(Application::SettingsKey().pluginsPath, path);
        ui->lineEditPluginsPath->setText(path);
    });

    auto keys = Application::SettingsKey();
    QString defaultPath = QCoreApplication::applicationDirPath() + "/plugins";
    QString pluginsPath = xApp->settings()->value(keys.pluginsPath, defaultPath).toString();
    ui->lineEditPluginsPath->setText(pluginsPath);
    connect(ui->lineEditPluginsPath, &QLineEdit::textChanged, this, [=, this](const QString &path) {
        xApp->settings()->setValue(keys.pluginsPath, path);
    });
    PluginManager::singleton().loadPlugins(pluginsPath);
}