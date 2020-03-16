#include "usbcamera.h"
#include <math.h>

UsbCamera::UsbCamera()
{
    m_stream = NULL;
    m_width = 0;
    m_height = 0;
    m_bgc = false;
    m_busy = false;
}

UsbCamera::~UsbCamera()
{
    stop();
}

bool UsbCamera::init( int width, int height)
{
    m_width = width;
    m_height = height;

    m_stream = new VideoCapture(0);
    if ( !m_stream->isOpened() ) {
        m_stream = NULL;
        return false;
    }
    *m_stream >> m_img;
    m_width = m_img.cols;
    m_height = m_img.rows;
    return true;
}

bool UsbCamera::start()
{
    return m_stream;
}

void UsbCamera::read()
{
    m_stream->read( m_img);
    Sensor::read();
}

void UsbCamera::stop()
{
    if ( m_stream )
        delete m_stream;
    m_stream = NULL;
}

Mat UsbCamera::image()
{
    return m_img;
}

int UsbCamera:: width()
{
    return m_width;
}

int UsbCamera::height()
{
    return m_height;
}

void UsbCamera::setAnalyze( uint8_t threshold, SIZ blocksize)
{
    m_dx = blocksize.w;
    m_dy = blocksize.h;
    m_th = threshold;
    m_old.clear();
    m_new.clear();
    for ( int cx = 0; cx < m_width; cx += m_dx )
        for ( int cy = 0; cy < m_height; cy += m_dy ) {
            m_old.add( {0,0,0});
            m_new.add( {0,0,0});
        }
}

void UsbCamera::setReferenceColor( CLR color, bool background)
{
    m_ref = color;
    m_bgc = background;
}

bool UsbCamera::scanColor( Mat& img, CLR& clr)
{
    long cnt = img.cols * img.rows;
    if ( cnt > 0 ) {
        int r = 0;
        int g = 0;
        int b = 0;
        for ( int x = 10; x < 20; x++ )
            for ( int y = 10; y < 20; y++ ) {
                b += img.at<Vec3b>(y,x)[0];
                g += img.at<Vec3b>(y,x)[1];
                r += img.at<Vec3b>(y,x)[2];
            }
        clr.b = b/cnt;
        clr.g = g/cnt;
        clr.r = r/cnt;
    }
    return (cnt > 0);
}

bool UsbCamera::colorChanged( CLR oldclr, CLR newclr)
{
    // split colors in rgb components
    int ord = oldclr.r;
    int ogr = oldclr.g;
    int obl = oldclr.b;
    int nrd = newclr.r;
    int ngr = newclr.g;
    int nbl = newclr.b;

    // calculate intensity per color
    int oi = ord + ogr + obl;
    int ni = nrd + ngr + nbl;
    if ( !ni || ! oi ) return false;

    // create rgb percentages independent from the intensity
    ord = ord * 100 / oi;
    ogr = ogr * 100 / oi;
    obl = obl * 100 / oi;
    nrd = nrd * 100 / ni;
    ngr = ngr * 100 / ni;
    nbl = nbl * 100 / ni;

    // return like colorEqual
    bool drd = (abs(nrd - ord)) < m_th;
    bool dgr = (abs(ngr - ogr)) < m_th;
    bool dbl = (abs(nbl - obl)) < m_th;
    return !(drd & dgr & dbl);
}

bool UsbCamera::colorEqual( CLR refclr, CLR newclr)
{
    bool drd = (abs(refclr.r - newclr.r) * 100 / 255) < m_th;
    bool dgr = (abs(refclr.g - newclr.g) * 100 / 255) < m_th;
    bool dbl = (abs(refclr.b - newclr.b) * 100 / 255) < m_th;
    if ( m_bgc )
        return !(drd & dgr & dbl);
    return drd & dgr & dbl;
}

/*
bool UsbCamera::intensityEqual( QColor refclr, QColor newclr)
{
    int rh, rs, rl, nh, ns, nl;
    refclr.getHsl( &rh, &rs, &rl);
    newclr.getHsl( &nh, &ns, &nl);
    if ( m_bgc )
        return !(abs( rl - nl) * 100 / 255 < m_th);
    return abs( rl - nl) * 100 / 255 < m_th;
}
*/

int UsbCamera::percentageChanged( int alterationtype)
{
    if ( m_busy ) return -1;
    m_busy = true;
    CLR clr;
    int    ix = 0;
    float  cnt = 0;
    Mat img;
    for ( int cx = 0; cx < m_img.cols - m_dx; cx += m_dx )
        for ( int cy = 0; cy < m_img.rows - m_dy; cy += m_dy ) {
            img = m_img( Rect( cx, cy, m_dx, m_dy));
            if ( scanColor( img, clr) ) {
                m_old.replace( ix, m_new.at( ix));
                m_new.replace( ix, clr);
                switch ( alterationtype ) {
                    case ALT_COLOR :
                        if ( colorChanged( m_old.at( ix), m_new.at( ix)) )
                            cnt += 1;
                        break;
//                   case ALT_INTENSITY :
//                        if ( !intensityEqual( m_old.at( ix), m_new.at( ix)) )
//                            cnt += 1;
//                        break;
                    case ALT_REFCOLOR :
                        if ( colorEqual( m_ref, m_new.at( ix)) )
                            cnt += 1;
                        break;
//                    case ALT_REFINTENSITY :
//                        if ( intensityEqual( m_ref, m_new.at( ix)) )
//                            cnt += 1;
//                        break;
                }
            }
            ix++;
//??            img.release();
//??            delete[] img.data;
        }
    if ( !m_new.count() ) return -1;
    cnt = (cnt * 100) / m_new.count();

    m_busy = false;

    return round( cnt);
}

/*
int UsbCamera::spotCount( uint8_t minimalsize)
{
    if ( m_busy ) return -1;
    m_busy = true;

    int cnt = 0;
    m_minsz = minimalsize;

    // in m_map create an image representation of included pixels
    m_map.fill( false, m_width * m_height);
    QRgb* rgb = (QRgb*) m_img.bits();
    QRgb* end = rgb + m_width * m_height;
    for ( int ix = 0; rgb < end; rgb++, ix++ )
        if ( colorEqual( m_ref, *rgb) )
            m_map.setBit( ix);

    // find the spots in m_map
    for ( int y = 0, ix = 0; y < m_height; y++ ) {
        for ( int x = 0; x < m_width; x++, ix++ )
            if ( m_map.at( ix) ) {
                m_cursz = 0;
                gatherSpot( x, y);
                int z = m_cursz;
                if ( m_cursz >= m_minsz )
                    cnt++;
            }
    }

    m_spot.clear();
    m_map.clear();

    m_busy = false;

    return cnt;
}

void UsbCamera::gatherSpot( int x, int y)
{
    if ( x < 0 || x >= m_width ) return;
    if ( y < 0 || y >= m_height ) return;

    int ix = y * m_width + x;
    if ( !m_map.at( ix) ) return;
    m_map.clearBit( ix);
    m_cursz++;

    gatherSpot( x-1, y-1);
    gatherSpot( x  , y-1);
    gatherSpot( x+1, y-1);

    gatherSpot( x-1, y  );
    gatherSpot( x+1, y  );

    gatherSpot( x-1, y+1);
    gatherSpot( x  , y+1);
    gatherSpot( x+1, y+1);
}
*/
