/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "bincounter.h"
#include "itemlibrary.h"
#include "connector.h"
#include "iopin.h"

#include "intProp.h"
#include "boolProp.h"

#define tr(str) simulideTr("BinCounter",str)

Component *BinCounter::construct( std::string type, std::string id)
{ return new BinCounter( type, id); }

LibraryItem* BinCounter::libraryItem()
{
    return new LibraryItem(
        "Binary Counter",
        "Arithmetic",
        "2to1.png",
        "Counter",
        BinCounter::construct );
}

BinCounter::BinCounter( std::string type, std::string id)
          : LogicComponent( type, id )
{
    m_TopValue = 1;
    m_width  = 3;
    m_height = 3;

    init({         // Inputs:
            "IL01>",
            "IL02R",
            "IU01S",
                   // Outputs:
            "OR01Q"
        });
    
    m_clkPin   = m_inpPin[0];     // eClockedDevice
    m_resetPin = m_inpPin[1];
    m_setPin   = m_inpPin[2];

    setSrInv( true );            // Invert Reset Pin
    useSetPin( false );          // Don't use Set Pin

    addPropGroup( { "Main", {
        new boolProp<BinCounter>("Pin_SET", "Use Set Pin","", this
                                , &BinCounter::pinSet,&BinCounter::useSetPin, propNoCopy ),
        new boolProp<BinCounter>("Clock_Inverted", "Clock Inverted"    ,"", this, &BinCounter::clockInv, &BinCounter::setClockInv ),
        new boolProp<BinCounter>("Reset_Inverted", "Set/Reset Inverted","", this, &BinCounter::srInv,    &BinCounter::setSrInv ),
        new intProp <BinCounter>("Max_Value",      "Count to"         ,"", this, &BinCounter::maxVal,   &BinCounter::setMaxVal,0,"uint" ),
    },groupNoCopy} );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> outputProps = IoComponent::outputProps();
    //将两类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + outputProps.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());

    addPropGroup({ "Electric",allProps,0 });
    addPropGroup( { "Timing"   , IoComponent::edgeProps(),0 } );
}
BinCounter::~BinCounter(){}

void BinCounter::stamp()
{
    m_Counter = 0;
    m_resetPin->changeCallBack( this );
    m_setPin->changeCallBack( this );
    LogicComponent::stamp();
}

void BinCounter::voltChanged()
{
    updateClock();
    bool clkRising = ( m_clkState == Clock_Rising );

    if( m_resetPin->getInpState() ) // Reset
    {
       m_Counter = 0;
       m_nextOutVal = 0;
    }
    else if( m_setPin->getInpState() ) // Set
    {
       m_Counter = m_TopValue;
       m_nextOutVal = 1;
    }
    else if( clkRising )
    {
        m_Counter++;

        if(      m_Counter == m_TopValue ) m_nextOutVal = 1;
        else if( m_Counter >  m_TopValue )
        {
            m_Counter = 0;
            m_nextOutVal = 0;
    }   }
    IoComponent::scheduleOutPuts( this );
}

void BinCounter::setSrInv( bool inv )
{
    m_resetInv = inv;
    m_resetPin->setInverted( inv );

    if( m_pinSet ) m_setPin->setInverted( inv );
    else           m_setPin->setInverted( false );
}

void BinCounter::useSetPin( bool set )
{
    m_pinSet = set;
    if( !set ) m_setPin->removeConnector();

    //m_setPin->setVisible( set );
    setSrInv( m_resetInv );
}
