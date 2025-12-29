/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "csource.h"
#include "simulator.h"
#include "itemlibrary.h"
#include "pin.h"

#include "doubleProp.h"
#include "boolProp.h"

#define tr(str) simulideTr("Csource",str)

Component* Csource::construct( std::string type, std::string id )
{ return new Csource( type, id ); }

LibraryItem* Csource::libraryItem()
{
    return new LibraryItem(
        "Controlled Source",
        "Sources",
        "csource.png",
        "Csource",
        Csource::construct );
}

Csource::Csource( std::string type, std::string id )
       : Component( type, id )
       , eResistor( id )
{
    //m_area = QRect(-16,-16, 32, 32 );

    m_pin.resize(4);
    // Control Pins
    m_pin[0] = new Pin( id+"-cpPin", 0, this);
    m_pin[1] = new Pin( id+"-cmPin", 1, this);

    m_ePin[0] = m_pin[0];
    m_ePin[1] = m_pin[1];
    // Source Pins
    m_pin[2] = new Pin(  id+"-s1Pin", 0, this);
    m_pin[3] = new Pin( id+"-s2Pin", 1, this);

    m_controlPins = true;
    m_currSource  = true;
    m_currControl = false;
    m_volt = 5;
    m_curr = 1;
    m_gain = 1;

    //setLabelPos( 4,-28, 0 );
    //setValLabelPos( 4, 18, 0 ); // x, y, rot

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
new boolProp<Csource>( "CurrSource"  , "Current Source"    ,"" , this, &Csource::currSource , &Csource::setCurrSource ),
new boolProp<Csource>( "CurrControl" , "Current Controlled","" , this, &Csource::currControl, &Csource::setCurrControl, propNoCopy ),
new boolProp<Csource>( "Control_Pins", "Use Control Pins"  ,"" , this, &Csource::controlPins, &Csource::setControlPins, propNoCopy ),
new doubleProp<Csource>( "Gain"        , "Gain"              ,"" , this, &Csource::gain       , &Csource::setGain ),
new doubleProp<Csource>( "Voltage"     , "Voltage"           ,"V", this, &Csource::volt       , &Csource::setVolt ),
new doubleProp<Csource>( "Current"     , "Current"           ,"A", this, &Csource::outCurrent , &Csource::setOutCurrent )
    }, 0 } );
}
Csource::~Csource() {}

void Csource::stamp()
{
    if( m_currControl ) m_admit = 1/cero_doub;
    else                m_admit = cero_doub;
    eResistor::stamp();

    m_pin[2]->setEnodeComp( m_pin[3]->getEnode() );
    m_pin[3]->setEnodeComp( m_pin[2]->getEnode() );
    m_pin[2]->createCurrent();
    m_pin[3]->createCurrent();

    m_lastCurr = 0;
    m_changed = true;
    updateStep();
}

void Csource::voltChanged()
{
    double volt = m_pin[0]->getVoltage() - m_pin[1]->getVoltage();
    setVoltage( volt );
}

void Csource::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    udtProperties();

    // m_pin[0]->setEnabled( m_controlPins );  
    // m_pin[0]->setVisible( m_controlPins );
    // m_pin[1]->setEnabled( m_controlPins );
    // m_pin[1]->setVisible( m_controlPins );

    if( m_currControl )
    {
        m_admit = 1/cero_doub;

    }
    else
    {
        m_admit = cero_doub;

    }
    eResistor::stampAdmit();

    if( m_currSource )
    {
        m_pin[2]->stampAdmitance( 0 );
        m_pin[3]->stampAdmitance( 0 );
    }else{
        m_pin[2]->stampAdmitance( 1/cero_doub );
        m_pin[3]->stampAdmitance( 1/cero_doub );
    }

    if( !m_controlPins && !m_linkedTo )
    {
        m_pin[0]->removeConnector();
        m_pin[1]->removeConnector();

        if( m_currSource )
        {
            m_pin[2]->stampCurrent(-m_curr );
            m_pin[3]->stampCurrent( m_curr );
        }else{
            m_pin[2]->stampCurrent( m_volt/cero_doub );
            m_pin[3]->stampCurrent(-m_volt/cero_doub );
        }
    }
    else voltChanged();

    bool connected = m_pin[0]->isConnected() && m_pin[1]->isConnected();
    m_pin[0]->changeCallBack( this, connected && m_controlPins );
    m_pin[1]->changeCallBack( this, connected && m_controlPins );
    //update();
}

void Csource::setVoltage( double v )
{
    double curr = v;
    //if( qFabs( curr - m_lastCurr ) < 1e-5 ) return;
    m_lastCurr = curr;

    if( m_currSource   ) curr = -curr;      // Current source
    else if( curr != 0 ) curr /= cero_doub; // Voltage source
    if( m_currControl  ) curr *= m_admit;   // Current controlled

    curr *= m_gain;
    m_pin[2]->stampCurrent( curr );
    m_pin[3]->stampCurrent(-curr );
}

bool Csource::setLinkedTo(Linkable *li)
{
    bool linked = Component::setLinkedTo( li );
    setControlPins( m_controlPins );
    return linked;
}

void Csource::setLinkedValue(double v, int i)
{
    if( m_currControl ) v /= m_admit;
    setVoltage( v );
}

void Csource::setGain( double g )
{
    m_gain = g;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Csource::setVolt( double volt )
{
    if( volt < 0 ) volt = 0;
    m_volt = volt;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Csource::setOutCurrent( double c )
{
    if( c < 0 ) c = 0;
    m_curr = c;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Csource::setCurrSource( bool c )
{
    m_currSource = c;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

 void Csource::setCurrControl( bool c )
 {
     m_currControl = c;
     m_changed = true;
     if( !Simulator::self()->isRunning() ) updateStep();
 }

void Csource::setControlPins( bool set )
{
    // int length = set ? 8 : 10;
    //m_pin[2]->setLength( length );
    //m_pin[3]->setLength( length );

    m_controlPins = set;
    m_changed = true;
    if( !Simulator::self()->isRunning() ) updateStep();
}

void Csource::udtProperties()
{
    //if( !m_propDialog ) return;
    bool controlled = m_controlPins || m_linkedTo; // Controlled by pins or Linked

    //m_propDialog->showProp("Voltage"    , !controlled && !m_currSource );
    //m_propDialog->showProp("Current"    , !controlled &&  m_currSource );
    //m_propDialog->showProp("CurrControl",  controlled );
    //m_propDialog->showProp("Gain"       ,  controlled );
}

//void Csource::slotProperties()
//{
//    Component::slotProperties();
//    udtProperties();
//}
