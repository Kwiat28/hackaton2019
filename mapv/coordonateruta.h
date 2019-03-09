********************************************************************/

#ifndef COORDONATERUTA_H
#define COORDONATERUTA_H

#include <QString>
#include <QVector>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlStreamWriter>
#include <QDomDocument>
#include <QNetworkCookieJar>
#include <QNetworkCookie>

class HttpReturn
{
public:
    bool ok;
    int numarCereri;
    QNetworkReply::NetworkError codEroare;
    QString eroare;
    QString raspuns;
    QString ultimulUrl;

    HttpReturn()
    {
        ok = false;
    }
};

class Http : public QObject
{
    Q_OBJECT

    QEventLoop lp;
    QNetworkAccessManager networkManager;
    QNetworkRequest networkRequest;

    QString UserAgent;

protected:


public:

    bool ok;
    int numarCereri;
    QString raspuns;
    QString lastError;
    QNetworkReply::NetworkError codEroare;
    QString ultimulUrl;

    Http()
    {
        connect(&networkManager, &QNetworkAccessManager::finished, &lp, &QEventLoop::quit);
        if(UserAgent.length() > 0)
            networkRequest.setRawHeader("User-Agent", UserAgent.toUtf8());
    }

    static HttpReturn Get(const QString & iUrl, const QString & iUserAgent = "")
    {
        QUrl url(iUrl);
        HttpReturn ras;
        int nrCer = 0;
        bool cere = true;
        while(cere && (nrCer < 10))
        {
            ++nrCer;
            cere = false;

            QEventLoop lp;
            QNetworkAccessManager networkManager;
            connect(&networkManager, &QNetworkAccessManager::finished, &lp, &QEventLoop::quit);
            QNetworkRequest networkRequest(url);
            if(iUserAgent.length() > 0)
                networkRequest.setRawHeader("User-Agent", iUserAgent.toUtf8());
            QNetworkReply *reply = networkManager.get(networkRequest);
            lp.exec();

            QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            if(!redirectionTarget.isNull())
            {
                url = url.resolved(redirectionTarget.toUrl());
                cere = true;
            }

            ras.raspuns = reply->readAll();
            ras.eroare = reply->errorString();
            ras.codEroare = reply->error();
            ras.numarCereri = nrCer;
            ras.ultimulUrl = url.toString();
            if(reply->error() == QNetworkReply::NoError)
                ras.ok = true;
            reply->deleteLater();
        }

        return ras;
    }

    static HttpReturn Post(const QString & iUrl, const QString iPostData, const QString & iUserAgent = "")
    {
        QUrl serviceUrl = QUrl(iUrl);
        QEventLoop lp;
        QNetworkAccessManager networkManager;
        connect(&networkManager, &QNetworkAccessManager::finished, &lp, &QEventLoop::quit);

        QNetworkRequest networkRequest(serviceUrl);
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        if(iUserAgent.length() > 0)
            networkRequest.setRawHeader("User-Agent", iUserAgent.toUtf8());

        QNetworkReply *reply = networkManager.post(networkRequest,
                                                   iPostData.toUtf8());
        lp.exec();
        HttpReturn ras;
        ras.raspuns = reply->readAll();
        ras.eroare = reply->errorString();
        ras.codEroare = reply->error();
        if(reply->error() == QNetworkReply::NoError)
            ras.ok = true;
        reply->deleteLater();
        return ras;
    }

    static QString Encode(const QString & iData)
    {
        return QUrl::toPercentEncoding(iData);
    }
};

class Coordonata
{
public:
    qreal Y;
    qreal X;
    Coordonata(){}
    Coordonata(qreal iY, qreal iX)
    {
        Y = iY;
        X = iX;
    }
};

class CoordonateRuta
{

public:
    QVector<Coordonata> gpx;

    CoordonateRuta()
    {}
    void getUrl(const QString & urlFisierGpx)
    {
        gpx.clear();
        HttpReturn raspuns = Http::Get(urlFisierGpx);
        if(raspuns.ok)
        {
            QDomDocument doc;
            doc.setContent(raspuns.raspuns);
            if(doc.elementsByTagName("rtept").count()>0)
            {
                QDomNodeList randuri = doc.elementsByTagName("rtept");
                for(int i = 0; i < randuri.size(); ++i)
                {
                    QDomNode rtept = randuri.at(i);
                    QDomNamedNodeMap ndMap = rtept.attributes();
                    qreal lat = ndMap.namedItem("lat").toAttr().value().toDouble();
                    qreal lon = ndMap.namedItem("lon").toAttr().value().toDouble();
                    if(lat!=0&&lon!=0)
                        gpx.push_back(Coordonata(lat, lon));
                }
            }
        }
    }
};

#endif // COORDONATERUTA_H
