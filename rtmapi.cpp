

#include <QtCore>
#include <QtXml>
#include <KIO/NetAccess>
#include <KIO/JobUiDelegate>
#include <KIO/Job>
#include <KIO/FileCopyJob>
#include <KTemporaryFile>

#include <KDebug>

#include "rtmapi.h"

class RtmApi::Private {
public:
    QString api_key;
    QString secret;
    QString frob;
    QString token;
    QString userId;
    QString userName;
    QString userFullName;
    QMap<QString, QString> args;
};


RtmApi::RtmApi(const QString &api_key, const QString &secret)
{
    d = new RtmApi::Private;
    d->api_key = api_key;
    d->secret = secret;

}

QString RtmApi::getAuthUrl()
{
    reset();
    addParam("perms", "delete");
    QString x = call("auth.getFrob"); //, "http://www.rememberthemilk.com/services/auth/");
    reset(); // clean call :-)
    
    QXmlStreamReader xml(x.toUtf8());
    
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.name() == "frob" && xml.isStartElement()) {
            xml.readNext();
            d->frob = xml.text().toString();
        }
    }

    addParam("perms", "delete"); // TODO: ask less permission, if we can. :-)
    addParam("frob", d->frob);
    QString url = getUrl("", "http://www.rememberthemilk.com/services/auth/");
    kDebug() << url;
    
    return url;
}

bool RtmApi::authenticate()
{
    addParam("frob", d->frob, true);
    QString x = call("auth.getToken");
    
    QXmlStreamReader xml(x.toUtf8());
    
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.name() == "token" && xml.isStartElement()) {
            xml.readNext();
            d->token = xml.text().toString();
        }
        if (xml.name() == "user") {
            d->userId = xml.attributes().value("id").toString();
            d->userName = xml.attributes().value("username").toString();
            d->userFullName = xml.attributes().value("fullname").toString();
        }
    }
    
    if (!d->token.isEmpty()) {
        return 0;
    } else {
        return 1;       
    }
}

QString RtmApi::getUrl(QString method, QString base)
{
    if (method != "") { addParam("method", "rtm."+method); };
    addParam("api_key", d->api_key);
    addParam("api_sig", getSignature());
    
    QString s = "?";
    QStringList ss;
    
    QMap<QString, QString>::const_iterator i = d->args.constBegin();
    while (i != d->args.constEnd()) {
        ss << i.key() + "=" + i.value();
        ++i;
    }
    
    s += ss.join("&");
    
    return base+s;
}

QString RtmApi::call(QString method, QString url)
{
    QString s = getUrl(method, url);
                  
    KIO::Job *job = KIO::get(s, KIO::Reload, KIO::HideProgressInfo);
    QByteArray data;
    if ( KIO::NetAccess::synchronousRun( job, 0, &data ) ) {
        return data;
    }
}

QString RtmApi::getSignature()
{
    QString s = d->secret + parameters();
    return QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Md5).toHex();
}

void RtmApi::addParam(QString key, QString value, bool r)
{
    if (r) { reset(); }
    d->args[key] = value;
}

QString RtmApi::parameters()
{
    QString s;
    
    QMap<QString, QString>::const_iterator i = d->args.constBegin();
    while (i != d->args.constEnd()) {
        s += i.key() + i.value();
        ++i;
    }
    
    return s;
}

void RtmApi::reset()
{
    d->args.clear();
}

RtmApi::~RtmApi()
{
    delete d;
}
