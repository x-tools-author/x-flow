/***************************************************************************************************
 * Copyright 2023-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QWidget>

namespace Ui {
class LicenseManagerUi;
}

class PayJsApi;
class LicenseManager;
class LicenseManagerUi : public QWidget
{
    Q_OBJECT
public:
    explicit LicenseManagerUi(const QString &sysDateFormat,
                              const QString &sysTimeFormat,
                              QWidget *parent = nullptr);
    ~LicenseManagerUi();

    void setTabIndex(int index);
    void updateParameters();

private:
    Ui::LicenseManagerUi *ui;
    LicenseManager *m_licenseManager;
    PayJsApi *m_payJsApi;
    const QString m_sysDateFormat;
    const QString m_sysTimeFormat;

private:
    void updateParametersOffline();
    void updateParametersOnline();
    bool showSomeTipsThenNext();

private slots:
    void onRegisterPushButtonClicked();
    void onLicenseStateChanged();
    void onRefreshPushButtonClicked();
    void onQrCodeDownloaded(const QImage &qrCode, const QString &orderId);
    void onExportPushButtonClicked();
    void onImportPushButtonClicked();
    void onCopyLicenseCodePushButtonClicked();
};
