/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "latchd.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "circuit.h"
#include "iopin.h"

#include "stringProp.h"
#include "boolProp.h"
#include "intProp.h"

#define tr(str) simulideTr("LatchD",str)

Component* LatchD::construct( std::string type, std::string id )
{ return new LatchD( type, id ); }

LibraryItem* LatchD::libraryItem()
{
    return new LibraryItem(
        "Latch" ,
        "Memory",
        "subc.png",
        "LatchD",
        LatchD::construct );
}

LatchD::LatchD( std::string type, std::string id )
      : LogicComponent( type, id )
{
    //m_width  = 4;
    //m_height = 10;
    //m_area = QRect(-(m_width*8/2),-(m_height*8/2), m_width*8, m_height*8 );
    
    m_useReset = false;
    m_tristate = true;
    createOePin("IR00OE", id+"-Pin_outEnable");

    m_clkPin = new IoPin(m_id+"-Pin_clock", 0, this, input );
    //m_clkPin->setLabelColor( QColor( 0, 0, 0 ) );

    m_resetPin = new IoPin(m_id+"-Pin_reset", 0, this, input );
    setupPin( m_resetPin,"L03RST");
    m_otherPin.emplace_back( m_resetPin );

    LatchD::setTrigger( InEnable );

    m_channels = 0;
    setChannels( 8 );
    setTriggerStr("Enable");


    addPropGroup( { "Main", {
        new intProp <LatchD>("Channels", "Size","_Channels", this
                    , &LatchD::channels, &LatchD::setChannels, propNoCopy,"uint" ),

        new stringProp <LatchD>("Trigger", "Trigger Type","", this
                    , &LatchD::triggerStr, &LatchD::setTriggerStr, propNoCopy,"enum" ),
        new boolProp<LatchD>("UseRS", "Reset Pin",""
                    , this, &LatchD::pinReset, &LatchD::setPinReset, propNoCopy ),
    }, groupNoCopy } );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
            new boolProp<LatchD>("Invert_Inputs", "Invert Inputs","", this
                    , &LatchD::invertInps, &LatchD::setInvertInps,propNoCopy),
            new boolProp<LatchD>("Tristate", "Tristate",LogicComponent::m_triggerList, this
                        , &LatchD::tristate, &LatchD::setTristate ),
    };
    std::vector<comProperty*> outputProps = IoComponent::outputProps();
    std::vector<comProperty*> outputType = IoComponent::outputType();
    //将四类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + customProps.size() + outputProps.size() + outputType.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), customProps.begin(), customProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());
    allProps.insert(allProps.end(), outputType.begin(), outputType.end());

    addPropGroup({ "Electric",allProps,0 });

    addPropGroup( { "Timing", IoComponent::edgeProps(),0 } );
}
LatchD::~LatchD(){}

void LatchD::stamp()
{
    if( m_trigger != Clock )
    { for( uint32_t i=0; i<m_inpPin.size(); ++i ) m_inpPin[i]->changeCallBack( this ); }
    m_resetPin->changeCallBack( this );
    LogicComponent::stamp();
}

void LatchD::voltChanged()
{
    if( m_tristate ) updateOutEnabled();
    updateClock();

    if( m_useReset && m_resetPin->getInpState() ) m_nextOutVal = 0;
    if( m_clkState == Clock_Allow )
    {
        m_nextOutVal = 0;
        for( uint32_t i=0; i<m_outPin.size(); ++i )
            if( m_inpPin[i]->getInpState() ) m_nextOutVal |= 1<<i;
    }
    scheduleOutPuts( this );
}

void LatchD::setChannels( int channels )
{
    if( channels == m_channels ) return;
    if( channels < 1 ) return;
    
    setNumInps( channels, "D");
    setNumOuts( channels );
    m_channels = channels;

    m_height = channels+2;
    //int y = -(std::fabs(m_height)/2)*8+8;

    //for( int i=0; i<channels; i++ )
    //{
    //    m_inpPin[i]->setY( y+i*8 );
    //    m_outPin[i]->setY( y+i*8 );
    //}
    //y += channels*8;
    //m_clkPin->setY( y );
    //m_oePin->setY( y );

}

void LatchD::setTristate( bool t )
{
    LogicComponent::setTristate( t );
}

void LatchD::setTrigger( trigger_t trigger )
{
    eClockedDevice::setTrigger( trigger );
}

void LatchD::setPinReset( bool r )
{
    m_useReset = r;
    if( !r ) m_resetPin->removeConnector();
}



