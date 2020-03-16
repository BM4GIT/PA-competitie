#ifndef USBCAMERA_H
#define USBCAMERA_H

// link with `pkg-config --cflags --libs opencv`

#include "linkedlist.h"
#include "sensor.h"
#include <opencv2/opencv.hpp>
using namespace cv;


#define ALT_INTENSITY    0
#define ALT_COLOR        1
#define ALT_REFCOLOR     2
#define ALT_REFINTENSITY 3

struct PNT
{
    uint16_t    x;
    uint16_t    y;
};

struct CLR
{
    uint8_t     b;
    uint8_t     g;
    uint8_t     r;
};

struct SIZ
{
    uint16_t    w;
    uint16_t    h;
};

class UsbCamera : public Sensor
{
public:
    UsbCamera();
    ~UsbCamera();

    // buffercount sets the number of buffers used for the camera stream
    bool init( int width, int height);

    bool start();
    void read();
    void stop();

    Mat    image();
    int    width();
    int    height();

    // Camera images can be analised in the following wayss:
    // - Color change per block of a sudden size.
    //   * Color deviation of the current image compared to
    //     that of the previous image.
    //     Options are: ALT_INTENSITY and ALT_COLOR.
    //   * Color deviation compared to a reference color.
    //     Options are: ALT_REFERENCE.
    // - Number of colored spots in the image.
    //   * Color deviation compared to a reference color.
    //     Options are: ALT_COUNT.

    // A threshold (a value of 0 to 255) determines
    // the minimal color change to be noticed.
    // Routine 'percentageChanged' returns the percentage of
    // blocks that got a color change.
    // Routine 'spotCount' returns the number of spots of the
    // specified color in a the range of the threshold
    // Pixels, being in the color range but residing in a
    // specified border around the spot, are disregarded.

    void setAnalyze( uint8_t threshold, SIZ blocksize = {0,0} );
    void setReferenceColor( CLR color, bool background);
    int  percentageChanged( int alterationtype);
//    int  spotCount( uint8_t minimalsize = 30);

protected:

    bool scanColor( Mat& img, CLR& clr);
    bool colorChanged( CLR oldclr, CLR newclr);
    bool colorEqual( CLR refclr, CLR newclr);
//    bool intensityChanged( QColor oldclr, QColor newclr);
//    bool intensityEqual( QColor refclr, QColor newclr);
//    void gatherSpot( int x, int y);

    Mat             m_img;      // scanned image
    int             m_width;    // required image pixel width
    int             m_height;   // required image pixel height

    int             m_dx; // block width
    int             m_dy; // block height
    uint8_t         m_th; // color treshold

//    QBitArray       m_map;  // b/w representation of an image
    LinkedList<PNT> m_spot; // spot point list in m_map
    int             m_minsz; // minimal spot size in pixels
    int             m_cursz; // current spot size in pixels

    LinkedList<CLR> m_old; // list of former avarage block colors
    LinkedList<CLR> m_new; // list of new average block colors
    CLR             m_ref; // reference color
    bool            m_bgc; // reference color is for background

    bool            m_busy; // busy with processing an image

    VideoCapture    *m_stream;
};

#endif // USBCAMERA_H
