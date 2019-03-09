

#ifndef TILEMAP_H
#define TILEMAP_H

#include "gps.h"
#include "mapobjects.h"
#include "slippymap.h"
#include "coordonateruta.h"

class TileMap: public QWidget
{
    Q_OBJECT

    MapObjects * obPeHarta;
    bool afiseazaRuta;

public:

    Gps * gps;
    CoordonateRuta cr;

    TileMap(MapObjects * iObPeHarta,
                             const QString & iCacheDir,
                             QWidget *parent) : QWidget(parent)
      , obPeHarta(iObPeHarta)
      , pressed(false)
      , snapped(false)
      , zoomed(false)
      , invert(false)
    {
        if(!QDir(iCacheDir).exists())
            QDir().mkpath(iCacheDir);
        m_normalMap = new SlippyMap(iCacheDir, this);
        connect(m_normalMap, SIGNAL(updated(QRect)), SLOT(updateMap(QRect)));

        QPushButton *btPlus=new QPushButton("+",this);
        QPushButton *btMinus=new QPushButton("-",this);
        //btPlus->setFlat(true);
        //btMinus->setFlat(true);
        btPlus->setAutoFillBackground(true);
        btMinus->setAutoFillBackground(true);
        btPlus->setGeometry(50,10,30,30);
        btMinus->setGeometry(10,10,30,30);
        //btPlus->setStyleSheet(QStringLiteral("background-color: rgba(255, 255, 255, 70%);"));
        //btMinus->setStyleSheet(QStringLiteral("background-color: rgba(255, 255, 255, 70%);"));
        btPlus->setFocusPolicy(Qt::NoFocus);
        btMinus->setFocusPolicy(Qt::NoFocus);
        connect(btPlus,SIGNAL(clicked()), this, SLOT(slPlus()));
        connect(btMinus,SIGNAL(clicked()), this, SLOT(slMinus()));

        gps = new Gps(this);
    }

    void setCenter(qreal lat, qreal lng)
    {
        m_normalMap->latitude = lat;
        m_normalMap->longitude = lng;
        m_normalMap->calculeazaCoordonateleVizibile();
        m_normalMap->invalidate();
    }

    void showRoute(qreal fromLatitude, qreal fromLongitude
              , qreal toLatitude, qreal toLongitude)
    {
        cr.getUrl(QString("http://router.project-osrm.org/viaroute?hl=en&loc=%1,%2&loc=%3,%4&output=gpx")
                  .arg(fromLatitude).arg(fromLongitude).arg(toLatitude).arg(toLongitude));
        afiseazaRuta = true;
        m_normalMap->invalidate();
    }

    void setRuta(const CoordonateRuta & iCr)
    {
        cr = iCr;
        afiseazaRuta = true;
    }
    void cancelRuta()
    {
        afiseazaRuta = false;
    }

public slots:
    void toggleNightMode()
    {
        invert = !invert;
        update();
    }

    void slPlus()
    {
        if(m_normalMap->zoom>=19)
            return;
        m_normalMap->zoom = m_normalMap->zoom + 1;
        m_normalMap->invalidate();
        update();
        this->setFocus();
    }

    void slMinus()
    {
        if(m_normalMap->zoom<=5)
            return;
        m_normalMap->zoom = m_normalMap->zoom - 1;
        m_normalMap->invalidate();
        update();
        this->setFocus();
    }

    void slClearCache()
    {
        m_normalMap->clearCache();
    }

private slots:
    void updateMap(const QRect &r) {
        update(r);
    }

protected:

    void resizeEvent(QResizeEvent *)
    {
        m_normalMap->width = width();
        m_normalMap->height = height();
        m_normalMap->invalidate();
    }

    void paintEvent(QPaintEvent *event)
    {
        QPainter p;
        p.begin(this);
        m_normalMap->render(&p, event->rect());
        p.setPen(Qt::black);
        p.drawText(rect(),  Qt::AlignBottom | Qt::TextWordWrap,
                   "Map data CCBYSA 2009 OpenStreetMap.org contributors");
        for(int i = 0; i < obPeHarta->Size(); ++i)
        {
            MapObject tx = obPeHarta->at(i);
            if(tx.latitudineY>=m_normalMap->cv.latMin
                    &&tx.latitudineY<=m_normalMap->cv.latMax
                    &&tx.longitudineX>=m_normalMap->cv.lonMin
                    &&tx.longitudineX<=m_normalMap->cv.lonMax
                    )
            {
                QPointF ph = m_normalMap->pozitiaPeHarta(tx.latitudineY,tx.longitudineX);
                QImage ic(tx.iconita);
                QPainter painter(&ic);
                QFont ft;ft.setPointSize(16);ft.setBold(true);
                painter.setFont(ft);
                painter.drawText(ic.rect(),Qt::AlignCenter, tx.text);

                p.drawPixmap(ph.x()-ic.height()/2,ph.y()-ic.width()/2,QPixmap::fromImage(ic));
            }
        }

        if(afiseazaRuta)
        {
            qreal lastY;
            qreal lastX;
            for(int i = 0; i< cr.gpx.size(); ++i)
            {
                qreal Y = cr.gpx.at(i).Y;
                qreal X = cr.gpx.at(i).X;
                if(i==0){
                    lastY = Y;
                    lastX = X;}
                {
                    QPainter p(this);
                    p.setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    p.drawLine(m_normalMap->pozitiaPeHarta(lastY, lastX),
                               m_normalMap->pozitiaPeHarta(Y, X));
                    lastY = Y;
                    lastX = X;
                }
            }
        }
        p.end();

        if (zoomed)
        {
            int dim = qMin(width(), height());
            int magnifierSize = qMin(MAX_MAGNIFIER, dim * 2 / 3);
            int radius = magnifierSize / 2;
            int ring = radius - 15;
            QSize box = QSize(magnifierSize, magnifierSize);

            // reupdate our mask
            if (maskPixmap.size() != box) {
                maskPixmap = QPixmap(box);
                maskPixmap.fill(Qt::transparent);

                QRadialGradient g;
                g.setCenter(radius, radius);
                g.setFocalPoint(radius, radius);
                g.setRadius(radius);
                g.setColorAt(1.0, QColor(255, 255, 255, 0));
                g.setColorAt(0.5, QColor(128, 128, 128, 255));

                QPainter mask(&maskPixmap);
                mask.setRenderHint(QPainter::Antialiasing);
                mask.setCompositionMode(QPainter::CompositionMode_Source);
                mask.setBrush(g);
                mask.setPen(Qt::NoPen);
                mask.drawRect(maskPixmap.rect());
                mask.setBrush(QColor(Qt::transparent));
                mask.drawEllipse(g.center(), ring, ring);
                mask.end();
            }

            QPoint center = dragPos - QPoint(0, radius);
            center = center + QPoint(0, radius / 2);
            QPoint corner = center - QPoint(radius, radius);

            QPoint xy = center * 2 - QPoint(radius, radius);

            // only set the dimension to the magnified portion
            if (zoomPixmap.size() != box) {
                zoomPixmap = QPixmap(box);
                zoomPixmap.fill(Qt::lightGray);
            }
            if (true) {
                QPainter p(&zoomPixmap);
                p.translate(-xy);
                p.end();
            }

            QPainterPath clipPath;
            clipPath.addEllipse(center, ring, ring);

            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing);
            p.setClipPath(clipPath);
            p.drawPixmap(corner, zoomPixmap);
            p.setClipping(false);
            p.drawPixmap(corner, maskPixmap);
            p.setPen(Qt::gray);
            p.drawPath(clipPath);
        }
        if (invert)
        {
            QPainter p(this);
            p.setCompositionMode(QPainter::CompositionMode_Difference);
            p.fillRect(event->rect(), Qt::white);
            p.end();
        }
    }

    void timerEvent(QTimerEvent *)
    {
        update();
    }

    void mousePressEvent(QMouseEvent *event)
    {
        if (event->buttons() != Qt::LeftButton)
            return;
        pressed = snapped = true;
        pressPos = dragPos = event->pos();
        tapTimer.stop();
        tapTimer.start(HOLD_TIME, this);
    }

    void mouseMoveEvent(QMouseEvent *event)
    {
        if (!event->buttons())
            return;
        if (!zoomed) {
            if (!pressed || !snapped) {
                QPoint delta = event->pos() - pressPos;
                pressPos = event->pos();
                m_normalMap->pan(delta);
                return;
            } else {
                const int threshold = 10;
                QPoint delta = event->pos() - pressPos;
                if (snapped) {
                    snapped &= delta.x() < threshold;
                    snapped &= delta.y() < threshold;
                    snapped &= delta.x() > -threshold;
                    snapped &= delta.y() > -threshold;
                }
                if (!snapped)
                    tapTimer.stop();
            }
        } else {
            dragPos = event->pos();
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent *)
    {
        zoomed = false;
        update();
    }

    void keyPressEvent(QKeyEvent *event) {
        if (!zoomed) {
            if (event->key() == Qt::Key_Left)
                m_normalMap->pan(QPoint(20, 0));
            if (event->key() == Qt::Key_Right)
                m_normalMap->pan(QPoint(-20, 0));
            if (event->key() == Qt::Key_Up)
                m_normalMap->pan(QPoint(0, 20));
            if (event->key() == Qt::Key_Down)
                m_normalMap->pan(QPoint(0, -20));
            if (event->key() == Qt::Key_Z || event->key() == Qt::Key_Select) {
                dragPos = QPoint(width() / 2, height() / 2);
            }
        } else {
            if (event->key() == Qt::Key_Z || event->key() == Qt::Key_Select) {
                zoomed = false;
                update();
            }
            QPoint delta(0, 0);
            if (event->key() == Qt::Key_Left)
                delta = QPoint(-15, 0);
            if (event->key() == Qt::Key_Right)
                delta = QPoint(15, 0);
            if (event->key() == Qt::Key_Up)
                delta = QPoint(0, -15);
            if (event->key() == Qt::Key_Down)
                delta = QPoint(0, 15);
            if (delta != QPoint(0, 0)) {
                dragPos += delta;
                update();
            }
        }
    }

private:
    SlippyMap *m_normalMap;
    bool pressed;
    bool snapped;
    QPoint pressPos;
    QPoint dragPos;
    QBasicTimer tapTimer;
    bool zoomed;
    QPixmap zoomPixmap;
    QPixmap maskPixmap;
    bool invert;
};

#endif
