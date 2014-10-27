/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <glib.h>
#include <QJSValueList>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QTimer>

#include "lunaserviceadapter.h"

namespace luna
{

class LunaServiceHandle
{
public:
    LunaServiceHandle(LSHandle *handle) : mHandle(handle), mRefCount(0) { }

    LSHandle* handle() const
    {
        return mHandle;
    }

    void ref()
    {
        mRefCount++;
    }

    bool unref()
    {
        mRefCount--;
        if (mRefCount == 0) {
            LSError error;
            LSErrorInit(&error);

            if (!LSUnregister(mHandle, &error)) {
                qWarning("Failed to unregister service: %s", error.message);
                LSErrorFree(&error);
            }

            return false;
        }

        return true;
    }

private:
    LSHandle *mHandle;
    unsigned int mRefCount;
};

static QMap<QString, LunaServiceHandle*> serviceHandles;

LunaServiceMessage::LunaServiceMessage(LSMessage *message, QObject *parent) :
    QObject(parent),
    mMessage(message)
{
    LSMessageRef(mMessage);
}

LunaServiceMessage::~LunaServiceMessage()
{
    LSMessageUnref(mMessage);
}

QString LunaServiceMessage::payload() const
{
    return QString(LSMessageGetPayload(mMessage));
}

QString LunaServiceMessage::applicationId() const
{
    return QString(LSMessageGetApplicationID(mMessage));
}

LSMessage* LunaServiceMessage::messageObject() const
{
    return mMessage;
}

LunaServiceCall::LunaServiceCall(QObject *parent) :
    QObject(parent),
    mServiceHandle(0),
    mToken(LSMESSAGE_TOKEN_INVALID),
    mResponseLimit(-1),
    mResponseCount(0)
{
}

LunaServiceCall::~LunaServiceCall()
{
    if (mToken != LSMESSAGE_TOKEN_INVALID)
        cancel();
}

void LunaServiceCall::setup(LSHandle *serviceHandle, QJSValue callback, QJSValue errorCallback, int responseLimit)
{
    mServiceHandle = serviceHandle;
    mCallback = callback;
    mErrorCallback = errorCallback;
    mResponseLimit = responseLimit;
}

bool LunaServiceCall::execute(const QString& uri, const QString& arguments)
{
    LSError error;
    LSErrorInit(&error);
    QString errorMessage;

    if (mResponseLimit == 1) {
        if (!LSCallOneReply(mServiceHandle, uri.toUtf8().constData(), arguments.toUtf8().constData(),
                            &LunaServiceCall::responseCallback, this, &mToken, &error)) {
            qWarning("Failed to call remote service %s", uri.toUtf8().constData());
            errorMessage = QString("Failed to call remote service: %0").arg(error.message);
            goto error;
        }
    }
    else {
        if (!LSCall(mServiceHandle, uri.toUtf8().constData(), arguments.toUtf8().constData(),
                            &LunaServiceCall::responseCallback, this, &mToken, &error)) {
            qWarning("Failed to call remote service %s", uri.toUtf8().constData());
            errorMessage = QString("Failed to call remote service: %0").arg(error.message);
            goto error;
        }
    }

    return true;

error:
    if (LSErrorIsSet(&error)) {
        LSErrorPrint(&error, stderr);
        LSErrorFree(&error);
    }

    if (mErrorCallback.isCallable())
        mErrorCallback.call(QJSValueList() << errorMessage);

    return false;
}

bool LunaServiceCall::responseCallback(LSHandle *handle, LSMessage *message, void *user_data)
{
    LunaServiceCall *call = static_cast<LunaServiceCall*>(user_data);
    return call->handleResponse(handle, message);
}

bool LunaServiceCall::handleResponse(LSHandle *handle, LSMessage *message)
{
    LunaServiceMessage msg(message);
    mResponseCount++;

    if (mCallback.isCallable()) {
        QQmlContext *context = QQmlEngine::contextForObject(parent());
        mCallback.call(QJSValueList() << context->engine()->newQObject(&msg));
    }

    const char* category = LSMessageGetCategory(message);
    bool messageInErrorCategory = (category && strcmp(LUNABUS_ERROR_CATEGORY, category) == 0);

    if (messageInErrorCategory || (mResponseLimit != -1 && mResponseCount >= mResponseLimit))
        cancel();

    return true;
}

void LunaServiceCall::cancel()
{
    if (mToken == LSMESSAGE_TOKEN_INVALID)
        return;

    if (mServiceHandle == 0)
        return;

    if (mResponseLimit != 1 || mResponseCount != 1) {
        LSError error;
        LSErrorInit(&error);

        if (!LSCallCancel(mServiceHandle, mToken, &error)) {
            qWarning("Failed to cancel call");
            LSErrorPrint(&error, stderr);
            LSErrorFree(&error);
        }
    }

    mToken = LSMESSAGE_TOKEN_INVALID;

    LunaServiceAdapter *adapter = static_cast<LunaServiceAdapter*>(parent());
    adapter->removePendingCall(this);
}

GMainLoop* LunaServiceAdapter::mainLoop()
{
    static GMainLoop *mainLoop = 0;

    if (!mainLoop) {
        GMainContext *mainContext = g_main_context_default();
        mainLoop = g_main_loop_new(mainContext, TRUE);
    }

    return mainLoop;
}

LunaServiceAdapter::LunaServiceAdapter(QObject *parent) :
    QObject(parent),
    mName(""),
    mUsePrivateBus(false),
    mServiceHandle(0),
    mInitialized(false),
    mService(""),
    mMethod("")
{
}

LunaServiceAdapter::~LunaServiceAdapter()
{
    Q_FOREACH (LunaServiceCall *call, mPendingCalls) {
        mPendingCalls.removeOne(call);
        delete call;
    }

    QString serviceHandleName = mName + (mUsePrivateBus ? "-priv" : "-pub");
    LunaServiceHandle *handle = serviceHandles.value(serviceHandleName);

    if (handle && !handle->unref()) {
        serviceHandles.remove(serviceHandleName);
        delete handle;
    }
}

void LunaServiceAdapter::classBegin()
{
}

void LunaServiceAdapter::componentComplete()
{
    LSError error;

    // check wether we have the handle for the same service already cached
    QString serviceHandleName = mName + (mUsePrivateBus ? "-priv" : "-pub");
    if (serviceHandles.contains(serviceHandleName)) {
        LunaServiceHandle *serviceHandle = serviceHandles.value(serviceHandleName);
        serviceHandle->ref();
        mServiceHandle = serviceHandle->handle();
    }
    else {
        LSErrorInit(&error);

        if (!LSRegisterPubPriv(mName.toUtf8().constData(), &mServiceHandle, !mUsePrivateBus, &error)) {
            qWarning("Could not register ls2 service handle!");
            goto error;
        }

        if(!LSGmainAttach(mServiceHandle, mainLoop(), &error)) {
            qWarning("Could not attach to glib main loop!");
            goto error;
        }

        LunaServiceHandle *handle = new LunaServiceHandle(mServiceHandle);
        handle->ref();
        serviceHandles.insert(serviceHandleName, handle);
    }

    mInitialized = true;

    emit initialized();

    return;

error:
    if (LSErrorIsSet(&error)) {
        LSErrorPrint(&error, stderr);
        LSErrorFree(&error);
    }
}

QString LunaServiceAdapter::name() const
{
    return mName;
}

bool LunaServiceAdapter::usePrivateBus() const
{
    return mUsePrivateBus;
}

QString LunaServiceAdapter::service() const
{
    return mService;
}

QString LunaServiceAdapter::method() const
{
    return mMethod;
}

void LunaServiceAdapter::setName(const QString& name)
{
    if (mInitialized) {
        qWarning("Not allowed to change service name after it has been initialized");
        return;
    }

    mName = name;
}

void LunaServiceAdapter::setUsePrivateBus(bool usePrivateBus)
{
    if (mInitialized) {
        qWarning("Not allowed to change service type after it has been initialized");
        return;
    }

    mUsePrivateBus = usePrivateBus;
}

void LunaServiceAdapter::setService(const QString& service)
{
    if (!service.startsWith("palm://") && !service.startsWith("luna://"))
        qWarning("%s: Invalid service name %s set", __FUNCTION__, service.toUtf8().constData());
    mService = service;
    updateCallUri();
}

void LunaServiceAdapter::setMethod(const QString& method)
{
    mMethod = method;
    updateCallUri();
}

void LunaServiceAdapter::setResponseCallback(QJSValue callback)
{
    mResponseCallback = callback;
}

void LunaServiceAdapter::setErrorCallback(QJSValue callback)
{
    mErrorCallback = callback;
}

void LunaServiceAdapter::updateCallUri()
{
    mCallUri = mService;
    if (!mCallUri.endsWith("/"))
        mCallUri += "/";
    mCallUri += mMethod;
}

LunaServiceAdapter::RegisteredMethod::RegisteredMethod(const QString &name, QJSValue callback)
    : mCallback(callback)
{
    memset(&mMethods, 0, sizeof(mMethods));
    mMethods[0].name = g_strdup(name.toUtf8().constData());
    mMethods[0].function = &LunaServiceAdapter::serviceMethodCallback;
}

LunaServiceAdapter::RegisteredMethod::~RegisteredMethod()
{
    if (mMethods[0].name != 0)
        g_free(const_cast<char *>(mMethods[0].name));
}

LSMethod* LunaServiceAdapter::RegisteredMethod::methods()
{
    return mMethods;
}

QJSValue LunaServiceAdapter::RegisteredMethod::callback()
{
    return mCallback;
}

QString LunaServiceAdapter::buildMethodPath(const QString& category, const QString& method)
{
    QString methodPath;

    if (category.endsWith("/"))
        methodPath = category + method;
    else
        methodPath = category + "/" + method;

    return methodPath;
}

bool LunaServiceAdapter::registerMethod(const QString& category, const QString& name, QJSValue callback)
{
    LSError error;

    QString methodPath = buildMethodPath(category, name);
    if (mServiceMethodCallbacks.contains(methodPath)) {
        qWarning() << "Method" << name << "for category" << category
                   << "already has a registered handler";
        return false;
    }

    if (!callback.isCallable()) {
        qWarning() << "Got something as callback which is not callable:" << callback.toString();
        return false;
    }

    LSErrorInit(&error);

    RegisteredMethod *m = new RegisteredMethod(name, callback);
    if (!LSRegisterCategoryAppend(mServiceHandle, category.toUtf8().constData(),
                                  m->methods(), NULL, &error)) {
        qWarning() << "Could not register method handler for method"
                   << name << "in category" << category  << ":" << error.message;
        LSErrorFree(&error);
        delete m;
        return false;
    }

    // It's not a problem if we override the data here for a category again and again as
    // we only have one instance per service
    if (!LSCategorySetData(mServiceHandle, category.toUtf8().constData(), this, &error)) {
        qWarning() << "Failed to set data for category" << category << ":"
                   << error.message;
        LSErrorFree(&error);
        return false;
    }

    mServiceMethodCallbacks.insert(methodPath, m);

    return true;
}

bool LunaServiceAdapter::serviceMethodCallback(LSHandle *handle, LSMessage *message, void *data)
{
    LunaServiceAdapter *instance = static_cast<LunaServiceAdapter*>(data);
    return instance->handleServiceMethodCallback(handle, message);
}

bool LunaServiceAdapter::handleServiceMethodCallback(LSHandle *handle, LSMessage *message)
{
    QString category = LSMessageGetCategory(message);
    QString method = LSMessageGetMethod(message);

    QString methodPath = buildMethodPath(category, method);

    if (!mServiceMethodCallbacks.contains(methodPath)) {
        qWarning() << "We don't have a register method" << methodPath;
        return false;
    }

    RegisteredMethod *m = mServiceMethodCallbacks.value(methodPath);

    LunaServiceMessage msg(message);
    QQmlContext *context = QQmlEngine::contextForObject(this);

    QJSValue response = m->callback().call(QJSValueList() << context->engine()->newQObject(&msg));

    QString responseStr;
    if (!response.isString()) {
        qWarning() << "Got something from callback which isn't a string:" << response.toString();
        qWarning() << "Sending error response to client";
        responseStr = "{\"returnValue\":false,\"errorText\":\"internal error occured\"}";
    }
    else {
        responseStr = response.toString();
    }

    LSError error;

    LSErrorInit(&error);

    if (!LSMessageReply(handle, message, responseStr.toUtf8().constData(), &error)) {
        qWarning() << "Failed to send response to client:" << error.message;
        LSErrorFree(&error);
        return false;
    }

    return true;
}

void LunaServiceAdapter::removePendingCall(LunaServiceCall *call)
{
    if (!mPendingCalls.contains(call))
        return;

    mPendingCalls.removeOne(call);

    delete call;
}

LunaServiceCall* LunaServiceAdapter::createAndExecuteCall(const QString& uri, const QString& arguments, QJSValue callback,  QJSValue errorCallback, int responseLimit)
{
    LunaServiceCall *call = new LunaServiceCall(this);

    call->setup(mServiceHandle, callback, errorCallback, responseLimit);
    call->execute(uri, arguments);

    mPendingCalls.append(call);

    return call;
}

QObject* LunaServiceAdapter::call(const QString& uri, const QString& arguments, QJSValue callback, QJSValue errorCallback)
{
    return createAndExecuteCall(uri, arguments, callback, errorCallback, 1);
}

QObject* LunaServiceAdapter::call(const QString& arguments)
{
    return createAndExecuteCall(mCallUri, arguments, mResponseCallback, mErrorCallback, 1);
}

QObject* LunaServiceAdapter::call(const QString& arguments, QJSValue callback, QJSValue errorCallback)
{
    return createAndExecuteCall(mCallUri, arguments, callback, errorCallback, 1);
}

QObject* LunaServiceAdapter::subscribe(const QString& uri, const QString& arguments, QJSValue callback, QJSValue errorCallback)
{
    return createAndExecuteCall(uri, arguments, callback, errorCallback, -1);
}

QObject* LunaServiceAdapter::subscribe(const QString& arguments)
{
    return createAndExecuteCall(mCallUri, arguments, mResponseCallback, mErrorCallback, -1);
}

QObject* LunaServiceAdapter::subscribe(const QString& arguments, QJSValue callback, QJSValue errorCallback)
{
    return createAndExecuteCall(mCallUri, arguments, callback, errorCallback, -1);
}

bool LunaServiceAdapter::addSubscription(const QString& key, QJSValue message)
{
    LunaServiceMessage *msg = 0;

    if (!message.isQObject())
        return false;

    msg = qobject_cast<LunaServiceMessage*>(message.toQObject());
    if (msg == 0)
        return false;

    if (!LSSubscriptionAdd(mServiceHandle, key.toUtf8().constData(), msg->messageObject(), NULL))
        return false;

    return true;
}

bool LunaServiceAdapter::replyToSubscribers(const QString& key, const QString& payload)
{
    if (!LSSubscriptionReply(mServiceHandle, key.toUtf8().constData(),
                             payload.toUtf8().constData(), NULL))
        return false;

    return true;
}

} // namespace luna
