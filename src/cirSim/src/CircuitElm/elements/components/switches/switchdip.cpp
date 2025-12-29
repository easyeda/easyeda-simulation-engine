/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "switchdip.h"
//#include "circuitwidget.h"
#include "itemlibrary.h"
#include "connector.h"
#include "simulator.h"
#include "circuit.h"
#include "e-node.h"
#include "pin.h"
#include "custombutton.h"

#include "intProp.h"

#define tr(str) simulideTr("SwitchDip",str)

Component* SwitchDip::construct( std::string type, std::string id )
{ return new SwitchDip( type, id ); }

LibraryItem* SwitchDip::libraryItem()
{
    return new LibraryItem(
        "Switch Dip",
        "Switches",
        "switchdip.png",
        "SwitchDip",
        SwitchDip::construct);
}

SwitchDip::SwitchDip( std::string type, std::string id )
         : Component( type, id )
         , eElement( id )
{
    //m_graphical = true;
    m_changed = true;

    //m_color = QColor( 50, 50, 70 );
    m_size = 0;
    m_state = 0;
    setSize( 8 );
    //setLabelPos(-16,-44, 0);
    
    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
new intProp<SwitchDip>("Size", "Size","_Lines", this
                      , &SwitchDip::size, &SwitchDip::setSize, propNoCopy,"uint" )
    }, groupNoCopy } );

    addPropGroup( {"Hidden", {
new intProp<SwitchDip>("State", "","", this, &SwitchDip::state, &SwitchDip::setState,0,"uint" )
    }, groupHidden} );
}
SwitchDip::~SwitchDip(){
    for (CustomButton* button : m_buttons) {
        delete button; 
    }
}

void SwitchDip::stamp()
{
    for( int i=0; i<m_size; i++ )
    {
        int pin1 = i*2;
        int pin2 = pin1+1;

        eNode* node0 = m_pin[pin1]->getEnode();
        eNode* node1 = m_pin[pin2]->getEnode();

        // if( node0 ) node0->setSwitched( true );
        // if( node1 ) node1->setSwitched( true );

        m_pin[pin1]->setEnodeComp( node1 );
        m_pin[pin2]->setEnodeComp( node0 );
    }
    m_changed = true;
    updateStep();
}

void SwitchDip::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    int i = 0;
    for( CustomButton* button : m_buttons )
    {
        double admit = 0;
        if( button->isChecked()  ) admit = 1e3;
        
        int pin = i*2;
        m_pin[pin]->stampAdmitance( admit );
        m_pin[pin+1]->stampAdmitance( admit );
        i++;
    }
}

void SwitchDip::onbuttonclicked()
{
   m_changed = true;

   int i = 0;
   for( CustomButton* button : m_buttons )
   {
       if( button->isChecked()  ) 
       {
           m_state |= 1<<i;
       }else{
           m_state &= ~(1<<i);
       }
       i++;
   }
}

void SwitchDip::setState( int state )
{
    if( m_state == state ) return;
    m_state = state;

    for( CustomButton* button : m_buttons )
    {
       bool switchState = state&1;
       state >>= 1;
       
       button->setChecked( switchState );
       
    //    if( switchState  ) button->setIcon(QIcon(":/switchbut.png"));
    //    else               button->setIcon(QIcon(":/stop.svg"));
    }
    m_changed = true;
}

void SwitchDip::createSwitches( int c )
{
    int start = m_size;
    m_size = m_size+c;
    m_pin.resize( m_size*2 );

    for( int i=start; i<m_size; i++ )
    {
        int index = i*2;
        std::string butId = m_id + std::string("-switch"+std::to_string(i));
        CustomButton* button = new CustomButton(i);

        button->setChecked( true );
        //button->setIcon(QIcon(":/switchbut.png"));
        m_buttons.push_back( button );
    

        Pin* pin = new Pin(butId+"-pinP", 0, this);
        m_pin[index] = pin;

        pin = new Pin( butId+"-pinN", 0, this);
        m_pin[index+1] = pin;
        
        m_state |= 1<<i;                          // default state = on;
}   }

void SwitchDip::deleteSwitches( int d )
{
    if( d > m_size ) d = m_size;
    int start = m_size-d;

    for( int i=start*2; i<m_size*2; i++ ) deletePin( m_pin[i] );

    for( int i=start; i<m_size; i++ )
    {
        if (!m_buttons.empty()) {
            delete m_buttons.back();  
            m_buttons.pop_back();    
        }
    }
    m_size = m_size-d;
    m_pin.resize( m_size*2 );
}

void SwitchDip::updateState(int index)
{
    int length = 0;
    for( CustomButton* button : m_buttons ){
        if(button->getIndex() == index){
            button->setChecked(!button->isChecked());
        }
        length++;
    }
    if(length == m_buttons.size()){
        return;
    }
    onbuttonclicked();
}

void SwitchDip::setSize( int size )
{
    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();
    if( size == 0 ) size = 8;
    
    if     ( size < m_size ) deleteSwitches( m_size-size );
    else if( size > m_size ) createSwitches( size-m_size );
}

void SwitchDip::remove()
{
    deleteSwitches( m_size );
    Component::remove();
}