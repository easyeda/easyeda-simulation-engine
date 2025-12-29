/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "bcdto7s.h"
#include "itemlibrary.h"
#include "iopin.h"
#include "simulator.h"

#include "boolProp.h"
#include "stringProp.h"

#define tr(str) simulideTr("BcdTo7S",str)

Component* BcdTo7S::construct( std::string type, std::string id )
{ return new BcdTo7S( type, id ); }

LibraryItem* BcdTo7S::libraryItem()
{
    return new LibraryItem(
        "Bcd To 7S.",
        "Converters",
        "2to3g.png",
        "BcdTo7S",
        BcdTo7S::construct );
}

BcdTo7S::BcdTo7S( std::string type, std::string id )
       : BcdBase( type, id )
{
    m_width  = 4;
    m_height = 8;

    init({         // Inputs:
            "IL03S0",
            "IL04S1",
            "IL05S2",
            "IL06S3",
                    // Outputs:
            "OR01a",
            "OR02b",
            "OR03c",
            "OR04d",
            "OR05e",
            "OR06f",
            "OR07g"
        });

    createOePin( "IU01OE ", id+"-in4");
    m_resetPin = new IoPin(m_id+"-Pin_reset", 0, this, input );
    setupPin( m_resetPin,"L02RST");
    // m_resetPin->setVisible( false );
    m_otherPin.emplace_back( m_resetPin );
    // 默认重置引脚值为false
    setPinReset(false);

    setResetInv( true );    // Invert Reset Pin

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
    new stringProp<BcdTo7S>("Custom_Segments", "Custom Characters CSV",""
                        , this, &BcdTo7S::customChars, &BcdTo7S::setCustomChars, propNoCopy ),

    new boolProp<BcdTo7S>("UseRS", "Reset Pin",""
                            , this, &BcdTo7S::pinReset, &BcdTo7S::setPinReset, propNoCopy ),

    new boolProp<BcdTo7S>("Reset_Inverted", "Reset Inverted",""
                            , this, &BcdTo7S::resetInv, &BcdTo7S::setResetInv ),
    },0} );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
        new boolProp<BcdTo7S>("Invert_Inputs", "Invert Inputs","", this
            , &BcdTo7S::invertInps, &BcdTo7S::setInvertInps, propNoCopy) 
    };
    std::vector<comProperty*> tristateProps = {
        new boolProp<BcdTo7S>("Tristate", "Tristate",""
                            , this, &BcdTo7S::tristate, &BcdTo7S::setTristate ),
    };
    std::vector<comProperty*> outputProps = IoComponent::outputProps();
    std::vector<comProperty*> outputType = IoComponent::outputType();
    //将五类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + customProps.size() + outputProps.size() + outputType.size() +tristateProps.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), customProps.begin(), customProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());
    allProps.insert(allProps.end(), outputType.begin(), outputType.end());
    allProps.insert(allProps.end(), tristateProps.begin(), tristateProps.end());

     addPropGroup( { "Electric",allProps,0 } );

    addPropGroup( { "Timing", IoComponent::edgeProps(),0 } );
}
BcdTo7S::~BcdTo7S(){}

void BcdTo7S::stamp()
{
    BcdBase::stamp();

    m_resetPin->changeCallBack( this, m_useReset );
}

void BcdTo7S::updateStep()
{
        if( !m_changed ) return;
    m_changed = false;

    if( !Simulator::self()->isRunning() ) m_digit = 0;
    else                                  voltChanged();

}

void BcdTo7S::voltChanged()
{
    LogicComponent::updateOutEnabled();
    BcdBase::voltChanged();
    if( m_useReset && m_resetPin->getInpState() ) m_nextOutVal = 0;
    else                                          m_nextOutVal = m_digit;
    scheduleOutPuts( this );
}

void BcdTo7S::setPinReset( bool r )
{
    m_useReset = r;
    // m_resetPin->setVisible( r );
    if( !r ) m_resetPin->removeConnector();

    // updtProperties();
}

void BcdTo7S::setResetInv( bool inv )
{
    m_resetInv = inv;
    m_resetPin->setInverted( inv );
}