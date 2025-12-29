/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "dht22.h"
#include "iopin.h"
#include "simulator.h"
#include "circuit.h"
#include "itemlibrary.h"

#include "doubleProp.h"
#include "stringProp.h"

#define tr(str) simulideTr("Dht22",str)

Component* Dht22::construct( std::string type, std::string id )
{ return new Dht22( type, id ); }

LibraryItem* Dht22::libraryItem()
{
    return new LibraryItem(
        "DHt22/11",
        "Sensors",
        "dht22.png",
        "Dht22",
        Dht22::construct );
}

Dht22::Dht22( std::string type, std::string id )
     : Component( type, id )
     , eElement( id )
{
    //m_area = QRect(-28,-20, 56, 40 );

    //m_graphical = true;
    m_temp = 22.5;
    m_humi = 68.5;
    m_tempInc = 0.5;
    m_humiInc = 5;
    m_set = true;

    m_enumUids = { "DHT11", "DHT22" };
    m_enumNames = { "DHT11", "DHT22" };

    m_pin.resize(4);
    m_pin[0] = m_inpin = new IoPin(id+"-inPin", 0, this );
    m_inpin->setOutHighV( 5 );
    m_pin[1] = new Pin(id+"-vccPin", 0, this );
    m_pin[1]->setUnused( true );
    m_pin[2] = new Pin(id+"-gdnPin", 0, this );
    m_pin[2]->setUnused( true );
    m_pin[3] = new Pin(id+"-ncPin", 0, this );
    m_pin[3]->setUnused( true );

    //m_font.setFamily("Ubuntu Mono");
    //m_font.setBold( true );
    //m_font.setLetterSpacing( QFont::PercentageSpacing, 100 );
    //setLabelPos(-28,-32, 0);
    setModel( "DHT22" );

    //m_button = new QPushButton( );
    //m_button->setMaximumSize( 6, 23);
    //m_button->setGeometry( 0, 0, 6, 23);
    //m_button->setCheckable( true );
    //m_button->setIconSize( QSize( 6, 23) );
    //m_button->setIcon(QIcon(":/up.png"));

    //QPushButton* u_button = new QPushButton();
    //u_button->setCursor( Qt::PointingHandCursor );
    //u_button->setMaximumSize( 9, 9 );
    //u_button->setGeometry(-5,-5, 9, 9);
    //u_button->setCheckable( false );
    //u_button->setIcon(QIcon(":/su.png"));

    //QPushButton* d_button = new QPushButton();
    //d_button->setCursor( Qt::PointingHandCursor );
    //d_button->setMaximumSize( 9, 9 );
    //d_button->setGeometry(-5,-5, 9, 9);
    //d_button->setCheckable( false );
    //d_button->setIcon(QIcon(":/giu.png"));

    //m_proxy = Circuit::self()->addWidget( m_button );
    //m_proxy->setParentItem( this );
    //m_proxy->setPos( QPoint(-23,-8) );

    //m_proxy = Circuit::self()->addWidget( u_button );
    //m_proxy->setParentItem( this );
    //m_proxy->setPos( QPoint( 17,-7) );

    //m_proxy = Circuit::self()->addWidget( d_button );
    //m_proxy->setParentItem( this );
    //m_proxy->setPos( QPoint( 17, 6) );

    //QObject::connect( m_button, &QPushButton::clicked, [=](){ onbuttonclicked(); } );
    //QObject::connect( u_button, &QPushButton::pressed, [=](){ upbuttonclicked(); } );
    //QObject::connect( d_button, &QPushButton::pressed, [=](){ downbuttonclicked(); } );

    addPropGroup( { "Main", {
new stringProp <Dht22>("DHT22"  , "Model"           ,""  , this, &Dht22::model,    &Dht22::setModel, propNoCopy,"enum"  ),
new doubleProp<Dht22>("TempInc", "Temp. increment" ,"°C", this, &Dht22::tempInc,  &Dht22::setTempInc ),
new doubleProp<Dht22>("HumiInc", "Humid. increment","_%", this, &Dht22::humidInc, &Dht22::setHumidInc ),
    },0} );
}
Dht22::~Dht22(){}

void Dht22::stamp()   // Called at Simulation Start
{
    m_outStep = 0;
    m_bitStep = 0;
    m_lastIn = true;
    m_inpin->setPinMode( input );
    m_inpin->changeCallBack( this, true );
}

void Dht22::voltChanged()                              // Called when Input Pin changes
{
    bool inState = m_inpin->getVoltage() > 2.5;
    if( m_lastIn && !inState ) m_lastTime = Simulator::self()->circTime(); // Falling edge
    else if( !m_lastIn && inState )                                        // Rising edge
    {
        uint64_t time = Simulator::self()->circTime()-m_lastTime; // in picosecondss
        if( time > m_start )                              // Minimum input pulse
        {
            m_inpin->setPinMode( openCo );
            m_inpin->scheduleState( true, 0 );
            m_inpin->changeCallBack( this, false ); // Stop receiving voltChange() CallBacks
            Simulator::self()->addEvent( 30*1e6, this ); // Send ack after 30 us
    }   }
    m_lastIn = inState;
}

void Dht22::runEvent()
{
    if( m_outStep == 0 )       // Send ACK
    {
        m_bit = 1ULL<<39;
        if( m_bitStep == 0 )   // Start ACK
        {
            m_bitStep = 1;
            m_inpin->scheduleState( false, 0 );
            Simulator::self()->addEvent( 80*1e6, this );
        }
        else{                  // End ACK
            m_bitStep = 0;
            m_outStep++;
            m_inpin->scheduleState( true, 0 );
            Simulator::self()->addEvent( 80*1e6, this );
    }   }
    else{                     // Send data
        if( m_bitStep == 0 )  // Start bit (low side)
        {
            m_bitStep = 1;
            m_inpin->scheduleState( false, 0 );
            Simulator::self()->addEvent( 50*1e6, this );
        }
        else{                 // End bit (high side)
            m_bitStep = 0;
            m_inpin->scheduleState( true, 0 );
            if( m_bit == 0 )             // Transmission finished
            {
                m_outStep = 0;
                m_inpin->setPinMode( input );
                m_inpin->changeCallBack( this, true );
                return;
            }
            uint64_t time = ( (m_data & m_bit) > 0 ) ? 70*1e6 : 27*1e6;
            Simulator::self()->addEvent( time, this );
            m_bit >>= 1;
}   }   }

void Dht22::calcData()
{
    uint64_t tempI, tempD, humiI, humiD;
    if( m_DHT22 )
    {
        uint64_t temp = std::fabs( m_temp*10 );
        if( m_temp < 0 ) temp |= 1<<15;
        tempI = temp >> 8;
        tempD = temp & 0xFF;

        uint64_t humi = m_humi*10;
        humiI = humi >> 8;
        humiD = humi & 0xFF;
    }else{
        tempI = m_temp;
        tempD = 0;//(m_temp-tempI)*10;
        humiI = m_humi;
        humiD = 0;//(m_humi-humiI)*10;
    }
    uint8_t checksum = tempI+tempD+humiI+humiD;
    m_data = (humiI<<32)+(humiD<<24)+(tempI<<16)+(tempD<<8)+checksum;
    //update();
}

//void Dht22::onbuttonclicked()
//{
//    m_set = m_button->isChecked();
//    if( m_set ) m_button->setIcon( QIcon(":/up.png") );
//    else        m_button->setIcon( QIcon(":/down.png") );
//}
//
//void Dht22::upbuttonclicked()
//{
//    if( m_set ) m_temp += m_tempInc;
//    else        m_humi += m_humiInc;
//
//    if( m_DHT22) {
//       if( m_temp > 80 )  m_temp = 80;
//       if( m_humi > 100 ) m_humi = 100;
//    }else{
//        if( m_temp > 50 ) m_temp = 50;
//        if( m_humi > 90 ) m_humi = 90;
//    }
//    calcData();
//}
//
//void Dht22::downbuttonclicked()
//{
//    if( m_set ) m_temp -= m_tempInc;
//    else        m_humi -= m_humiInc;
//
//    if( m_DHT22) {
//        if( m_temp < -40 ) m_temp = -40;
//        if( m_humi < 0  )  m_humi = 0;
//    }else{
//        if( m_temp < 0 )  m_temp = 0;
//        if( m_humi < 20 ) m_humi = 20;
//    }
//    calcData();
//}

void Dht22::setModel( std::string model )
{
    m_DHT22 = (model == "DHT22");
    if( m_DHT22 ) m_start = 1e9;
    else          m_start = 18e9;

    calcData();
}

std::string Dht22::model() { return m_DHT22 ? "DHT22" : "DHT11"; }

double Dht22::trim( double data ) { return (double)(((int)(data*10)))/10; }
