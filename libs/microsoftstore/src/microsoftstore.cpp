/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "MicrosoftStore.h"

#include <QCoreApplication>

#include "WindowsStore.h"

using namespace WinRT;

HINSTANCE gDllHandle = nullptr;
WindowsStorePtr gStorePtr = nullptr;
WindowsStoreFreeFunc gWindowsStoreFreeFunc = nullptr;
WindowsStoreGetPriceFunc gWindowsStoreGetPriceFunc = nullptr;
WindowsStorePurchaseFunc gWindowsStorePurchaseFunc = nullptr;
WindowsStoreInitializeFunc gWindowsStoreInitializeFunc = nullptr;
WindowsStoreLicenseStateFunc gWindowsStoreLicenseStateFunc = nullptr;

void storePurchaseCallback(int error, const wchar_t* message, void* userData)
{
    const QString msg = QString::fromWCharArray(message);
    qInfo() << "storePurchaseCallback: error: " << error << " message: " << msg;
    emit MicrosoftStore::singleton().purchaseCallbackFunction(error, msg);
}

void storeLicenseStateChangedCallback(int error, const wchar_t* message, void* userData)
{
    // IsTrial, Full, inactive
    const QString msg = QString::fromWCharArray(message);
    qInfo() << "storeLicenseStateChangedCallback: error: " << error << " message: " << msg;
    emit MicrosoftStore::singleton().licenseStateChangedCallbackFunction(error, msg);
}

void storeLicenseStateCallback(int error, const wchar_t* message, void* userData)
{
    const QString msg = QString::fromWCharArray(message);
    qInfo() << "storeLicenseStateCallback: error: " << error << " message: " << msg;
    emit MicrosoftStore::singleton().licenseStateChangedCallbackFunction(error, msg);
}

void storeGetPriceCallback(int error, const wchar_t* message, void* userData)
{
    const QString msg = QString::fromWCharArray(message);
    qInfo() << "storeGetPriceCallback: error: " << error << " message: " << msg;
    emit MicrosoftStore::singleton().getPriceCallbackFunction(error, msg);
}

MicrosoftStore::MicrosoftStore(QObject* parent)
    : QObject(parent)
    , m_result(WINRT_NO_ERROR)
{
    connect(this,
            &MicrosoftStore::licenseStateChangedCallbackFunction,
            this,
            [=, this](int error, const QString& message) {
                Q_UNUSED(error);
                if (message == QString("Full") && m_result == WINRT_NO_ERROR) {
                    this->m_isFull = true;
                } else {
                    this->m_isFull = false;
                }
            });
}

MicrosoftStore::~MicrosoftStore()
{
    if (gStorePtr) {
        gWindowsStoreFreeFunc(gStorePtr);
    }

    if (gDllHandle) {
        FreeLibrary(gDllHandle);
    }
}

MicrosoftStore &MicrosoftStore::singleton()
{
    static MicrosoftStore instance;
    return instance;
}

int MicrosoftStore::apiVersion()
{
    QString version = apiVersionString();
    QStringList parts = version.split(QLatin1Char('.'));
    if (parts.size() != 3)
        return -1;

    quint32 major = parts.at(0).toInt();
    quint32 minor = parts.at(1).toInt();
    quint32 patch = parts.at(2).toInt();
    quint32 rawVersion = (major << 16) | (minor << 8) | patch;
    return rawVersion;
}

QString MicrosoftStore::apiVersionString()
{
    return QStringLiteral("1.0.0");
}

bool MicrosoftStore::initialize(void* hwnd)
{
    HINSTANCE dllHandle = nullptr;
    QString tmp = QCoreApplication::applicationDirPath();
    tmp += QStringLiteral("/WindowsStoreDLL.dll");
    dllHandle = LoadLibrary(tmp.toStdWString().c_str());
    Q_ASSERT_X(dllHandle, Q_FUNC_INFO, "Failed to load WindowsStore.dll");
    qInfo() << "Load WindowsStore.dll" << (dllHandle ? "success" : "failed");

    WindowsStoreErrorType result = WINRT_NO_ERROR;

    gWindowsStoreInitializeFunc = reinterpret_cast<WindowsStoreInitializeFunc>(
        ::GetProcAddress(dllHandle, "store_initialize"));
    gWindowsStorePurchaseFunc = reinterpret_cast<WindowsStorePurchaseFunc>(
        ::GetProcAddress(dllHandle, "store_purchase"));
    gWindowsStoreLicenseStateFunc = reinterpret_cast<WindowsStoreLicenseStateFunc>(
        ::GetProcAddress(dllHandle, "store_license_state"));
    gWindowsStoreGetPriceFunc = reinterpret_cast<WindowsStoreGetPriceFunc>(
        ::GetProcAddress(dllHandle, "store_get_price"));
    gWindowsStoreFreeFunc = reinterpret_cast<WindowsStoreFreeFunc>(
        ::GetProcAddress(dllHandle, "store_free"));

    // initialize Windows Store functionality
    result = gWindowsStoreInitializeFunc(&gStorePtr,
                                         reinterpret_cast<HWND>(hwnd),
                                         storeLicenseStateChangedCallback,
                                         nullptr);
    if (result != WINRT_NO_ERROR) {
        switch (result) {
        case WINRT_WINDOWS_RUNTIME_ERROR:
            qWarning() << "Unable to initialize Windows Runtime.";
            break;

        case WINRT_WINDOWS_VERSION_ERROR:
            qWarning() << "This version of Windows does not support Windows::Services::Store.";
            break;

        case WINRT_APP_PACKAGE_ERROR:
            qWarning() << "This app is not running inside of an App Package.";
            break;

        default:
            qWarning() << "Unable to initialize Windows Store.";
            break;
        }
    }

    m_result = result;
    return result == WINRT_NO_ERROR;
}

bool MicrosoftStore::isValid()
{
    return m_result == WINRT_NO_ERROR;
}

bool MicrosoftStore::isFull()
{
    return m_isFull;
}

void MicrosoftStore::purchase()
{
    gWindowsStorePurchaseFunc(gStorePtr, storePurchaseCallback, (void*) this);
}

void MicrosoftStore::getLicenseState()
{
    gWindowsStoreLicenseStateFunc(gStorePtr, storeLicenseStateCallback, (void*) this);
}

void MicrosoftStore::getPrice()
{
    gWindowsStoreGetPriceFunc(gStorePtr, storeGetPriceCallback, (void*) this);
}
