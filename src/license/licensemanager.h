/***************************************************************************************************
 * Copyright 2023-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QJsonArray>
#include <QObject>

#include <qaesencryption.h>

class PayJsApi;
class LicenseManager : public QObject
{
    Q_OBJECT
public:
    enum class LicenseState { Registered, Expired, Unregistered };
    Q_ENUM(LicenseState)

private:
    explicit LicenseManager(QObject *parent = nullptr);

public:
    LicenseManager(const LicenseManager &) = delete;
    LicenseManager &operator=(const LicenseManager &) = delete;

    static LicenseManager &singleton();

    QString machineCode();
    QString licenseCode();
    QString generateLicenseCode(const QString &machineCode, const QString &orderId = QString(""));
    QString getOrderID();
    QJsonArray getPurchaseRecords();
    QString productId() const;
    void setProductId(const QString &productId);

    void setIvKey(const QString &productId);
    void setEndOfTrialCallback(std::function<void()> callback);

    void importLicense();
    void exportLicense();
    void registerApp(const QString &licenseCode);
    void clearLicense();
    void setPeriodOfValidity(int year = 100);

    int licenseState();
    bool isRegistered() const;
    bool isTrial();

signals:
    void licenseStateChanged(int licenseState);
    void errorOccurred(const QString &msg);

private:
    struct MachineCodeContext
    {
        QString machineId;
        QString productId;
    };
    struct LicenseCodeContext
    {
        MachineCodeContext machineCodeContext;
        QList<qint64> purchaseRecords;
        QString orderId;
        bool isValid;
    };
    struct LicenseCodeContextJsonKeys
    {
        const QString machineCode{"machineCode"};
        const QString machineId{"machineId"};
        const QString productId{"productId"};
        const QString purchaseRecords{"purchaseRecords"};
        const QString orderId{"orderId"};
    } m_jsonKeys;

private:
    QAESEncryption *m_aes;
    QByteArray m_key;
    QByteArray m_iv;
    QString m_productId;
    QString m_licenseFile;
    int m_licenseState;
    int m_periodOfValidity{100};
    std::function<void()> m_endOfTrialCallback{nullptr};
    PayJsApi *m_payJsApi;

private:
    QString machineUuid();
    QString encode(const QJsonObject &jsonObject);
    QJsonObject decode(const QString &license);
    LicenseCodeContext decodeLicenseCode(const QString &licenseCode);
    int verifyLicense(const QString &license);
    void callEndOfTrialCallback();

private slots:
    void onQrCodeDownloaded(const QImage &qrCode, const QString &orderId);
    void onOrderPayed(const QString &orderId);
};
