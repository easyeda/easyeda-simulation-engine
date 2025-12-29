/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "iocomponent.h"
//#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "iopin.h"

#include "doubleProp.h"
#include "boolProp.h"
#include "stringProp.h"


#define tr(str) simulideTr("IoComponent",str)

IoComponent::IoComponent( std::string type, std::string id)
           : Component( type, id )
           , LogicFamily()
{
    m_outValue = 0;

    m_inpPullups = 0;
    m_outPullups = 0;

    m_openCol    = false;
    m_invInputs  = false;
    m_invOutputs = false;

    m_openImp = 100/eElement::cero_doub;
}
IoComponent::~IoComponent(){}

std::vector<comProperty*> IoComponent::inputProps()
{
    return {
        new stringProp<IoComponent>("Family", "Logic Family", getFamilyKeys(m_families)
                                , this, &IoComponent::family, &IoComponent::setFamily, 0,"enum" ),

        new doubleProp<IoComponent>("SupplyV", "Supply Voltage", "V"
                                 , this, &IoComponent::supplyV, &IoComponent::setSupplyV ),

        new comProperty( "", "Inputs:","","",0),

        new doubleProp<IoComponent>("Input_High_V", "Low to High Threshold", "V"
                                 , this, &IoComponent::inpHighV, &IoComponent::setInpHighV ),

        new doubleProp<IoComponent>("Input_Low_V", "High to Low Threshold", "V"
                                 , this, &IoComponent::inpLowV, &IoComponent::setInpLowV ),

        new doubleProp<IoComponent>("Input_Imped", "Input Impedance", "MΩ"
                                 , this, &IoComponent::inputImp, &IoComponent::setInputImp ),

        new doubleProp<IoComponent>("InPullups", "Input pullups", "Ω"
                                  , this, &IoComponent::inpPullups, &IoComponent::setInpPullups, propNoCopy )
    };
}

std::vector<comProperty*> IoComponent::outputProps()
{
    return {
        new comProperty("", "Outputs:","","",0),

        new doubleProp<IoComponent>("Out_High_V", "Output High Voltage", "V"
                                 , this, &IoComponent::outHighV, &IoComponent::setOutHighV ),

        new doubleProp<IoComponent>("Out_Low_V", "Output Low Voltage", "V"
                                 , this, &IoComponent::outLowV, &IoComponent::setOutLowV ),

        new doubleProp<IoComponent>("Out_Imped", "Output Impedance", "Ω"
                                 , this, &IoComponent::outImp, &IoComponent::setOutImp ),

        new doubleProp<IoComponent>("Oen_Imped", "Output Z Impedance", "Ω"
                                  , this, &IoComponent::openImp, &IoComponent::setOpenImp )
    };
}

std::vector<comProperty*> IoComponent::outputType()
{
    return {
        new doubleProp<IoComponent>("Pullups", "Ouput pullups", "Ω"
                                  , this, &IoComponent::outPullups, &IoComponent::setOutPullups, propNoCopy ),
        new boolProp<IoComponent>( "Inverted"      , "Invert Outputs",""
                                  , this, &IoComponent::invertOuts, &IoComponent::setInvertOuts, propNoCopy ),
        new boolProp<IoComponent>( "Open_Collector", "Open Drain"    ,""
                                  , this, &IoComponent::openCol,    &IoComponent::setOpenCol   , propNoCopy )
    };
}

std::vector<comProperty*> IoComponent::edgeProps()
{
    return {
        new doubleProp<IoComponent>( "pd_n"  , "Propagation delay","_Gates"
                                  , this, &IoComponent::propSize, &IoComponent::setPropSize ),
        new doubleProp<IoComponent>( "Tpd_ps", "Gate Delay","ps"
                                  , this, &IoComponent::propDelay, &IoComponent::setPropDelay ),
        new doubleProp<IoComponent>( "Tr_ps" , "Rise Time" ,"ps"
                                  , this, &IoComponent::riseTime,  &IoComponent::setRiseTime ),
        new doubleProp<IoComponent>( "Tf_ps" , "Fall Time" ,"ps"
                                  , this, &IoComponent::fallTime,  &IoComponent::setFallTime ) ,
        new stringProp <IoComponent>("invertPins","",""
                               , this, &IoComponent::invertPins, &IoComponent::setInvertPins, propHidden )
    };

}

void IoComponent::initState()
{
    for( uint32_t i=0; i<m_outPin.size(); ++i )
    {
        m_outPin[i]->setStateZ( false );
        m_outPin[i]->setOutState( false );
    }
    m_nextOutVal = m_outValue = 0;
    while( !m_outQueue.empty()  ) m_outQueue.pop();
    while( !m_timeQueue.empty() ) m_timeQueue.pop();
}

void IoComponent::runOutputs()
{
    m_outValue = m_outQueue.front();
    m_outQueue.pop();

    if( !m_timeQueue.empty() )
    {uint64_t nextTime = m_timeQueue.front()-Simulator::self()->circTime();
        m_timeQueue.pop();
        Simulator::self()->addEvent( nextTime, m_eElement );
    }

    for( uint32_t i=0; i<m_outPin.size(); ++i )
    {
        bool state = m_outValue & (1<<i);
        m_outPin[i]->scheduleState( state, 0 );
    }
}

void IoComponent::scheduleOutPuts( eElement* el )
{
    // uint64_t delay = m_delayBase*m_delayMult;

    // if( !delay )
    // {
        if( m_nextOutVal == m_outValue ) return;
        m_outValue = m_nextOutVal;
        for( uint32_t i=0; i<m_outPin.size(); ++i )
            m_outPin[i]->scheduleState( m_outValue & (1<<i), 0 );
    //     return;
    // }
    // if(  m_outQueue.empty() )
    // {
    //     if( m_nextOutVal == m_outValue ) return;
    //     Simulator::self()->addEvent( delay, el );
    // }
    // else          // New Event while previous Event not dispatched
    // {
    //     if( m_nextOutVal == m_outQueue.back() ) return;
    //     uint64_t nextTime = Simulator::self()->circTime()+delay;
    //     m_timeQueue.push( nextTime );

    //     m_eElement = el;
    // }
    // m_outQueue.push( m_nextOutVal );
}

void IoComponent::setSupplyV( double v )
{
    if( v < 0 ) v = 0;
    m_supplyV = v;

    updateData();
}

void IoComponent::setInpHighV( double volt )
{
    if( volt > m_supplyV ) volt = m_supplyV;

    if( m_inpHighV == volt ) return;
    m_inpHighV = volt;

    for( IoPin* pin : m_inpPin )   pin->setInputHighV( volt );
    for( IoPin* pin : m_outPin )   pin->setInputHighV( volt );
    for( IoPin* pin : m_otherPin ) pin->setInputHighV( volt );
    LogicFamily::setInpHighV( volt );
}

void IoComponent::setInpLowV( double volt )
{
    if( volt < 0) volt = 0;
    if( m_inpLowV == volt ) return;
    m_inpLowV = volt;

    for( IoPin* pin : m_inpPin )   pin->setInputLowV( volt );
    for( IoPin* pin : m_outPin )   pin->setInputLowV( volt );
    for( IoPin* pin : m_otherPin ) pin->setInputLowV( volt );
    LogicFamily::setInpLowV( volt );
}

void IoComponent::setOutHighV( double volt )
{
    if( volt > m_supplyV ) volt = m_supplyV;

    if( m_outHighV == volt ) return;
    m_outHighV = volt;

    for( IoPin* pin : m_inpPin )   pin->setOutHighV( volt );
    for( IoPin* pin : m_outPin )   pin->setOutHighV( volt );
    for( IoPin* pin : m_otherPin ) pin->setOutHighV( volt );
    LogicFamily::setOutHighV( volt );
}

void IoComponent::setOutLowV( double volt )
{
    if( volt < 0) volt = 0;

    if( m_outLowV == volt ) return;
    m_outLowV = volt;

    for( IoPin* pin : m_inpPin )   pin->setOutLowV( volt );
    for( IoPin* pin : m_outPin )   pin->setOutLowV( volt );
    for( IoPin* pin : m_otherPin ) pin->setOutLowV( volt );
    LogicFamily::setOutLowV( volt );
}

void IoComponent::setInputImp( double imp )
{
    if( imp < 1e-14 ) imp = 1e-14;

    if( m_inpImp == imp ) return;
    m_inpImp = imp;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_inpPin )   pin->setInputImp( imp );
    for( IoPin* pin : m_outPin )   pin->setInputImp( imp );
    for( IoPin* pin : m_otherPin ) pin->setInputImp( imp );
    Simulator::self()->resumeSim();
}

void IoComponent::setOutImp( double imp )
{
    if( imp < 1e-14 ) imp = 1e-14;

    if( m_outImp == imp ) return;
    m_outImp = imp;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_inpPin )   pin->setOutputImp( imp );
    for( IoPin* pin : m_outPin )   pin->setOutputImp( imp );
    for( IoPin* pin : m_otherPin ) pin->setOutputImp( imp );
    Simulator::self()->resumeSim();
}

void IoComponent::setOpenImp( double imp )
{
    m_openImp = imp;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_inpPin )   pin->setOpenImp( imp );
    for( IoPin* pin : m_outPin )   pin->setOpenImp( imp );
    for( IoPin* pin : m_otherPin ) pin->setOpenImp( imp );
    Simulator::self()->resumeSim();
}

void IoComponent::setInvertOuts( bool invert )
{
    //if( m_inpvOutputs == invert ) return;
    m_invOutputs = invert;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_outPin ) pin->setInverted( invert );
    // Circuit::self()->update();
    Simulator::self()->resumeSim();
}

void IoComponent::setInvertInps( bool invert )
{
    //if( m_inpvInputs == invert ) return;
    m_invInputs = invert;

    Simulator::self()->pauseSim();
    for( IoPin* pin : m_inpPin ) pin->setInverted( invert );
    // Circuit::self()->update();
    Simulator::self()->resumeSim();
}

void IoComponent::setOpenCol( bool op )
{
    if( m_openCol == op ) return;
    m_openCol = op;
    Simulator::self()->pauseSim();
    for( IoPin* pin : m_outPin )
    {
        if( op ) pin->setPinMode( openCo );
        else     pin->setPinMode( output );
    }
    //Circuit::self()->update();
    Simulator::self()->resumeSim();
}

void IoComponent::setInpPullups( double p )
{
    m_inpPullups = p;
    for( uint32_t i=0; i<m_inpPin.size(); ++i ) m_inpPin[i]->setPullup( p );
}

void IoComponent::setOutPullups( double p )
{
    m_outPullups = p;
    for( uint32_t i=0; i<m_outPin.size(); ++i ) m_outPin[i]->setPullup( p );
}

void IoComponent::setRiseTime( double time )
{
    LogicFamily::setRiseTime( time );
    for( IoPin* pin : m_outPin ) pin->setRiseTime( m_timeLH*1.25 );
}

void IoComponent::setFallTime( double time )
{
    LogicFamily::setFallTime( time );
    for( IoPin* pin : m_outPin ) pin->setFallTime( m_timeHL*1.25 );
}

void IoComponent::init( std::vector<std::string> pins ) // Example: pin = "IL02Name" => input, left, number 2, label = "Name"
{
    //m_area = QRect( -(m_width/2)*8, -(m_height/2)*8, m_width*8, m_height*8 );

    std::vector<std::string> inputs;
    std::vector<std::string> outputs;

    for (const std::string& pin : pins) 
    {
        if (pin.compare(0, 1, "I") == 0) 
        {
            inputs.push_back(pin);
        }
        else if (pin.compare(0, 1, "O") == 0) 
        {
            outputs.push_back(pin);
        }
        else 
        {
            std::cout << " LogicComponent::init: pin name error " << pin << std::endl;
        }
    }
    int i = m_inpPin.size();
    m_inpPin.resize( i+inputs.size() );
    for( std::string inp : inputs ) // Example input = "L02Name"
    {
        m_inpPin[i] = createPin( inp, m_id+"-in"+std::to_string(i) );
        i++;
    }
    i = m_outPin.size();
    m_outPin.resize( i+outputs.size() );
    for( std::string out : outputs ) // Example output = "L02Name"
    {
        m_outPin[i] = createPin( out, m_id+"-out"+std::to_string(i) );
        i++;
}   }

IoPin* IoComponent::createPin( std::string data, std::string id ) // Example data = "L02" => left side, number 2
{
    pinMode_t mode = (data[0] == 'I') ? input : output;
    data.erase(0, 1);


    IoPin* pin = new IoPin( id, 0, this, mode );
    setupPin( pin, data );
    return pin;
}


void IoComponent::setupPin( IoPin *pin, std::string data ) // Example data = "L02" => left side, position 2
{
    std::string pos = data.substr(0, 1);
    data.erase(0, 1);
    int num = std::stoi(data.substr(0, 2));
    data.erase(0, 2);
    std::string label = data;
    initPin( pin );
}

void IoComponent::initPin( IoPin* pin )
{
    pin->setInputHighV( m_inpHighV );
    pin->setInputLowV( m_inpLowV );
    pin->setInputImp( m_inpImp );
    pin->setOutHighV( m_outHighV );
    pin->setOutLowV( m_outLowV );
    pin->setOutputImp( m_outImp  );
}

void IoComponent::setNumInps( uint32_t pins, std::string label, int bit0, int id0  )
{
    setNumPins( &m_inpPin, pins, label, bit0, false, id0 );
}

void IoComponent::setNumOuts( uint32_t pins, std::string label, int bit0, int id0 )
{
    setNumPins( &m_outPin, pins, label, bit0, true, id0 );
}


void IoComponent::setNumPins( std::vector<IoPin*>* pinList, uint32_t pins
                              , std::string label, int bit0, bool out,  int id0 )
{
    uint32_t oldSize = pinList->size();
    //if( pins == oldSize ) return;
    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    //int halfW = (m_width/2)*8;//m_width*8/2;
    //int x           = out ? halfW+8 : -(halfW)-8;
    //int angle       = out ?  0  : 180;
    std::string id      = out ? "-out" : "-in";
    pinMode_t mode  = out ? output : input;

    id = m_id+id;

    if( pins < oldSize ) deletePins( pinList, oldSize-pins );
    else                 pinList->resize( pins );

    if( m_outPin.size() > m_inpPin.size() ) m_height = m_outPin.size();
    else                                   m_height = m_inpPin.size();

    int halfH;
    if( label.empty() ) halfH = m_height*8/2; // Gates
    else
    {
        m_height += 1;
        halfH = (m_height/2)*8;
    }
    //m_area = QRect(-halfW,-halfH, m_width*8, m_height*8 );

    int start = 8;
    if( label.empty() ) start = 4;  // Gates
    // else if( start%8 ) start +=4;

    for( uint32_t i=0; i<pins; ++i )
    {
        //int y = m_area.y() + i*8 + start;

        std::string num = "";
        std::string pinId = id;
        if( bit0 >= 0  )
        {
            pinId += std::to_string(id0);
            num = std::to_string(bit0);
        }
        pinList->at(i) = new IoPin(pinId, i, this, mode );
        initPin( pinList->at(i) );
        if( mode == output && m_invOutputs ) pinList->at(i)->setInverted( true );
        if( mode == input && m_invInputs ) pinList->at(i)->setInverted( true );

        //if( !label.isEmpty() ) pinList->at(i)->setLabelText( label+num );
        //pinList->at(i)->setLabelColor( QColor( 0, 0, 0 ) );
        if( bit0 >= 0 )
        {
            id0++;
            bit0++;
        }
    }
    //setflip();
    //Circuit::self()->update();
    setInpPullups( m_inpPullups );
    setOutPullups( m_outPullups );

}

void IoComponent::deletePins( std::vector<IoPin*>* pinList, uint32_t pins )
{
    uint32_t oldSize = pinList->size();
    if( pins > oldSize ) pins = oldSize;

    uint32_t newSize = oldSize-pins;
    for( uint32_t i=oldSize-1; i>newSize-1; --i ) deletePin( pinList->at(i) );

    pinList->resize( newSize );
}

std::vector<Pin*> IoComponent::getPins()
{
    std::vector<Pin*> pins;
    pins.reserve( m_inpPin.size()+m_outPin.size()+m_otherPin.size() );
    for( Pin* pin : m_inpPin    ) pins.emplace_back( pin );
    for( Pin* pin : m_outPin   ) pins.emplace_back( pin );
    for( Pin* pin : m_otherPin ) pins.emplace_back( pin );
    return pins;
}

void IoComponent::remove()
{
    for( IoPin* pin : m_inpPin )    pin->removeConnector();
    for( IoPin* pin : m_outPin )   pin->removeConnector();
    for( IoPin* pin : m_otherPin ) pin->removeConnector();
    Component::remove();
}
std::string IoComponent::getFamilyKeys(const std::unordered_map<std::string, logicFamily_t> &map) {
    return std::accumulate(
        map.begin(), map.end(), std::string{},
        [](std::string acc, const auto &pair) {
            return acc.empty() ? pair.first : std::move(acc) + "," + pair.first;
        }
    );
}

