/*************************************************************************
**
** 
**************************************************************************/

#ifndef MAPOBJECTS_H
#define MAPOBJECTS_H

#include <QtCore>

class MapObject
{
public:
    qreal longitudineX;
    qreal latitudineY;
    QString iconita;
    QString text;
    MapObject(qreal iLatitudineY, qreal iLongitudineX,
                      const QString & iIconita, const QString & iText)
    {
        latitudineY = iLatitudineY;
        longitudineX = iLongitudineX;
        iconita = iIconita;
        text = iText;
    }
    void setCoordonate(qreal iLatitudineY, qreal iLongitudineX)
    {
        latitudineY = iLatitudineY;
        longitudineX = iLongitudineX;
    }
    void setIconita(const QString & iIconita)
    {
        iconita = iIconita;
    }
    void setText(const QString & iText)
    {
        text = iText;
    }
};

class MapObjects
{
    QList < MapObject > listaObiecte;

public:

    void Add(qreal Latitude, qreal Longitude,
                const QString & Icon, const QString & Text)
    {
        listaObiecte.push_back(MapObject(Latitude, Longitude, Icon, Text));
    }

    void Add(MapObject mo)
    {
        listaObiecte.push_back(mo);
    }

    MapObject at(int i)
    {
        if(i >= 0 && i < listaObiecte.size())
            return listaObiecte.at(i);
        else
            return MapObject(0,0,"","");
    }

    int Size()
    {
        return listaObiecte.size();
    }
};

#endif // MAPOBJECTS_H
