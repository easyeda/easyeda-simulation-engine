/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "mux_analog.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "circuit.h"
#include "e-resistor.h"
#include "e-node.h"
#include "iopin.h"

#include "doubleProp.h"
#include "intProp.h"

//#define tr(str) simulideTr("MuxAnalog",str)

Component* MuxAnalog::construct( std::string type, std::string id )
{ return new MuxAnalog( type, id ); }

LibraryItem* MuxAnalog::libraryItem()
{
    return new LibraryItem(
        "Analog Mux",
        "Other Active",
        "1to3-c.png",
        "MuxAnalog",
        MuxAnalog::construct );
}

MuxAnalog::MuxAnalog( std::string type, std::string id )
         : Component( type, id )
         , eElement( id )
{
    //setLabelPos(-16,-16, 0 );
    //setValLabelPos(-16,-16-10, 0 );
    
    m_zPin = new Pin(m_id+"-PinInput", 0, this );
    //m_zPin->setLabelText( "Z" );
    
    m_enPin = new Pin( m_id+"-PinEnable", 0, this );
    //m_enPin->setLabelText( "En" );
    m_enPin->setInverted( true );
    
    m_admit = 1000;
    m_addrBits = 0;
    m_channels = 0;
    setAddrBits( 3 );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
new intProp <MuxAnalog>("Address_Bits", "Address Size","_Bits", this, &MuxAnalog::addrBits,  &MuxAnalog::setAddrBits, propNoCopy,"uint" ),
new doubleProp<MuxAnalog>("Impedance"   , "Impedance"   ,"Ω"    , this, &MuxAnalog::impedance, &MuxAnalog::setImpedance ),
    },0} );
}
MuxAnalog::~MuxAnalog(){}

void MuxAnalog::stamp()
{
    m_address = 0;
    m_enabled = true;

    eNode* enode = m_zPin->getEnode();
    for( int i=0; i<m_channels; ++i )
    {
        m_ePin[i]->setEnode( enode );
        double admit = (i == 0) ? m_admit : cero_doub;
        m_resistor[i]->setAdmit( admit );
    }

    for( Pin* pin : m_addrPin ) pin->changeCallBack( this );
    m_enPin->changeCallBack( this );
}

void MuxAnalog::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    voltChanged();
}

void MuxAnalog::voltChanged()
{
    bool oldEnabled = m_enabled;
    m_enabled = m_enPin->getVoltage() < 2.5;

    int address = 0;
    for( int i=0; i<m_addrBits; ++i )
    {
        bool state = (m_addrPin[i]->getVoltage()>2.5);
        if( state ) address += pow( 2, i );
    }
    if ( oldEnabled != m_enabled || address != m_address )
        Simulator::self()->addEvent( 10000/*m_propDelay*/, this );
    m_address = address;
}

void MuxAnalog::runEvent()
{
    if( !m_enabled )
    {
        for( int i=0; i<m_channels; ++i ) m_resistor[i]->setAdmit( cero_doub );
        return;
    }

    for( int i=0; i<m_channels; ++i )
    {
        if( i == m_address )
        {    if( m_resistor[i]->admit() == cero_doub ) m_resistor[i]->setAdmit( m_admit ); }
        else if( m_resistor[i]->admit() != cero_doub ) m_resistor[i]->setAdmit( cero_doub );
}   }

void MuxAnalog::setAddrBits( int bits )
{
    if( bits == m_addrBits ) return;
    if( bits < 1 ) bits = 1;
    
    int channels = pow( 2, bits );
    
    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();
    if( bits < m_addrBits ) deleteAddrBits( m_addrBits-bits );
    else                    createAddrBits( bits-m_addrBits );

    if  ( channels < m_channels ) deleteResistors( m_channels-channels );
    else                          createResistors( channels-m_channels );
    
    m_channels = channels;
    
    int rside = m_channels*8+8;
    int size = 5*8 + bits*8;
    if( rside > size ) size = rside;

    //m_area = QRect( -2*8, 0, 4*8, size );

    //m_enPin->setPos( QPoint(-3*8,4*8+bits*8 ) );
    //m_enPin->isMoved();

    //Circuit::self()->update();
}

void MuxAnalog::createAddrBits( int c )
{
    int start = m_addrBits;
    m_addrBits = m_addrBits+c;
    m_addrPin.resize( m_addrBits );

    for( int i=start; i<m_addrBits; i++ )
    {
        m_addrPin[i] = new Pin( m_id+"-pinAddr"+std::to_string(i), 0, this);
        //m_addrPin[i]->setLabelText( "A"+std::to_string(i) );
}   }

void MuxAnalog::deleteAddrBits( int d )
{
    for( int i=m_addrBits-d; i<m_addrBits; i++ ) deletePin( m_addrPin[i] );
    m_addrBits = m_addrBits-d;
    m_addrPin.resize( m_addrBits );
}

void MuxAnalog::createResistors( int c )
{
    int start = m_channels;
    m_channels = m_channels+c;
    m_resistor.resize( m_channels );
    m_chanPin.resize( m_channels );
    m_ePin.resize( m_channels );

    for( int i=start; i<m_channels; i++ )
    {
        std::string reid = m_id+"-resistor"+std::to_string(i);
        m_resistor[i] = new eResistor( reid );
        m_ePin[i]     = new ePin( reid+"-pinL", 0 );
        m_resistor[i]->setEpin( 0, m_ePin[i] );
        
        m_chanPin[i] = new Pin(m_id+"-pinY"+std::to_string(i), 0, this);
        //m_chanPin[i]->setLabelText( "Y"+std::to_string(i) );
        m_resistor[i]->setEpin( 1, m_chanPin[i] );

        m_resistor[i]->setAdmit( cero_doub );
}   }

void MuxAnalog::deleteResistors( int d )
{
    int start = m_channels-d;

    for( int i=start; i<m_channels; ++i )
    {
        deletePin( m_chanPin[i] );
        delete m_ePin[i];
        delete m_resistor[i];
    }
    m_resistor.resize( start );
    m_chanPin.resize( start );
    m_ePin.resize( start );
}

void MuxAnalog::remove()
{
    m_zPin->removeConnector();
    m_enPin->removeConnector();
    for( Pin* pin :m_addrPin  ) pin->removeConnector();

    deleteResistors( m_channels );
    deleteAddrBits( m_addrBits );
    
    Component::remove();
}
