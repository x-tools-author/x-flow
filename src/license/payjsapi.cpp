/***************************************************************************************************
 * Copyright 2020-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "payjsapi.h"

#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QMovie>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QVariant>

PayJsApi::PayJsApi(QObject* parent)
    : QThread(parent)
    , m_networkAccessManager(Q_NULLPTR)
{}

PayJsApi::~PayJsApi()
{
    m_networkAccessManager = Q_NULLPTR;
    if (isRunning()) {
        exit();
        wait();
    }
}

PayJsApi& PayJsApi::singleton()
{
    static PayJsApi instance;
    return instance;
}

int PayJsApi::price() const
{
    return m_price;
}

void PayJsApi::setPrice(int price)
{
    m_price = price;
}

void PayJsApi::run()
{
    qInfo() << "Start to generate order.";

    QNetworkAccessManager* netMgr = new QNetworkAccessManager;
    m_networkAccessManager = netMgr;
    QNetworkRequest networkRequest(QUrl("https://payjs.cn/api/native"));
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    // Check the order state.
    QTimer* checkerOrderStateTimer = new QTimer;
    checkerOrderStateTimer->setSingleShot(true);
    checkerOrderStateTimer->setInterval(2 * 1000);
    connect(
        checkerOrderStateTimer,
        &QTimer::timeout,
        this,
        [&]() { checkOrderState(m_orderId); },
        Qt::DirectConnection);
    connect(
        this,
        &PayJsApi::invokeCheckOrderState,
        netMgr,
        [=]() { checkerOrderStateTimer->start(); },
        Qt::QueuedConnection);

    // Generate order. https://help.payjs.cn/api/native.html
    QJsonObject jsonObj;
    jsonObj.insert(QString("mchid"), QString("1602508490"));
    jsonObj.insert(QString("total_fee"), QString::number(m_price));
    jsonObj.insert(QString("out_trade_no"), QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    jsonObj = payJsSign(m_communicationKey, jsonObj);
    auto reply = netMgr->post(networkRequest, QJsonDocument(jsonObj).toJson(QJsonDocument::Compact));
    if (reply) {
        QObject::connect(
            reply,
            &QNetworkReply::finished,
            this,
            [=]() {
                if (reply->error() == QNetworkReply::NoError) {
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
                    QJsonObject jsonObj = jsonDoc.object();
                    const QString returnMsg = jsonObj.value("return_msg").toString();
                    if (returnMsg == QString("SUCCESS")) {
                        m_orderId = jsonObj.value("payjs_order_id").toString();
                        const QUrl url = QUrl(jsonObj.value("qrcode").toString());
                        downloadQrCode(url);
                        emit invokeCheckOrderState();
                    } else {
                        qInfo() << "error: " << returnMsg;
                        emit errorOccurred(returnMsg);
                    }
                } else {
                    qDebug() << reply->errorString();
                }
            },
            Qt::DirectConnection);
    } else {
        emit errorOccurred(QString("Post message error!"));
    }

    exec();
}

void PayJsApi::downloadQrCode(const QUrl& url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    auto reply = m_networkAccessManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();
        QImage qrCode;
        qrCode.loadFromData(reinterpret_cast<uchar*>(data.data()), data.length());
        emit qrCodeDownloaded(qrCode, m_orderId);
    });

    connect(reply,
            QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this,
            [=]() { emit errorOccurred(reply->errorString()); });
}

void PayJsApi::checkOrderState(const QString& orderId)
{
    QNetworkRequest networkRequest(QUrl("https://payjs.cn/api/check"));
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    QJsonObject jsonObj;
    jsonObj.insert(QString("payjs_order_id"), orderId);
    jsonObj = payJsSign(m_communicationKey, jsonObj);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    auto reply = m_networkAccessManager->post(networkRequest,
                                              jsonDoc.toJson(QJsonDocument::Compact));
    if (reply) {
        QObject::connect(reply, &QNetworkReply::finished, this, [=]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.value("status").toInt()) {
                    emit orderPayed(m_orderId);
                } else {
                    emit invokeCheckOrderState();
                }
            } else {
                emit errorOccurred(reply->errorString());
            }
        });
    }
}

QJsonObject PayJsApi::payJsSign(QString communicationKey,
                                QJsonObject payJsRequestJsonWithoutSignSection)
{
    // https://help.payjs.cn/api/sign.html
    QStringList keys = payJsRequestJsonWithoutSignSection.keys();
    QString signRawData;
    for (char ch = 0; ch < CHAR_MAX; ch++) {
        for (int i = 0; i < keys.count(); i++) {
            const QString key = keys.at(i);
            if (key.at(0).unicode() == ch) {
                if (!signRawData.isEmpty()) {
                    signRawData.append("&");
                }

                signRawData.append(key);
                signRawData.append("=");
                if (payJsRequestJsonWithoutSignSection.value(key).type() == QJsonValue::String) {
                    signRawData.append(payJsRequestJsonWithoutSignSection.value(key).toString());
                } else {
                    signRawData.append(
                        QString::number(payJsRequestJsonWithoutSignSection.value(key).toInt()));
                }
            }
        }
    }

    signRawData.append("&key=").append(communicationKey);
    QString sign = QString::fromLatin1(
        QCryptographicHash::hash(signRawData.toLatin1(), QCryptographicHash::Md5).toHex());
    sign = sign.toUpper();
    payJsRequestJsonWithoutSignSection.insert("sign", sign);
    return payJsRequestJsonWithoutSignSection;
}
