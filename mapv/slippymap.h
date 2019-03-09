
**
****************************************************************************/

#ifndef SLIPPYMAP_H
#define SLIPPYMAP_H

#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QtNetwork>
#include <QtWidgets>
#include <QtPositioning>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// how long (milliseconds) the user need to hold (after a tap on the screen)
// before triggering the magnifying glass feature
// 701, a prime number, is the sum of 229, 233, 239
// (all three are also prime numbers, consecutive!)
#define HOLD_TIME 701

// maximum size of the magnifier
// Hint: see above to find why I picked this one :)
#define MAX_MAGNIFIER 229

class CoordonateVizibile
{
public:
    qreal latMin;
    qreal latMax;
    qreal lonMin;
    qreal lonMax;
    CoordonateVizibile()
    {
        latMin=0;
        latMax=0;
        lonMin=0;
        lonMax=0;
    }
};

uint qHash(const QPoint& p);

// tile size in pixels
const int tdim = 256;

QPointF tileForCoordinate(qreal lat, qreal lng, int zoom);

qreal longitudeFromTile(qreal tx, int zoom);

qreal latitudeFromTile(qreal ty, int zoom);

class SlippyMap: public QObject
{
    Q_OBJECT

    QNetworkDiskCache *cache;

public:
    int width;
    int height;
    int zoom;
    qreal latitude;
    qreal longitude;
    QString cacheDir;

    CoordonateVizibile cv;

    SlippyMap(const QString & iCacheDir, QObject *parent = 0)
        : QObject(parent)
        , width(400)
        , height(300)
        , zoom(12)
        , latitude(44.436012)
        , longitude(26.102392)
        , cacheDir(iCacheDir)
    {
        calculeazaCoordonateleVizibile();
        m_emptyTile = QPixmap(tdim, tdim);
        m_emptyTile.fill(Qt::lightGray);

        cache = new QNetworkDiskCache;
        cache->setCacheDirectory(cacheDir);
        m_manager.setCache(cache);
        connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(handleNetworkData(QNetworkReply*)));
    }
    void clearCache()
    {
        cache->clear();
    }
    void calculeazaCoordonateleVizibile()
    {
        QPointF dxMin = QPointF(width/2,-(height/2)) / qreal(tdim);
        QPointF dxMax = QPointF(-(width/2),height/2) / qreal(tdim);
        QPointF centerMin = tileForCoordinate(latitude, longitude, zoom) - dxMin;
        QPointF centerMax = tileForCoordinate(latitude, longitude, zoom) - dxMax;
        cv.lonMin = longitudeFromTile(centerMin.x(), zoom);
        cv.lonMax = longitudeFromTile(centerMax.x(), zoom);
        cv.latMin = latitudeFromTile(centerMin.y(), zoom);
        cv.latMax = latitudeFromTile(centerMax.y(), zoom);
    }
    QPointF pozitiaPeHarta(qreal iLatY, qreal iLonX)
    {
        return QPointF(((iLonX-cv.lonMin)/(cv.lonMax-cv.lonMin))*(width-1),
                    height-((iLatY-cv.latMin)/(cv.latMax-cv.latMin))*(height-1));
    }
    void invalidate()
    {
        if (width <= 0 || height <= 0)
            return;

        QPointF ct = tileForCoordinate(latitude, longitude, zoom);
        qreal tx = ct.x();
        qreal ty = ct.y();

        calculeazaCoordonateleVizibile();

        // top-left corner of the center tile
        int xp = width / 2 - (tx - floor(tx)) * tdim;
        int yp = height / 2 - (ty - floor(ty)) * tdim;

        // first tile vertical and horizontal
        int xa = (xp + tdim - 1) / tdim;
        int ya = (yp + tdim - 1) / tdim;
        int xs = static_cast<int>(tx) - xa;
        int ys = static_cast<int>(ty) - ya;

        // offset for top-left tile
        m_offset = QPoint(xp - xa * tdim, yp - ya * tdim);

        // last tile vertical and horizontal
        int xe = static_cast<int>(tx) + (width - xp - 1) / tdim;
        int ye = static_cast<int>(ty) + (height - yp - 1) / tdim;

        // build a rect
        m_tilesRect = QRect(xs, ys, xe - xs + 1, ye - ys + 1);

        if (m_url.isEmpty())
            download();

        emit updated(QRect(0, 0, width, height));
    }

    void render(QPainter *p, const QRect &rect)
    {
        for (int x = 0; x <= m_tilesRect.width(); ++x)
            for (int y = 0; y <= m_tilesRect.height(); ++y) {
                QPoint tp(x + m_tilesRect.left(), y + m_tilesRect.top());
                QRect box = tileRect(tp);
                if (rect.intersects(box)) {
                    if (m_tilePixmaps.contains(tp))
                        p->drawPixmap(box, m_tilePixmaps.value(tp));
                    else
                        p->drawPixmap(box, m_emptyTile);
                }
            }
    }

    void pan(const QPoint &delta)
    {
        QPointF dx = QPointF(delta) / qreal(tdim);
        QPointF center = tileForCoordinate(latitude, longitude, zoom) - dx;
        latitude = latitudeFromTile(center.y(), zoom);
        longitude = longitudeFromTile(center.x(), zoom);
        calculeazaCoordonateleVizibile();
        invalidate();
    }

private slots:

    void handleNetworkData(QNetworkReply *reply)
    {
        QImage img;
        QPoint tp = reply->request().attribute(QNetworkRequest::User).toPoint();
        QUrl url = reply->url();
        if (!reply->error())
            if (!img.load(reply, 0))
                img = QImage();
        reply->deleteLater();
        m_tilePixmaps[tp] = QPixmap::fromImage(img);
        if (img.isNull())
            m_tilePixmaps[tp] = m_emptyTile;
        emit updated(tileRect(tp));

        // purge unused spaces
        QRect bound = m_tilesRect.adjusted(-2, -2, 2, 2);
        foreach(QPoint tp, m_tilePixmaps.keys())
        if (!bound.contains(tp))
            m_tilePixmaps.remove(tp);

        download();
    }

    void download()
    {
        QPoint grab(0, 0);
        for (int x = 0; x <= m_tilesRect.width(); ++x)
            for (int y = 0; y <= m_tilesRect.height(); ++y) {
                QPoint tp = m_tilesRect.topLeft() + QPoint(x, y);
                if (!m_tilePixmaps.contains(tp)) {
                    grab = tp;
                    break;
                }
            }
        if (grab == QPoint(0, 0)) {
            m_url = QUrl();
            return;
        }

        //QString path = "http://tile.openstreetmap.org/%1/%2/%3.png";
        QString path = "http://otile1.mqcdn.com/tiles/1.0.0/osm/%1/%2/%3.png";
        m_url = QUrl(path.arg(zoom).arg(grab.x()).arg(grab.y()));
        QNetworkRequest request;
        request.setUrl(m_url);
        request.setRawHeader("User-Agent", "Nokia (Qt) Graphics Dojo 1.0");
        request.setAttribute(QNetworkRequest::User, QVariant(grab));
        m_manager.get(request);
    }

signals:
    void updated(const QRect &rect);

protected:
    QRect tileRect(const QPoint &tp)
    {
        QPoint t = tp - m_tilesRect.topLeft();
        int x = t.x() * tdim + m_offset.x();
        int y = t.y() * tdim + m_offset.y();
        return QRect(x, y, tdim, tdim);
    }

private:
    QPoint m_offset;
    QRect m_tilesRect;
    QPixmap m_emptyTile;
    QHash<QPoint, QPixmap> m_tilePixmaps;
    QNetworkAccessManager m_manager;
    QUrl m_url;
};

#endif // SLIPPYMAP_H
