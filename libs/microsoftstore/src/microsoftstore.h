/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QObject>

class MicrosoftStore : public QObject
{
    Q_OBJECT
private:
    explicit MicrosoftStore(QObject *parent = nullptr);

public:
    ~MicrosoftStore();
    MicrosoftStore(const MicrosoftStore &) = delete;
    MicrosoftStore &operator=(const MicrosoftStore &) = delete;

    static MicrosoftStore &singleton();
    static int apiVersion();
    static QString apiVersionString();

    bool initialize(void *hwnd);
    bool isValid();
    bool isFull();

    void purchase();
    void getLicenseState();
    void getPrice();

signals:
    void purchaseCallbackFunction(int error, const QString &message);
    void licenseStateChangedCallbackFunction(int error, const QString &message);
    void getPriceCallbackFunction(int error, const QString &message);

private:
    int m_result;
    bool m_isFull{false};
};
