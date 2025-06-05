/***************************************************************************************************
 * Copyright 2023-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "licensemanagerui.h"
#include "ui_licensemanagerui.h"

#include <QApplication>
#include <QClipboard>
#include <QMessageBox>

#include "licensemanager.h"
#include "payjsapi.h"

LicenseManagerUi::LicenseManagerUi(const QString &sysDateFormat,
                                   const QString &sysTimeFormat,
                                   QWidget *parent)
    : QWidget{parent}
    , ui{new Ui::LicenseManagerUi}
    , m_sysDateFormat{sysDateFormat}
    , m_sysTimeFormat{sysTimeFormat}
{
    ui->setupUi(this);
    m_licenseManager = &LicenseManager::singleton();
    m_payJsApi = &PayJsApi::singleton();

    connect(m_licenseManager,
            &LicenseManager::licenseStateChanged,
            this,
            &LicenseManagerUi::onLicenseStateChanged);
    connect(ui->pushButtonCopy, &QPushButton::clicked, this, [=]() {
        qApp->clipboard()->setText(ui->textBrowserMachineCode->toPlainText());
    });
    connect(ui->pushButtonPaste, &QPushButton::clicked, this, [=, this]() {
        const QString text = qApp->clipboard()->text();
        ui->textBrowserLicenseCode->setText(text);
    });
    connect(ui->pushButtonRegister,
            &QPushButton::clicked,
            this,
            &LicenseManagerUi::onRegisterPushButtonClicked);
    connect(m_payJsApi, &PayJsApi::qrCodeDownloaded, this, &LicenseManagerUi::onQrCodeDownloaded);
    connect(ui->pushButtonRefresh,
            &QPushButton::clicked,
            this,
            &LicenseManagerUi::onRefreshPushButtonClicked);
    connect(ui->pushButtonExportLicense,
            &QPushButton::clicked,
            this,
            &LicenseManagerUi::onExportPushButtonClicked);
    connect(ui->pushButtonImportLicense,
            &QPushButton::clicked,
            this,
            &LicenseManagerUi::onImportPushButtonClicked);
    connect(ui->pushButtonLicenseCode,
            &QPushButton::clicked,
            this,
            &LicenseManagerUi::onCopyLicenseCodePushButtonClicked);

    updateParameters();
}

LicenseManagerUi::~LicenseManagerUi()
{
    delete ui;
}

void LicenseManagerUi::setTabIndex(int index)
{
    if (index >= 0 && index < ui->tabWidget->count()) {
        ui->tabWidget->setCurrentIndex(index);
    }

    updateParameters();
}

void LicenseManagerUi::updateParameters()
{
    updateParametersOffline();
    updateParametersOnline();
}

void LicenseManagerUi::updateParametersOffline()
{
    ui->textBrowserMachineCode->setText(m_licenseManager->machineCode());
    ui->textBrowserLicenseCode->setText(m_licenseManager->licenseCode());
}

void LicenseManagerUi::updateParametersOnline()
{
    if (m_licenseManager->isRegistered()) {
        ui->labelPriceInfo->setStyleSheet("color:green;");
        ui->labelPriceInfo->setText(tr("Software Registered"));
        ui->labelOrderQrCode->setPixmap(QPixmap(":/res/images/license.png"));
    } else {
        ui->labelPriceInfo->setStyleSheet("color:red;");
        ui->labelPriceInfo->setText(tr("Software not Registered"));
        ui->labelOrderQrCode->setPixmap(QPixmap(":/res/images/unlicense.png"));
    }
}

bool LicenseManagerUi::showSomeTipsThenNext()
{
    int state = m_licenseManager->licenseState();
    if (state == static_cast<int>(LicenseManager::LicenseState::Registered)) {
        int ret = QMessageBox::information(
            this,
            tr("Software Registered"),
            tr("The software has been registered. Please backup the original license. Do you "
               "want to continue the registration?"),
            QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            return false;
        }
    } else if (state == static_cast<int>(LicenseManager::LicenseState::Expired)) {
        int ret = QMessageBox::information(
            this,
            tr("Have Expired"),
            tr("The original license file will be overwritten, please note the backup, do you "
               "want to continue the registration?"),
            QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            return false;
        }
    }

    return true;
}

void LicenseManagerUi::onRegisterPushButtonClicked()
{
    if (showSomeTipsThenNext()) {
        QString licenseKey = ui->textBrowserLicenseCode->toPlainText();
        m_licenseManager->registerApp(licenseKey);
    }
}

void LicenseManagerUi::onLicenseStateChanged()
{
    if (isVisible()) {
        if (m_licenseManager->isRegistered()) {
            QMessageBox::information(this,
                                     tr("Registered Successfully"),
                                     tr("After the registration is successful, you can export "
                                        "the license file on the Export License page."));
        } else {
            QMessageBox::information(this,
                                     tr("Cancel Registration"),
                                     tr("Cancel registration successfully, function limited, "
                                        "please re-register or import license file."));
        }
    }

    updateParameters();
}

void LicenseManagerUi::onRefreshPushButtonClicked()
{
    if (showSomeTipsThenNext()) {
        if (!m_payJsApi->isRunning()) {
            m_payJsApi->start();
        }
    }
}

void LicenseManagerUi::onQrCodeDownloaded(const QImage &qrCode, const QString &orderId)
{
    int price = m_payJsApi->price();
    const QString priceString = QString::number(price / 100.0, 'f', 2);
    QString priceInfo = tr("Order: %1, Price: %2CNY").arg(orderId, priceString);
    ui->labelOrderQrCode->setPixmap(QPixmap::fromImage(qrCode));
    ui->labelPriceInfo->setText(priceInfo);
}

void LicenseManagerUi::onExportPushButtonClicked()
{
    m_licenseManager->exportLicense();
}

void LicenseManagerUi::onImportPushButtonClicked()
{
    m_licenseManager->importLicense();
}

void LicenseManagerUi::onCopyLicenseCodePushButtonClicked()
{
    qApp->clipboard()->setText(ui->textBrowserLicenseCode->toPlainText());
}