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

#include "lunaserviceadapter.h"

namespace luna
{

LunaServiceCall::LunaServiceCall(QObject *parent) :
    QObject(parent),
    mServiceHandle(0),
    mToken(LSMESSAGE_TOKEN_INVALID),
    mResponseLimit(-1),
    mResponseCount(0)
{
}

void LunaServiceCall::setup(LSHandle *serviceHandle, QJSValue callback, int responseLimit)
{
    mServiceHandle = serviceHandle;
    mCallback = callback;
    mResponseLimit = responseLimit;
}

bool LunaServiceCall::execute(const QString& uri, const QString& arguments)
{
    LSError error;
    LSErrorInit(&error);

    if (mResponseLimit == 1) {
        if (!LSCallOneReply(mServiceHandle, uri.toUtf8().constData(), arguments.toUtf8().constData(),
                            &LunaServiceCall::responseCallback, this, &mToken, &error)) {
            qWarning("Failed to call remote service %s", uri.toUtf8().constData());
            goto error;
        }
    }
    else {
        if (!LSCall(mServiceHandle, uri.toUtf8().constData(), arguments.toUtf8().constData(),
                            &LunaServiceCall::responseCallback, this, &mToken, &error)) {
            qWarning("Failed to call remote service %s", uri.toUtf8().constData());
            goto error;
        }
    }

    return true;

error:
    if (LSErrorIsSet(&error)) {
        LSErrorPrint(&error, stderr);
        LSErrorFree(&error);
    }

    return false;
}

bool LunaServiceCall::responseCallback(LSHandle *handle, LSMessage *message, void *user_data)
{
    LunaServiceCall *call = static_cast<LunaServiceCall*>(user_data);
    return call->handleResponse(handle, message);
}

bool LunaServiceCall::handleResponse(LSHandle *handle, LSMessage *message)
{
    mResponseCount++;

    QString data = LSMessageGetPayload(message);
    mCallback.call(QJSValueList() << data);

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

    // Schedule ourself for deletion as we're not needed anymore
    deleteLater();
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
    mInitialized(false)
{
}

void LunaServiceAdapter::classBegin()
{
}

void LunaServiceAdapter::componentComplete()
{
    LSError error;
    LSErrorInit(&error);

    if (!LSRegisterPubPriv(mName.toUtf8().constData(), &mServiceHandle, !mUsePrivateBus, &error)) {
        qWarning("Could not register ls2 service handle!");
        goto error;
    }

    if(!LSGmainAttach(mServiceHandle, mainLoop(), &error)) {
        qWarning("Could not attach to glib main loop!");
        goto error;
    }

    mInitialized = true;

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

LunaServiceCall* LunaServiceAdapter::createAndExecuteCall(const QString& uri, const QString& arguments, QJSValue callback, int responseLimit)
{
    LunaServiceCall *call = new LunaServiceCall();
    call->setup(mServiceHandle, callback, responseLimit);
    call->execute(uri, arguments);
    return call;
}

QObject* LunaServiceAdapter::call(const QString& uri, const QString& arguments, QJSValue callback)
{
    return createAndExecuteCall(uri, arguments, callback, 1);
}

QObject* LunaServiceAdapter::subscribe(const QString& uri, const QString& arguments, QJSValue callback)
{
    return createAndExecuteCall(uri, arguments, callback, -1);
}

} // namespace luna
