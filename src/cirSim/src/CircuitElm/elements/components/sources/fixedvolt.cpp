/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "fixedvolt.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "iopin.h"
//#include "custombutton.h"

#include "doubleProp.h"
#include "boolProp.h"

#define tr(str) simulideTr("FixedVolt",str)

Component* FixedVolt::construct( std::string type, std::string id )
{ return new FixedVolt( type, id ); }

LibraryItem* FixedVolt::libraryItem()
{
    return new LibraryItem(
        "Fixed Volt.",
        "Sources",
        "voltage.png",
        "Fixed Voltage",
        FixedVolt::construct );
}

void FixedVolt::setSmall(bool s)
{
        m_small = s;
}

FixedVolt::FixedVolt( std::string type, std::string id )
          : Component( type, id )
          , eElement( id )
{
    //m_area = QRect( -10, -10, 20, 20 );

    //m_graphical = true;
    m_changed   = false;

    m_pin.resize(1);
    m_pin[0] = m_outPin = new IoPin(id+"-outnod", 0, this, source );

    //setValLabelPos(-16, 8 , 0 ); // x, y, rot 
    //setLabelPos(-64,-24 );

    //m_button = new CustomButton();
    //m_button->setMaximumSize( 16,16 );
    //m_button->setGeometry(-20,-16,16,16);
    //m_button->setCheckable( true );

    //m_proxy = Circuit::self()->addWidget( m_button );
    //m_proxy->setParentItem( this );
    //m_proxy->setPos( QPoint(-32, -8) );

    setSmall( false );
    setVolt( 5.0 );
    
    Simulator::self()->addToUpdateList( this );

    //QObject::connect( m_button, &CustomButton::clicked, [=](){ onbuttonclicked(); } );

    addPropGroup( { "Main", {
        new doubleProp<FixedVolt>( "Voltage", "Voltage","V"
                    , this, &FixedVolt::volt, &FixedVolt::setVolt ),
        new boolProp<FixedVolt>("Small", "Small size", ""
                    , this, &FixedVolt::isSmall, &FixedVolt::setSmall ),
            }, 0} );

    addPropGroup( { "Hidden", {
        new boolProp<FixedVolt>( "Out", "",""
                    , this, &FixedVolt::out, &FixedVolt::setOut ),
    }, groupHidden} );
}
FixedVolt::~FixedVolt(){}

void FixedVolt::stamp()
{
    if( !Simulator::self()->isPaused() ) m_changed = true;
}

void FixedVolt::updateStep()
{
    if( !m_changed ) return;
    //这段可以考虑下优化，将ctl直接注入setOutState,有待后续完全改写完成后看适应性
    if (ctl)
    {
        m_outPin->setOutState(true);    //有按钮  ---------------代表更新电压值
        ctl = false;
    }
    m_changed = false;
    //update();
}

void FixedVolt::onbuttonclicked()
{
    if( Simulator::self()->isRunning() ) m_changed = true;
    else
    {
        m_outPin->setOutState(false);
        //update();
    }
}

void FixedVolt::updateOutput()
{
    m_outPin->setOutHighV( m_voltage );
    m_changed = true;
}

bool FixedVolt::out() { return ctl; }   //--------------------------原本是有按钮控制的,目前用ctl控制

void FixedVolt::setOut( bool out )   //设置带稳压源的开关的通断
{
    //m_button->setChecked( out );
    ctl = out;
    onbuttonclicked();
}

void FixedVolt::setVolt( double v )
{
    m_voltage = v;
    m_outPin->setOutHighV( m_voltage );
    m_changed = true;
}

