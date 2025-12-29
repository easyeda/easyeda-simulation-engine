/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "sevensegment.h"
//#include "circuitwidget.h"
#include "itemlibrary.h"
#include "connector.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"

#include "doubleProp.h"
#include "stringProp.h"
#include "boolProp.h"
#include "intProp.h"

//#define tr(str) simulideTr("SevenSegment",str)

Component* SevenSegment::construct( std::string type, std::string id )
{ return new SevenSegment( type, id ); }

LibraryItem* SevenSegment::libraryItem()
{
    return new LibraryItem(
        "7 Segment",
        "Leds",
        "seven_segment.png",
        "Seven Segment",
        SevenSegment::construct );
}

SevenSegment::SevenSegment( std::string type, std::string id )
            : Component( type, id )
            , eElement( id )
{
    //m_graphical = true;

    //setLabelPos( 20,-44, 0 );

    //m_color = QColor(0,0,0);
    m_commonCathode = true;
    // m_verticalPins  = false;
    m_numDisplays = 0;
    m_threshold  = 2.4;
    m_maxCurrent = 0.02;
    m_resistance = 1;

    m_ePin.resize(8);
    m_pin.resize(8);

    std::string pinid;
    for( int i=0; i<7; ++i ) // Create Pins for 7 segments
    {
        pinid = m_id+"-pin_"+ static_cast<char>( 97+i ); // a..g
        m_ePin[i] = m_pin[i] = new Pin( pinid, 0, this );
    }
    // Pin dot
    m_ePin[7] = m_pin[7] = new Pin(m_id+"-pin_dot", 0, this );

    setNumDisplays( 1 );

    addPropGroup( { "Main", {
        new stringProp <SevenSegment>("Color", "Color","", this, &SevenSegment::colorStr, &SevenSegment::setColorStr,0,"enum" ),
        new intProp <SevenSegment>("NumDisplays"  , "Size","_7seg", this, &SevenSegment::numDisplays, &SevenSegment::setNumDisplays, propNoCopy ),
    }, 0} );
    addPropGroup( { "Electric", {
        new boolProp<SevenSegment>("CommonCathode", "Common Cathode" ,"",  this, &SevenSegment::isComCathode, &SevenSegment::setComCathode, propNoCopy ),
        new doubleProp<SevenSegment>("Threshold" , "Forward Voltage","V", this, &SevenSegment::threshold,  &SevenSegment::setThreshold ),
        new doubleProp<SevenSegment>("MaxCurrent", "Max Current"    ,"A", this, &SevenSegment::maxCurrent, &SevenSegment::setMaxCurrent ),
        new doubleProp<SevenSegment>("Resistance", "Resistance"     ,"Ω", this, &SevenSegment::resistance, &SevenSegment::setResistance ),
    },0} );
}
SevenSegment::~SevenSegment() { }

void SevenSegment::stamp()
{
    for( int i=0; i<8; ++i ) m_enode[i] = m_ePin[i]->getEnode(); // Get eNode of pin i
    for( int i=0; i<m_numDisplays; ++i )
    {
        eNode* commonEnode = m_commonPin[i]->getEnode();     // Get eNode of common

        int pin;
        for( int j=0; j<8; ++j )
        {
            pin = i*8+j;
            if( m_commonCathode )
            {
                m_cathodePin[pin]->setEnode( commonEnode );
                m_anodePin[pin]->setEnode( m_enode[j] );
            }else{
                m_anodePin[pin]->setEnode( commonEnode );
                m_cathodePin[pin]->setEnode( m_enode[j] );
}   }   }   }

std::string SevenSegment::getElementStatusData()
{
   std::string night = "";
   for(auto segment: m_segment){
    night = night+" "+ std::to_string(segment->intensity());
   }
   return night;
}

std::string SevenSegment::colorStr()
{
    if( m_segment[0] ) return m_segment[0]->colorStr();
    else               return "Yellow";
}

void SevenSegment::setColorStr( std::string color )
{
    for( LedSmd* segment : m_segment ) segment->setColorStr( color );
}

std::vector<std::string> SevenSegment::getEnumUids( std::string ) { return m_segment[0]->getEnumUids(""); }
std::vector<std::string> SevenSegment::getEnumNames( std::string ){ return m_segment[0]->getEnumNames(""); }

void SevenSegment::setNumDisplays( int displays )
{
    if( displays < 1 ) displays = 1;
    if( displays == m_numDisplays ) return;

    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    //m_area = QRect(-18,-24-4, 32*displays+4, 48+8 );

    if( displays > m_numDisplays )
    {
        resizeData( displays );
        for( int i=m_numDisplays; i<displays; ++i ) createDisplay( i );
    }else{
        for( int i=displays; i<m_numDisplays; ++i ) deleteDisplay( i );
        resizeData( displays );
    }
    m_numDisplays = displays;
    setResistance( m_resistance );
    setThreshold( m_threshold );
    setMaxCurrent( m_maxCurrent );

    //Circuit::self()->update();
}

void SevenSegment::resizeData( int displays )
{
    m_pin.resize( 8+displays );
    m_commonPin.resize( displays );
    m_cathodePin.resize( displays*8 );
    m_anodePin.resize( displays*8 );
    m_segment.resize( displays*8 );
}

void SevenSegment::setComCathode( bool isCommonCathode )
{
    if( Simulator::self()->isRunning() ) 
        Simulator::self()->stopSim();
    m_commonCathode = isCommonCathode;            //共阳极，共阴极
    std::string label = m_commonCathode ? "|" : "+";
    // for( int i=0; i<m_numDisplays; ++i )
    //    m_commonPin[i]->setLabelText( label );
}
        
void SevenSegment::setResistance( double res )
{
    if( res < 1e-6 ) res = 1;
    m_resistance = res;
    for( uint32_t i=0; i<m_segment.size(); ++i ) m_segment[i]->setRes( res );
}

void SevenSegment::setThreshold( double threshold )
{
    if( threshold < 1e-6 ) threshold = 2.4;
    m_threshold = threshold;
    for( uint32_t i=0; i<m_segment.size(); ++i ) m_segment[i]->setThreshold( threshold );
}

void SevenSegment::setMaxCurrent( double current )
{
    if( current < 1e-6 ) current = 0.02;
    m_maxCurrent = current;
    for( uint32_t i=0; i<m_segment.size(); ++i ) m_segment[i]->setMaxCurrent( current );
}

void SevenSegment::deleteDisplay(int n )
{
    deletePin( m_commonPin[n] );
    for( int i=0; i<8; ++i ) delete m_segment[n*8+i];
}

void SevenSegment::createDisplay(int n )
{
    int x = 32*n;

    // Pin common
    std::string label = m_commonCathode ? "|" : "+";
    std::string pinid = m_id+"-pin_common"+ static_cast<char>( 97+n );
    m_commonPin[n] = m_pin[8+n]= new Pin(pinid, 0, this );
    //m_commonPin[n]->setLength( 4 );
    //m_commonPin[n]->setSpace( 1 );
    //m_commonPin[n]->setFontSize( 4 );
    //m_commonPin[n]->setLabelText( label );

    for( int i=0; i<8; ++i ) // Create segments
    {
        pinid = m_id+"-led_"+ static_cast<char>( 97+i );
        LedSmd* lsmd;
        if( i<7 ) lsmd = new LedSmd( "LEDSMD", pinid); // Segment
        else      lsmd = new LedSmd( "LEDSMD", pinid);  // Point

        //lsmd->setParentItem(this);
        //lsmd->setFlag( QGraphicsItem::ItemIsSelectable, false );
        //lsmd->setAcceptedMouseButtons( 0 );
        lsmd->setRes( m_resistance );
        lsmd->setMaxCurrent( m_maxCurrent );
        lsmd->setThreshold( m_threshold );
        lsmd->setColorStr( colorStr() );

        m_anodePin  [n*8+i] = lsmd->getEpin(0);
        m_cathodePin[n*8+i] = lsmd->getEpin(1);
        m_segment   [n*8+i] = lsmd;
    }

    //设置贴片led灯的位置，用于显示
    //m_segment[n*8+0]->setPos( x-5, -20 );
    //m_segment[n*8+1]->setPos( x+11.5, -16 );
    //m_segment[n*8+1]->setRotation(96);
    //m_segment[n*8+2]->setPos( x+10, 3 );
    //m_segment[n*8+2]->setRotation(96);
    //m_segment[n*8+3]->setPos( x-8, 19 );
    //m_segment[n*8+4]->setPos( x-9, 3 );
    //m_segment[n*8+4]->setRotation(96);
    //m_segment[n*8+5]->setPos( x-7.5, -16 );
    //m_segment[n*8+5]->setRotation(96);
    //m_segment[n*8+6]->setPos( x-6.5, 0 );
    //m_segment[n*8+7]->setPos( x+12, 19 );
}

