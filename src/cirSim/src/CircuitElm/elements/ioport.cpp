/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "ioport.h"
#include "simulator.h"

IoPort::IoPort( std::string name )
      : eElement( name )
{
    m_name = name;
    if (!name.empty())
    {
        m_shortName = "P" + name.substr(name.size() - 1);
    }
    else
    {
        std::cerr << "空字符！" << std::endl;
    }
    m_numPins = 0;
}
IoPort::~IoPort(){}

void IoPort::reset()
{
    m_index = 0;
    m_pinState  = 0;
    m_nextState = 0;
    m_pinDirection = 0;
    m_pinMode  = input;

    for( IoPin* pin : m_pins ) {
        pin->setOutState( 0 );
        pin->setPinMode( input );
    }
}

void IoPort::runEvent()
{
    setOutState( m_nextState );
    /*for( int i=0; i<m_numPins; ++i ){
        IoPin* pin = m_pins[i];
        bool nextState = (m_nextState & 1<<i)>0;
        //if( pin->getOutState() != nextState )     // Pin changed
            pin->setOutState( nextState );
    }*/
    if( m_index > 0 ) 
        nextStep();
}

void IoPort::trigger()
{
    if( m_index == 0 ) nextStep();
}

void IoPort::nextStep()
{
    outState_t outState = m_outVector.at( m_index );
    m_index++;
    if( m_index >= m_outVector.size() ) m_index = 0;

    m_nextState = outState.state;
    uint64_t time = outState.time;
    if( time ) Simulator::self()->addEvent( time, this );
    else       runEvent();
}

void IoPort::scheduleState( uint32_t val, uint64_t time )
{
    if( m_pinState == val ) return;
    m_nextState = val;
    Simulator::self()->addEvent( time, this );
}

void IoPort::setOutState( uint32_t val )
{
    uint32_t changed = m_pinState ^ val; // See which Pins have actually changed
    if( changed == 0 ) return;
    m_pinState = val;

    for( int i=0; i<m_numPins; ++i ){
        uint32_t flag = 1<<i;
        if( changed & flag ) m_pins[i]->setOutState( (val & flag) > 0 ); // Pin changed
    }
}

void IoPort::setOutStatFast( uint32_t val )
{
    uint32_t changed = m_pinState ^ val; // See which Pins have actually changed
    if( changed == 0 ) return;
    m_pinState = val;

    for( int i=0; i<m_numPins; ++i ){
        uint32_t flag = 1<<i;
        if( changed & flag ) m_pins[i]->setOutStatFast( (val & flag) > 0 ); // Pin changed
    }
}

uint32_t IoPort::getInpState()
{
    uint32_t data = 0;
    for( int i=0; i<m_numPins; ++i ) if( m_pins[i]->getInpState() ) data += (1 << i);
    return data;
}

void IoPort::setDirection( uint32_t val )
{
    uint32_t changed = m_pinDirection ^ val;  // See which Pins have actually changed
    if( changed == 0 ) return;
    m_pinDirection = val;

    for( int i=0; i<m_numPins; ++i ){
        uint32_t flag = 1<<i;
        if( changed & flag ) m_pins[i]->setPinMode( ((val & flag) > 0) ? output : input ); // Pin changed
}   }

void IoPort::setPinMode( pinMode_t mode )
{
    if( m_pinMode == mode ) return;
    m_pinMode = mode;
    for( IoPin* pin : m_pins ) pin->setPinMode( mode );
}

void IoPort::changeCallBack( eElement* el, bool ch )
{
    for( IoPin* pin : m_pins ) pin->changeCallBack( el, ch );
}

void IoPort::createPins( Component* comp, std::string pins, uint32_t pinMask )
{
    m_numPins = std::stoi(pins); // 假设 pins 是一个表示引脚数量的字符串
    if( m_numPins )
    {
        m_pins.resize( m_numPins );

        for( int i=0; i<m_numPins; ++i )
        {
            if( pinMask & 1<<i )
                m_pins[i] = createPin( i, m_name+ std::to_string(i), comp );//new IoPin( this, i, m_name+std::string::number(i), IoComp );
        }
    }
    else
    {
        std::istringstream iss(pins);
        std::string pinName;
        while (std::getline(iss, pinName, ','))
        {
            IoPin* pin = createPin( m_numPins, m_name+pinName , comp );//new IoPin( this, i, m_name+pinName, IoComp );
            m_pins.emplace_back( pin );
            m_numPins++;
        }
    }
}

IoPin* IoPort::createPin( int i, std::string id, Component* comp )
{
    IoPin* pin = new IoPin( comp->getUid()+"-"+id, i, comp, input );
    pin->setOutHighV( 5 );
    //pin->setPinState( input_low );
    return pin;
}

IoPin* IoPort::getPinN( uint8_t i )
{
    if( i >= m_pins.size() ) return NULL;
    return m_pins[i];
}

IoPin* IoPort::getPin( std::string pinName )
{
    IoPin* pin = nullptr;
    if (pinName.find(m_name) == 0 || pinName.find(m_shortName) == 0)
    {
        std::string pinId = pinName;
        // 移除 m_name 和 m_shortName
        if (pinId.find(m_name) == 0)
            pinId.erase(0, m_name.length());
        else if (pinId.find(m_shortName) == 0)
            pinId.erase(0, m_shortName.length());

        // 将剩余的字符串转换为整数
        int pinNumber = std::stoi(pinId);
        pin = getPinN(pinNumber);
    }
    else
    {
        for (IoPin* ioPin : m_pins)
        {
            std::string pid = ioPin->pinId();
            // 分割 pid 并获取最后一个部分
            std::istringstream iss(pid);
            std::string lastPart;
            while (std::getline(iss, lastPart, '-')) {}
            // 移除 m_name
            size_t pos = lastPart.find(m_name);
            if (pos != std::string::npos)
                lastPart.erase(pos, m_name.length());
            if (lastPart == pinName)
                return ioPin;
        }
    }
    //if (!pin)
    //    std::cerr << "ERROR: IoPort::getPin NULL Pin:" << pinName << std::endl;
    return pin;
}

// ---- Script Engine -------------------
//#include "angelscript.h"
//void IoPort::registerScript( asIScriptEngine* engine )
//{
//    engine->RegisterObjectType("IoPort", 0, asOBJ_REF | asOBJ_NOCOUNT );
//
//    engine->RegisterObjectMethod("IoPort", "void setPinMode(uint m)"
//                                   , asMETHODPR( IoPort, setPinMode, (uint), void)
//                                   , asCALL_THISCALL );
//
//    engine->RegisterObjectMethod("IoPort", "uint getInpState()"
//                                   , asMETHODPR( IoPort, getInpState, (), uint)
//                                   , asCALL_THISCALL );
//
//    engine->RegisterObjectMethod("IoPort", "void scheduleState( uint32 state, uint64 time )"
//                                   , asMETHODPR( IoPort, scheduleState, (uint32_t,uint64_t), void)
//                                   , asCALL_THISCALL );
//
//    engine->RegisterObjectMethod("IoPort", "void setOutState(uint s)"
//                                   , asMETHODPR( IoPort, setOutState, (uint), void)
//                                   , asCALL_THISCALL );
//
//    engine->RegisterObjectMethod("IoPort", "void addOutState( uint64 t, uint s )"
//                                   , asMETHODPR( IoPort, addOutState, (uint64_t t, uint s), void)
//                                   , asCALL_THISCALL );
//
//    engine->RegisterObjectMethod("IoPort", "void changeCallBack(eElement@ s, bool s)"
//                                   , asMETHODPR( IoPort, changeCallBack, (eElement*, bool), void)
//                                   , asCALL_THISCALL );
//
//    engine->RegisterObjectMethod("IoPort", "void trigger()"
//                                   , asMETHODPR( IoPort, trigger, (), void)
//                                   , asCALL_THISCALL );
//}
