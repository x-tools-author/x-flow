/***************************************************************************************************
 * Copyright 2020-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QCryptographicHash>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QThread>
#include <QTimer>

class PayJsApi : public QThread
{
    Q_OBJECT
public:
    PayJsApi(QObject *parent = Q_NULLPTR);

public:
    ~PayJsApi();
    PayJsApi(const PayJsApi &) = delete;
    PayJsApi &operator=(const PayJsApi &) = delete;

    static PayJsApi &singleton();

    int price() const;
    void setPrice(int price);

protected:
    void run() override;

private:
    int m_price{1};                                       // 价格，单位为分
#ifdef X_MACHINE_ID                                       // 商户号
    const QString m_machineId{X_MACHINE_ID};
#else
    const QString m_machineId{"0123456789"};
#endif
#ifdef X_COMMUNICATION_KEY // 通信秘钥
    const QString m_communicationKey{X_COMMUNICATION_KEY};
#else
    const QString m_communicationKey{"abcdefghijklmnoh"};
#endif
    QNetworkAccessManager *m_networkAccessManager;
    QString m_orderId;

private:
    void downloadQrCode(const QUrl &url);
    void checkOrderState(const QString &orderId);
    QJsonObject payJsSign(QString communicationKey, QJsonObject payJsRequestJsonWithoutSignSection);

private:
    Q_SIGNAL void invokeCheckOrderState();

signals:
    void errorOccurred(QString msg);
    void qrCodeDownloaded(const QImage &qrCode, const QString &orderId);
    void orderPayed(QString orderId);
};
