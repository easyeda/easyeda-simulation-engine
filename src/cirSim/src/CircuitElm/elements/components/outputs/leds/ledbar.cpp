/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "ledbar.h"
#include "itemlibrary.h"
#include "connector.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"

#include "doubleProp.h"
#include "stringProp.h"
#include "boolProp.h"
#include "intProp.h"

#define tr(str) simulideTr("LedBar",str)

Component* LedBar::construct( std::string type, std::string id )
{ return new LedBar( type, id ); }

LibraryItem* LedBar::libraryItem()
{
    return new LibraryItem(
        "Led Bar",
        "Leds",
        "ledbar.png",
        "LedBar",
        LedBar::construct);
}

LedBar::LedBar( std::string type, std::string id )
      : Component( type, id )
{
    //m_area = QRect(-8,-28, 16, 64 );
    //m_graphical = true;
    //m_color = QColor(0,0,0);
    m_size = 0;
    setSize( 8 );
    setRes( 0.6 );
    //setLabelPos(-16,-44, 0);
    //setValLabelPos(-16,-44-12, 0);

    addPropGroup( { "Main", {
        new stringProp <LedBar>("Color", "Color","" , this
                        , &LedBar::colorStr, &LedBar::setColorStr,0,"enum" ),

        new intProp <LedBar>("Size" , "Size","_Leds", this
                    , &LedBar::size, &LedBar::setSize, propNoCopy ),

        new boolProp<LedBar>("Grounded", "Grounded","" , this
                    , &LedBar::grounded, &LedBar::setGrounded, propNoCopy ),
    }, 0} );

    addPropGroup( { "Electric", {
        new doubleProp<LedBar>("Threshold" , "Forward Voltage","V", this
                        , &LedBar::threshold,  &LedBar::setThreshold ),

        new doubleProp<LedBar>("MaxCurrent", "Max Current"    ,"A", this
                        , &LedBar::maxCurrent, &LedBar::setMaxCurrent ),

        new doubleProp<LedBar>("Resistance", "Resistance"     ,"Ω", this
                        , &LedBar::res,        &LedBar::setRes ),
    }, 0} );
}
LedBar::~LedBar(){}

void LedBar::createLeds( int c )
{
    bool initialized = m_size > 0;
    int start = m_size;
    m_size = m_size+c;
    m_led.resize( m_size );
    m_pin.resize( m_size*2 );
    
    for( int i=start; i<m_size; i++ )
    {
        int index = i*2;
        
        std::string ledid = m_id+"-led"+std::to_string(i);

        Pin* pin0 = new Pin(ledid+"-pinP", 0, this );
        m_pin[index] = pin0;
        
        Pin* pin1 = new Pin(ledid+"-pinN", 0, this );
        m_pin[index+1] = pin1;

        m_led[i] = new LedSmd( "LEDSMD", ledid, pin0, pin1 );
        ////图形界面相关
        //m_led[i]->setParentItem(this); 
        //m_led[i]->setPos( 0,-28+2+i*8 );
        //m_led[i]->setFlag( QGraphicsItem::ItemIsSelectable, false );
        //m_led[i]->setAcceptedMouseButtons(0);
        
        if( initialized ){
            m_led[i]->setGrounded( grounded() );
            m_led[i]->setRes( res() );
            m_led[i]->setMaxCurrent( maxCurrent() ); 
            m_led[i]->setThreshold( threshold() );
            m_led[i]->setColorStr( colorStr() );
}   }   }

void LedBar::deleteLeds( int d )
{
    if( d > m_size ) d = m_size;
    int start = m_size-d;
    
    if( grounded() )
    { for( int i=start; i<m_size; i++ ) m_led[i]->setGrounded( false ); }

    for( int i=start*2; i<m_size*2; i++ ) deletePin( m_pin[i] );
    for( int i=start;   i<m_size;   i++ ) delete m_led[i];
    m_size = m_size-d;
    m_led.resize( m_size );
    m_pin.resize( m_size*2 );
}

std::string LedBar::colorStr()
{
    if( m_led[0] ) return m_led[0]->colorStr();
    else           return "Yellow";
}

void LedBar::setColorStr( std::string color )
{
    for( LedSmd* led : m_led ) led->setColorStr( color );
}

std::vector<std::string> LedBar::getEnumUids( std::string ) { return m_led[0]->getEnumUids(""); }
std::vector<std::string> LedBar::getEnumNames( std::string ){ return m_led[0]->getEnumNames(""); }

void LedBar::setSize( int size )
{
    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    if( size < 1 ) size = 1;
    if     ( size < m_size ) deleteLeds( m_size-size );
    else if( size > m_size ) createLeds( size-m_size );
    
    //m_area = QRect(-8,-28, 16, m_size*8 );

    //Circuit::self()->update();
}

double LedBar::threshold() { return m_led[0]->threshold(); }

void LedBar::setThreshold( double threshold ) 
{ for( int i=0; i<m_size; i++ ) m_led[i]->setThreshold( threshold ); }

double LedBar::maxCurrent() { return m_led[0]->maxCurrent(); }

void LedBar::setMaxCurrent( double current ) 
{ for( int i=0; i<m_size; i++ ) m_led[i]->setMaxCurrent( current ); }

double LedBar::res() { return m_led[0]->res(); }

void LedBar::setRes( double resist )
{ for( int i=0; i<m_size; i++ ) m_led[i]->setRes( resist ); }

bool LedBar::grounded() { return m_led[0]->grounded(); }

void LedBar::setGrounded( bool grounded )
{ for( int i=0; i<m_size; i++ ) m_led[i]->setGrounded( grounded ); }

//void LedBar::setHidden( bool hid, bool hidArea, bool hidLabel )
//{
//    Component::setHidden( hid, hidArea, hidLabel );
//    if( hid )
//    {
//        m_area = QRect( 2,-26, 4, m_size*8-4 ); // -2 pixels so boundingRect fits exactly
//        for( int i=0; i<m_size; i++ )
//            m_led[i]->setPos( 2,-28+2+i*8 );
//    }else{
//        m_area = QRect(-8,-28, 16, m_size*8 );
//        for( int i=0; i<m_size; i++ )
//        {
//            m_led[i]->setPos( 0,-28+2+i*8 );
//            if( m_led[0]->grounded() ) m_pin[i*2+1]->setVisible( false );
//        }
//    }
//}

void LedBar::remove()
{
    deleteLeds( m_size );
    Component::remove();
}
