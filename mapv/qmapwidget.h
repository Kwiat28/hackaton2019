

#ifndef QMAPWIDGET_H
#define QMAPWIDGET_H

#include <QtCore>
#include <QWidget>
#include "mapobjects.h"
#include "tilemap.h"

class QMapWidget:public QMainWindow
{
    Q_OBJECT

    MapObjects mapObj;
    TileMap * map;

    void Init()
    {
        map = new TileMap(
                    &mapObj
#ifdef ANDROID
                    , "/mnt/sdcard/" + QCoreApplication::applicationName() + "/Cache"
#else
                    , QCoreApplication::applicationDirPath() + "/Cache"
#endif
                    , this
                    );
        setCentralWidget(map);
    }

public:
    QMapWidget(QWidget *parent = 0)
    {
        Init();
    }

    QMapWidget(MapObjects iMapObjects, QWidget *parent = 0)
    {
        setMapObjects(iMapObjects);
        Init();
    }

    ~QMapWidget()
    {
        delete map;
    }

    void showRoute(qreal fromLatitude, qreal fromLongitude
                   , qreal toLatitude, qreal toLongitude)
    {
        map->showRoute(fromLatitude, fromLongitude, toLatitude, toLongitude);
    }

    void setMapObjects(MapObjects iMapObjects)
    {
        mapObj = iMapObjects;
    }

    bool hasPositioning()
    {
        return (map->gps->OK && map->gps->services.count()>0 && map->gps->latitudine!=0 && map->gps->longitudine!=0);
    }

    void showCurrentPosition()
    {
        map->setCenter(map->gps->latitudine, map->gps->longitudine);
    }

    void startPositioning()
    {
        map->gps->slPornesteActualizarePozitie();
    }
    void stopPositioning()
    {
        map->gps->slOpresteActualizarePozitie();
    }

    QString gpsInfo()
    {
        return QString("Available services: %1\nLatitude: %2\nLongitude: %3")
                .arg(map->gps->services.join(", ")).arg(map->gps->latitudine).arg(map->gps->longitudine);
    }
};

#endif // QMAPWIDGET_H
