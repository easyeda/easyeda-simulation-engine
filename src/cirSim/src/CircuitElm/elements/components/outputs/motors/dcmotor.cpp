/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "dcmotor.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "pin.h"
#include "label.h"

#include "doubleProp.h"
#include "intProp.h"

#define tr(str) simulideTr("DcMotor",str)

Component* DcMotor::construct( std::string type, std::string id )
{ return new DcMotor( type, id ); }

LibraryItem* DcMotor::libraryItem()
{
    return new LibraryItem(
       "Dc Motor",
        "Motors",
        "dcmotor.png",
        "DcMotor",
        DcMotor::construct );
}

DcMotor::DcMotor( std::string type, std::string id )
       : LinkedComponent( type, id )
       , eResistor( id )
{
    //m_graphical = true;
    //
    //m_area = QRectF( -35,-33, 70, 66 );
    //m_color = QColor( 50, 50, 70 );
    m_ang  = 0;
    m_speed = 0;
    m_voltNom = 5;
    setRpm( 60 );

    m_pin.resize( 2 );
    m_ePin[0] = m_pin[0] = new Pin(m_id+"-lPin", 0, this);
    //m_pin[0]->setLength( 4 );
    //m_pin[0]->setFontSize( 9 );
    //m_pin[0]->setSpace( 1.7 );
    //m_pin[0]->setLabelText("+");

    m_ePin[1] =m_pin[1] = new Pin( m_id+"-rPin", 1, this);
    //m_pin[1]->setLength( 4 );
    //m_pin[1]->setFontSize( 9 );
    //m_pin[1]->setSpace( 1.7 );
    //m_pin[1]->setLabelText("–");  // U+2013

    //setShowId( true );
    //setLabelPos(-22,-48, 0);
    //setValLabelPos(-14, 36, 0);

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
new intProp <DcMotor>( "RPM_Nominal" , "Nominal Speed"  ,"_RPM", this, &DcMotor::rpm,    &DcMotor::setRpm ),
new doubleProp<DcMotor>( "Volt_Nominal", "Nominal Voltage","V"   , this, &DcMotor::volt,   &DcMotor::setVolt ),
new doubleProp<DcMotor>( "Resistance"  , "Resistance"     ,"Ω"   , this, &DcMotor::getRes, &DcMotor::setResSafe )
    },0} );
}
DcMotor::~DcMotor(){}

void DcMotor::initialize()
{
    m_ang = 0;
    m_speed = 0;
    m_delta = 0;
    m_lastTime = 0;
    m_updtTime = 0;
    m_LastVolt = 0;
}

void DcMotor::stamp()
{
     if( m_ePin[0]->isConnected() && m_ePin[1]->isConnected() )
     {
         m_ePin[0]->changeCallBack( this );
         m_ePin[1]->changeCallBack( this );
     }
     eResistor::stamp();
}

void DcMotor::updateStep()
{
    updatePos();

    if( m_updtTime ) m_speed = m_delta/(m_updtTime/1e12);

    m_ang += m_motStPs*m_delta;
    m_ang = remainder( m_ang, (16.0*360.0) );

    if(  m_linkedComp.size() )
    {
        double val = m_ang*1000/(16.0*360.0);
        if( val > 0 ) val = 1000-val;
        else          val = -val;
        for( Component* comp : m_linkedComp ) comp->setLinkedValue( val ); // 0-1000
    }
    m_delta = 0;
    m_updtTime = 0;
    //update();
}

void DcMotor::voltChanged() { updatePos(); }

void DcMotor::updatePos()
{
    uint64_t timePS = Simulator::self()->circTime();
    uint64_t duration = timePS-m_lastTime;
    m_updtTime += duration;
    m_lastTime = timePS;

    m_delta += (m_LastVolt/m_voltNom)*(duration/1e12);
    m_LastVolt = m_ePin[1]->getVoltage() - m_ePin[0]->getVoltage();
}

void DcMotor::setRpm( int rpm )
{
    if( rpm < 1 ) rpm = 1;
    m_rpm = rpm;
    m_motStPs = 16*360*rpm/60;

    //update();
}

