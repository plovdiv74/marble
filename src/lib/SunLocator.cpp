// Copyright 2007-2009 David Roberts <dvdr18@gmail.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.


#include "SunLocator.h"
#include "ExtDateTime.h"
#include "PlanetaryConstants.h"
#include "MarbleMath.h"
 
#include <QtCore/QDebug>

#include <cmath>
// M_PI is sometimes defined in <cmath>
#ifndef M_PI 
#define M_PI 3.14159265358979323846264338327950288419717
#endif

using namespace Marble;

using std::sin;
using std::cos;
using std::asin;
using std::abs;

qreal deg2rad(qreal x) { return x*M_PI/180.0; }
qreal rad2deg(qreal x) { return x*180.0/M_PI; }

const int J2000 = 2451545; // epoch J2000 = 1 January 2000, noon Terrestrial Time (11:58:55.816 UTC)

// taking the full moon of 15 January 1900 19:07 UTC as the epoch for the moon
const qreal MOON_EPOCH = 2415035.297; // value from http://home.hiwaay.net/~krcool/Astro/moon/fullmoon.htm
const qreal MOON_SYNODIC_PERIOD = 29.530588;

const int update_interval = 60000; // emit updateSun() every update_interval ms

namespace Marble {

class SunLocatorPrivate
{
public:
    explicit SunLocatorPrivate(ExtDateTime *dateTime)
        : m_lon( 0.0 ),
          m_lat( 0.0 ),
          m_datetime( dateTime ),
          m_show( false ),
          m_citylights( false ),
          m_centered( false ),
          m_body( "" )
    {
    }

    qreal m_lon;
    qreal m_lat;

    ExtDateTime* m_datetime;
    bool m_show;
    bool m_citylights;
    bool m_centered;
    QString m_body;
};

}

SunLocator::SunLocator(ExtDateTime *dateTime)
  : QObject(),
    d( new SunLocatorPrivate( dateTime ))
{
}

SunLocator::~SunLocator()
{
    delete d;
}

void SunLocator::updatePosition()
{
    if( d->m_body == "moon" ) {
        qreal days = (qreal)d->m_datetime->toJDN() + d->m_datetime->dayFraction() - MOON_EPOCH; // days since the first full moon of the 20th century
        days /= MOON_SYNODIC_PERIOD; // number of orbits the moon has made (relative to the sun as observed from earth)
        days = days - (int)days; // take fractional part
        if(days < 0.0) days += 1.0; // for dates before MOON_EPOCH
        
        qDebug() << "MOON:" << (int)(days*100) << "% of orbit completed and" << (int)(abs((days-0.5)*2) * 100) << "% illuminated";
        
        d->m_lon = (1-days) * 2*M_PI;
        d->m_lat = 0.0; // not necessarily accurate but close enough (only differs by about +-6 degrees of this value)
	
	return;
    }
    
    PlanetaryConstants pc = PC_EARTH; // default to the earth
    // planets
         if ( d->m_body == "mercury" ) pc = PC_MERCURY;
    else if ( d->m_body == "venus" )   pc = PC_VENUS;
    else if ( d->m_body == "earth" )   pc = PC_EARTH;
    else if ( d->m_body == "mars" )    pc = PC_MARS;
    else if ( d->m_body == "jupiter" ) pc = PC_JUPITER;
    else if ( d->m_body == "saturn" )  pc = PC_SATURN;
    else if ( d->m_body == "uranus" )  pc = PC_URANUS;
    else if ( d->m_body == "neptune" ) pc = PC_NEPTUNE;
    // dwarf planets ... (everybody likes pluto)
    else if ( d->m_body == "pluto" )   pc = PC_PLUTO;
    
    long day = d->m_datetime->toJDN() - J2000; // find current Julian day number relative to epoch J2000
    
    // from http://www.astro.uu.nl/~strous/AA/en/reken/zonpositie.html
    qreal M = pc.M_0 + pc.M_1*day; // mean anomaly
    qreal C = pc.C_1*sin(M) + pc.C_2*sin(2*M) + pc.C_3*sin(3*M) + pc.C_4*sin(4*M) + pc.C_5*sin(5*M) + pc.C_6*sin(6*M); // equation of center
    qreal nu = M + C; // true anomaly
    qreal lambda_sun = nu + pc.Pi + M_PI; // ecliptic longitude of sun as seen from planet
    qreal delta_sun = asin(sin(pc.epsilon)*sin(lambda_sun)); // declination of sun as seen from planet
    qreal alpha_sun = atan2(cos(pc.epsilon)*sin(lambda_sun), cos(lambda_sun)); // right ascension of sun as seen from planet
    
    qreal theta = alpha_sun; // solar noon occurs when sidereal time is equal to alpha_sun
    d->m_lon = M_PI - (pc.theta_0 + pc.theta_1 * (day + d->m_datetime->dayFraction()) - theta); // convert sidereal time to geographic longitude
    while(d->m_lon < 0) d->m_lon += 2*M_PI;
    d->m_lat = -delta_sun; // convert positive north to positive south
    
    qDebug() << "alpha_sun =" << rad2deg(alpha_sun);
    qDebug() << "delta_sun =" << rad2deg(delta_sun);
    qDebug() << "d->m_lon =" << rad2deg(d->m_lon);
    qDebug() << "d->m_lat =" << rad2deg(d->m_lat);
}


qreal SunLocator::shading(qreal lon, qreal lat) const
{

    // haversine formula
    qreal a = sin((lat-d->m_lat)/2.0);
    qreal b = sin((lon-d->m_lon)/2.0);
    qreal h = (a*a)+cos(lat)*cos(d->m_lat)*(b*b); 

    /*
      h = 0.0 // directly beneath sun
      h = 0.5 // sunrise/sunset line
      h = 1.0 // opposite side of earth to the sun
      theta = 2*asin(sqrt(h))
    */

    qreal twilightZone = 0.0;

    if ( d->m_body == "earth" || d->m_body == "venus" ) {
        twilightZone = 0.1; // this equals 18 deg astronomical twilight.
    }
	
    qreal brightness;
    if ( h <= 0.5 - twilightZone / 2.0 )
        brightness = 1.0;
    else if ( h >= 0.5 + twilightZone / 2.0 )
        brightness = 0.0;
    else
        brightness = ( 0.5 + twilightZone/2.0 - h ) / twilightZone;
	
    return brightness;
}

void SunLocator::shadePixel(QRgb& pixcol, qreal brightness) const
{
    // daylight - no change
    if ( brightness > 0.99999 )
        return;
		
    if ( brightness < 0.00001 ) {
        // night
//      Doing  "pixcol = qRgb(r/2, g/2, b/2);" by shifting some electrons around ;)
        pixcol = qRgb(qRed(pixcol) * 0.35, qGreen(pixcol) * 0.35, qBlue(pixcol)  * 0.35); // by shifting some electrons around ;)
        // pixcol = (pixcol & 0xff000000) | ((pixcol >> 1) & 0x7f7f7f);
    } else {
        // gradual shadowing
        int r = qRed( pixcol );
        int g = qGreen( pixcol );
        int b = qBlue( pixcol );
        qreal  d = 0.65 * brightness + 0.35;
        pixcol = qRgb((int)(d * r), (int)(d * g), (int)(d * b));
    }
}

void SunLocator::shadePixelComposite(QRgb& pixcol, QRgb& dpixcol,
                                     qreal brightness) const
{
    // daylight - no change
    if ( brightness > 0.99999 )
        return;
	
    if ( brightness < 0.00001 ) {
        // night
        pixcol = dpixcol;
    } else {
        // gradual shadowing
        qreal& d = brightness;
		
        int r = qRed( pixcol );
        int g = qGreen( pixcol );
        int b = qBlue( pixcol );
		
        int dr = qRed( dpixcol );
        int dg = qGreen( dpixcol );
        int db = qBlue( dpixcol );
		
        pixcol = qRgb( (int)( d * r + (1 - d) * dr ),
                       (int)( d * g + (1 - d) * dg ),
                       (int)( d * b + (1 - d) * db ) );
    }
}

void SunLocator::update()
{
    qDebug() << "void SunLocator::update()";
    updatePosition();
    if ( d->m_show || d->m_centered )
    {
        if ( d->m_show )
            emit updateSun();
        if ( d->m_centered )
            emit centerSun();
        return;
    }

    emit updateStars();
}

void SunLocator::setShow(bool show)
{
    if ( show == d->m_show ) {
        return;
    }
    
    qDebug() << "void SunLocator::setShow( bool )";
    d->m_show = show;
    updatePosition();

    emit updateSun();
}

void SunLocator::setCentered(bool centered)
{
    if ( centered == d->m_centered ) {
        return;
    }

    qDebug() << "SunLocator::setCentered";
    qDebug() << "sunLocator =" << this;
    d->m_centered = centered;
    if ( d->m_centered ) {
        updatePosition();
        emit centerSun();
    } else
        emit reenableWidgetInput();
}

void SunLocator::setBody(QString body)
{
    if ( body == d->m_body ) {
        return;
    }

    QString previousBody = d->m_body;

    qDebug() << "SunLocator::setBody( QString )";    
    d->m_body = body;
    updatePosition();

    if ( !previousBody.isEmpty() ) {
        emit updateSun();
    }
}

void SunLocator::setCitylights(bool show)
{
    d->m_citylights = show;
}

bool SunLocator::getShow() const
{
    return d->m_show;
}

bool SunLocator::getCitylights() const
{
    return d->m_citylights;
}

bool SunLocator::getCentered() const
{
    return d->m_centered;
}

qreal SunLocator::getLon() const
{
    return d->m_lon * 180.0 / M_PI;
}

qreal SunLocator::getLat() const
{
    return -d->m_lat * 180.0 / M_PI;
}

ExtDateTime* SunLocator::datetime() const
{
    return d->m_datetime;
}

#include "SunLocator.moc"
