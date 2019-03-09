/*************************************************************************

**************************************************************************/

#ifndef GPS_H
#define GPS_H

#include <QObject>
//#include <QtPositioning>
#include <QtGeoLocation>

class Gps : public QObject
{
    Q_OBJECT

    QGeoPositionInfoSource *source;

public:
    bool seActualizeaza;
    bool OK;
    QStringList services;
    qreal latitudine;
    qreal longitudine;
    explicit Gps(QObject *parent = 0)
    {
        latitudine = 0;
        longitudine = 0;
        OK = false;
        seActualizeaza = false;

        services = QGeoPositionInfoSource::availableSources();
        source = QGeoPositionInfoSource::createDefaultSource(this);

        if(services.isEmpty()){}

        if(source)
        {
            source->setPreferredPositioningMethods(QGeoPositionInfoSource::
                                                   AllPositioningMethods);
            connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),
                    this, SLOT(slOnPositionUpdate(QGeoPositionInfo)));
            slPornesteActualizarePozitie();
            OK = true;
        }
    }

signals:

public slots:
    void slPornesteActualizarePozitie()
    {
        if(source && !(services.isEmpty()))
        {
            seActualizeaza = true;
            source->startUpdates();
        }
    }
    void slOpresteActualizarePozitie()
    {
        if(source && !(services.isEmpty()))
        {
            seActualizeaza = false;
            source->stopUpdates();
        }
    }
private slots:
    void slOnPositionUpdate(const QGeoPositionInfo &info)
    {
        latitudine = info.coordinate().latitude();
        longitudine = info.coordinate().longitude();
    }
};

#endif // GPS_H
