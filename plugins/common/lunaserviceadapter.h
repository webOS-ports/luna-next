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

#ifndef LUNASERVICEADAPTER_H_
#define LUNASERVICEADAPTER_H_

#include <QObject>
#include <QString>
#include <QQmlParserStatus>
#include <QJSValue>
#include <QList>
#include <QVariant>
#include <luna-service2/lunaservice.h>

namespace luna
{

class LunaServiceMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString payload READ payload CONSTANT)
    Q_PROPERTY(QString applicationId READ applicationId CONSTANT)

public:
    LunaServiceMessage(LSMessage *message, QObject *parent = 0);
    ~LunaServiceMessage();

    QString payload() const;
    QString applicationId() const;

    LSMessage* messageObject() const;

private:
    LSMessage *mMessage;
};

class LunaServiceCall : public QObject
{
    Q_OBJECT

public:
    explicit LunaServiceCall(QObject *parent = 0);

    void setup(LSHandle *serviceHandle, QJSValue callback, QJSValue errorCallback, int responseLimit = -1);
    bool execute(const QString& uri, const QString& arguments);

    Q_INVOKABLE void cancel();

private:
    LSHandle *mServiceHandle;
    QJSValue mCallback;
    QJSValue mErrorCallback;
    LSMessageToken mToken;
    int mResponseLimit;
    int mResponseCount;

    static bool responseCallback(LSHandle *handle, LSMessage *message, void *user_data);
    bool handleResponse(LSHandle *handle, LSMessage *message);
};

class LunaServiceAdapter : public QObject,
                           public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(bool usePrivateBus READ usePrivateBus WRITE setUsePrivateBus)
    Q_PROPERTY(QString service READ service WRITE setService)
    Q_PROPERTY(QString method READ method WRITE setMethod)
    Q_PROPERTY(QJSValue onResponse WRITE setResponseCallback)
    Q_PROPERTY(QJSValue onError WRITE setErrorCallback)

public:
    LunaServiceAdapter(QObject *parent = 0);
    virtual ~LunaServiceAdapter();

    void classBegin();
    void componentComplete();

    QString name() const;
    bool usePrivateBus() const;
    QString service() const;
    QString method() const;

    void setName(const QString& name);
    void setUsePrivateBus(bool usePrivateBus);
    void setService(const QString& service);
    void setMethod(const QString& method);
    void setResponseCallback(QJSValue callback);
    void setErrorCallback(QJSValue callback);

    Q_INVOKABLE QObject* call(const QString& uri, const QString& arguments, QJSValue callback, QJSValue errorCallback);
    Q_INVOKABLE QObject* call(const QString& arguments);
    Q_INVOKABLE QObject* call(const QString& arguments, QJSValue, QJSValue errorCallback);
    Q_INVOKABLE QObject* subscribe(const QString& uri, const QString& arguments, QJSValue callback, QJSValue errorCallback);
    Q_INVOKABLE QObject* subscribe(const QString& arguments);
    Q_INVOKABLE QObject* subscribe(const QString& arguments, QJSValue callback, QJSValue errorCallback);
    Q_INVOKABLE bool registerMethod(const QString& category, const QString& name, QJSValue callback);
    Q_INVOKABLE bool addSubscription(const QString& key, QJSValue message);
    Q_INVOKABLE bool replyToSubscribers(const QString& key, const QString& payload);

signals:
    void initialized();

private:
    QString mName;
    bool mUsePrivateBus;
    LSHandle *mServiceHandle;
    bool mInitialized;
    QString mService;
    QString mMethod;
    QJSValue mResponseCallback;
    QJSValue mErrorCallback;
    QString mCallUri;

    LunaServiceCall* createAndExecuteCall(const QString& uri, const QString& arguments, QJSValue callback, QJSValue errorCallback, int responseLimit);

    QString buildMethodPath(const QString& category, const QString& method);
    void updateCallUri();

    static bool serviceMethodCallback(LSHandle *handle, LSMessage *message, void *data);
    bool handleServiceMethodCallback(LSHandle *handle, LSMessage *message);

    class RegisteredMethod
    {
    public:
        RegisteredMethod(const QString& name, QJSValue callback);
        ~RegisteredMethod();

        LSMethod* methods();
        QJSValue callback();

    private:
        LSMethod mMethods[2];
        QJSValue mCallback;
    };

    QMap<QString, RegisteredMethod*> mServiceMethodCallbacks;

    static GMainLoop *mainLoop();
};

} // namespace luna

#endif
