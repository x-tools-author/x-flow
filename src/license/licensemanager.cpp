/***************************************************************************************************
 * Copyright 2023-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "licensemanager.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

#include "payjsapi.h"

LicenseManager::LicenseManager(QObject *parent)
    : QObject(parent)
{
    m_productId = qApp->applicationName();
    m_payJsApi = &PayJsApi::singleton();
    connect(m_payJsApi, &PayJsApi::orderPayed, this, &LicenseManager::onOrderPayed);
    connect(m_payJsApi, &PayJsApi::qrCodeDownloaded, this, &LicenseManager::onQrCodeDownloaded);

    m_licenseState = static_cast<int>(LicenseState::Unregistered);
    m_licenseFile = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    m_licenseFile += QString("/%1.license").arg(m_productId);
    m_aes = new QAESEncryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    m_key = QCryptographicHash::hash(m_productId.toLatin1(), QCryptographicHash::Sha256);
    m_iv = QCryptographicHash::hash(m_productId.toLatin1(), QCryptographicHash::Md5);

    QString licenseCode = this->licenseCode();
    if (!licenseCode.isEmpty()) {
        m_licenseState = verifyLicense(licenseCode);
    }
}

LicenseManager &LicenseManager::singleton()
{
    static LicenseManager instance;
    return instance;
}

QString LicenseManager::machineCode()
{
    QJsonObject ctx;
    ctx.insert(m_jsonKeys.machineId, machineUuid());
    ctx.insert(m_jsonKeys.productId, m_productId);

    return encode(ctx);
}

QString LicenseManager::licenseCode()
{
    QFile file(m_licenseFile);
    if (!file.open(QFile::ReadOnly)) {
        return QByteArray();
    }

    QByteArray data = file.readAll();
    file.close();
    return QString::fromLatin1(data.toHex());
}

QString LicenseManager::generateLicenseCode(const QString &machineCode, const QString &orderId)
{
    QJsonArray purchaseRecords;
    purchaseRecords.append(QDateTime::currentSecsSinceEpoch());

    QJsonObject ctx;
    ctx.insert(m_jsonKeys.machineCode, machineCode);
    ctx.insert(m_jsonKeys.purchaseRecords, purchaseRecords);
    ctx.insert(m_jsonKeys.orderId, orderId);

    return encode(ctx);
}

QString LicenseManager::getOrderID()
{
    QString key = licenseCode();
    QJsonObject ctx = decode(key);
    return ctx.value("orderId").toString();
}

QJsonArray LicenseManager::getPurchaseRecords()
{
    QString key = licenseCode();
    QJsonObject ctx = decode(key);
    return ctx.value("purchaseRecords").toArray();
}

QString LicenseManager::productId() const
{
    return m_productId;
}

void LicenseManager::setProductId(const QString &productId)
{
    m_productId = productId;
}

void LicenseManager::setIvKey(const QString &productId)
{
    const QByteArray ivKey = productId.toLatin1();
    m_key = QCryptographicHash::hash(ivKey, QCryptographicHash::Sha256);
    m_iv = QCryptographicHash::hash(ivKey, QCryptographicHash::Md5);
}

void LicenseManager::setEndOfTrialCallback(std::function<void()> callback)
{
    m_endOfTrialCallback = callback;
}

void LicenseManager::importLicense()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Import License"),
                                                    "",
                                                    tr("License Files (*.license)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open license file!";
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    QString licenseCode = QString::fromLatin1(data.toHex());
    registerApp(licenseCode);
}

void LicenseManager::exportLicense()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    tr("Export License"),
                                                    "",
                                                    tr("License Files (*.license)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "Failed to open license file:" << file.errorString();
        return;
    }

    file.write(QByteArray::fromHex(licenseCode().toLatin1()));
    file.close();
}

void LicenseManager::registerApp(const QString &licenseCode)
{
    m_licenseState = verifyLicense(licenseCode);
    if (m_licenseState == static_cast<int>(LicenseState::Registered)) {
        QFile file(m_licenseFile);
        if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
            qWarning() << "Failed to write license key to file!";
            return;
        }

        QByteArray data = QByteArray::fromHex(licenseCode.toLatin1());
        file.write(data);
        file.close();
        emit licenseStateChanged(m_licenseState);
    }
}

void LicenseManager::clearLicense()
{
    int ret
        = QMessageBox::warning(nullptr,
                               tr("Unregister"),
                               QString(
                                   "The license information will be cleared, please make sure you "
                                   "have backed up the license file, do you want to continue?"),
                               QMessageBox::Yes | QMessageBox::Cancel);
    if (ret == QMessageBox::Yes && QFile::remove(m_licenseFile)) {
        qInfo() << "The license file has been removed!";
        m_licenseState = static_cast<int>(LicenseState::Unregistered);
        emit licenseStateChanged(m_licenseState);
    }
}

int LicenseManager::licenseState()
{
    return m_licenseState;
}

bool LicenseManager::isRegistered() const
{
    return m_licenseState == static_cast<int>(LicenseState::Registered);
}

bool LicenseManager::isTrial()
{
    return m_licenseState != static_cast<int>(LicenseState::Registered);
}

QString LicenseManager::machineUuid()
{
#ifdef Q_OS_WIN
    // wmic csproduct get UUID
    QString program{"wmic"};
    QStringList arguments{"csproduct", "get", "UUID"};
    QProcess process;
    process.start(program, arguments);
    process.waitForFinished();
    QString str = process.readAllStandardOutput();
    QStringList tmpList = str.split("\r\r\n", Qt::SkipEmptyParts);
    if (tmpList.isEmpty()) {
        return QString{"00000000-0000-0000-0000-000000000000"};
    } else {
        return tmpList.last().trimmed();
    }
#else
    return QString("00000000-0000-0000-0000-000000000000");
#endif
}

QString LicenseManager::encode(const QJsonObject &jsonObject)
{
    QJsonDocument doc;
    doc.setObject(jsonObject);
    QByteArray json = doc.toJson(QJsonDocument::Compact);
    QByteArray encodeData = m_aes->encode(json, m_key, m_iv);
    return QString::fromLatin1(encodeData.toHex()).toUpper();
}

QJsonObject LicenseManager::decode(const QString &license)
{
    if (license.isEmpty()) {
        return QJsonObject();
    }

    QByteArray encodeData = QByteArray::fromHex(license.toLatin1());
    QByteArray decodeData = m_aes->decode(encodeData, m_key, m_iv);
    decodeData = m_aes->removePadding(decodeData);

    QJsonDocument doc = QJsonDocument::fromJson(decodeData);
    return doc.object();
}

LicenseManager::LicenseCodeContext LicenseManager::decodeLicenseCode(const QString &licenseCode)
{
    LicenseCodeContext ctx;
    ctx.isValid = false;
    QJsonObject obj = decode(licenseCode);
    if (obj.isEmpty()) {
        qWarning() << "The license is invalid.";
        return ctx;
    }

    QString machineCode = obj.value(m_jsonKeys.machineCode).toString();
    QJsonObject machineCodeObj = decode(machineCode);
    if (machineCodeObj.isEmpty()) {
        qWarning() << "The machine code is invalid.";
        return ctx;
    }

    ctx.isValid = true;
    ctx.machineCodeContext.machineId = machineCodeObj.value(m_jsonKeys.machineId).toString();
    ctx.machineCodeContext.productId = machineCodeObj.value(m_jsonKeys.productId).toString();
    ctx.orderId = obj.value(m_jsonKeys.orderId).toString();
    QJsonArray array = obj.value(m_jsonKeys.purchaseRecords).toArray();
    for (auto &&i : array) {
        ctx.purchaseRecords.append(i.toInt());
    }

    return ctx;
}

int LicenseManager::verifyLicense(const QString &license)
{
    if (license.isEmpty()) {
        qWarning() << "The license is empty.";
        return static_cast<int>(LicenseState::Unregistered);
    }

    QJsonObject ctx = decode(license);
    QString machineCode = ctx.value("machineCode").toString();
    QJsonArray purchaseRecords = ctx.value("purchaseRecords").toArray();

    if (machineCode != this->machineCode()) {
        qWarning() << "The machine code is not match.";
        return static_cast<int>(LicenseState::Unregistered);
    }

    for (auto &&i : purchaseRecords) {
        const qint64 purchaseRecord = i.toInt();
        const QDateTime dateTime = QDateTime::fromSecsSinceEpoch(purchaseRecord);
        if (dateTime.addYears(m_periodOfValidity) >= QDateTime::currentDateTime()) {
            return static_cast<int>(LicenseState::Registered);
        }
    }

    qWarning() << "The license is expired.";
    return static_cast<int>(LicenseState::Expired);
}

void LicenseManager::callEndOfTrialCallback()
{
    if (m_licenseState == static_cast<int>(LicenseState::Registered)) {
        return;
    }

    if (m_endOfTrialCallback) {
        m_endOfTrialCallback();
    } else {
        QMessageBox::warning(nullptr,
                             tr("End of Trial"),
                             tr("The trial period has expired, the application will be closed."));
        QApplication::closeAllWindows();
        QApplication::quit();
    }
}

void LicenseManager::onQrCodeDownloaded(const QImage &qrCode, const QString &orderId) {}

void LicenseManager::onOrderPayed(const QString &orderId)
{
    const QString machineCode = this->machineCode();
    const QString licenseCode = generateLicenseCode(machineCode, orderId);
    registerApp(licenseCode);
}
