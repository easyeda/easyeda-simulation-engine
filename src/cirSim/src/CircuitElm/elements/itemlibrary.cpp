/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */
 
#include "itemlibrary.h"
#include "circuit.h"

//BEGIN Item includes
#include "ammeter.h"
#include "sim_adc.h"
//#include "aip31068_i2c.h"
#include "audio_out.h"
#include "battery.h"
#include "bcdto7s.h"
#include "bcdtodec.h"
#include "bincounter.h"
#include "BJT.h"
#include "buffer.h"
#include "bus.h"
#include "capacitor.h"
#include "clock.h"
#include "csource.h"
#include "currsource.h"
#include "dac.h"
#include "dcmotor.h"
#include "dectobcd.h"
#include "demux.h"
#include "dht22.h"
#include "diac.h"
//#include "dial.h"
#include "diode.h"
//#include "ds1307.h"
//#include "ds1621.h"
#include "ds18b20.h"
//#include "dynamic_memory.h"
#include "elcapacitor.h"
//#include "ellipse.h"
//#include "esp01.h"
#include "fixedvolt.h"
#include "flipflopd.h"
#include "flipflopjk.h"
#include "flipfloprs.h"
#include "flipflopt.h"
//#include "freqmeter.h"
#include "fulladder.h"
//#include "function.h"
#include "gate_and.h"
#include "gate_or.h"
#include "gate_xor.h"
#include "ground.h"
//#include "header.h"
//#include "hd44780.h"
//#include "ili9341.h"
//#include "image.h"
//#include "i2cram.h"
//#include "i2ctoparallel.h"
#include "inductor.h"
//#include "keypad.h"
//#include "ky023.h"
//#include "ky040.h"
//#include "ks0108.h"
#include "lamp.h"
#include "latchd.h"
#include "ldr.h"
#include "led.h"
#include "ledbar.h"
#include "ledmatrix.h"
#include "ledrgb.h"
//#include "line.h"
#include "lm555.h"
//#include "logicanalizer.h"
//#include "max72xx_matrix.h"
//#include "mcu.h"
#include "memory.h"
#include "mosfet.h"
#include "mux.h"
#include "mux_analog.h"
#include "op_amp.h"
//#include "oscope.h"
//#include "pcd8544.h"
#include "probe.h"
#include "potentiometer.h"
#include "push.h"
#include "rail.h"
//#include "rectangle.h"
//#include "relay.h"
#include "resistor.h"
#include "resistordip.h"
#include "rtd.h"
//#include "serialport.h"
//#include "serialterm.h"
#include "servo.h"
#include "sevensegment.h"
#include "sevensegment_bcd.h"
#include "shiftreg.h"
#include "socket.h"
#include "sr04.h"
#include "scr.h"
//#include "ssd1306.h"
#include "stepper.h"
#include "strain.h"
#include "subcircuit.h"
#include "subpackage.h"
#include "switch.h"
//#include "switchdip.h"
#include "thermistor.h"
//#include "textcomponent.h"
//#include "touchpad.h"
#include "transformer.h"
#include "triac.h"
#include "tunnel.h"
#include "varresistor.h"
//#include "voltmeter.h"
#include "volt_reg.h"
#include "voltsource.h"
#include "wavegen.h"
#include "wire.h"
//#include "ws2812.h"
#include "zener.h"

//END Item includes

ItemLibrary* ItemLibrary::m_pSelf = NULL;

ItemLibrary::ItemLibrary()
{
    m_pSelf = this;
    loadItems();
}
ItemLibrary::~ItemLibrary()
{
    for( LibraryItem* item : m_items ) delete item;
}


// 这里每个类别开头的addItem函数中的_createItem，全是NULL的原因：创建函数为空的 LibraryItem 对象用于在元件库中表示分类标题，它用来给ui界面显示我们包装了哪些元器件
// 而后面跟着的在不同类中的libraryItem函数，则是用来创建具体的元器件对象，并将其添加到元件库中，他是带有创建函数的
void ItemLibrary::loadItems()
{
    m_items.clear();
    // Meters
    //addItem( new LibraryItem((std::string) "Meters", (std::string)"", (std::string)"", (std::string)"Meters", nullptr);
    addItem( Probe::libraryItem() );
    //addItem( Voltimeter::libraryItem() );
    addItem( Ammeter::libraryItem() );
    //addItem( FreqMeter::libraryItem() );
    //addItem( Oscope::libraryItem() );
    //addItem( LAnalizer::libraryItem() );
    // Sources
    ////addItem( new LibraryItem( ("Sources"), "", "","Sources", NULL ) );
    addItem( FixedVolt::libraryItem() );
    addItem( Clock::libraryItem() );
    addItem( WaveGen::libraryItem() );
    addItem( VoltSource::libraryItem() );
    addItem( CurrSource::libraryItem() );
    addItem( Csource::libraryItem() );
    addItem( Battery::libraryItem() );
    addItem( Rail::libraryItem() );
    addItem( Ground::libraryItem() );
    // Switches
    ////addItem( new LibraryItem( ("Switches"), "", "","Switches", NULL ) );
    addItem( Push::libraryItem() );
    addItem( Switch::libraryItem() );
    //addItem( SwitchDip::libraryItem() );
    //addItem( Relay::libraryItem() );
    //addItem( KeyPad::libraryItem() );
    // Passive
    //addItem( new LibraryItem( ("Passive"), "", "","Passive", NULL ) );
    //addItem( new LibraryItem( ("Resistors"), "Passive", "resistors.png","Resistors", NULL ) );
    addItem( Resistor::libraryItem() );
    addItem( ResistorDip::libraryItem() );
    addItem( Potentiometer::libraryItem() );
    addItem( VarResistor::libraryItem() );
    //addItem( new LibraryItem( ("Resistive Sensors"), "Passive", "resistorsensors.png","Resistive Sensors", NULL ) );
    addItem( Ldr::libraryItem() );
    addItem( Thermistor::libraryItem() );
    addItem( RTD::libraryItem() );
    addItem( Strain::libraryItem() );
    //addItem( new LibraryItem( ("Reactive"), "Passive", "reactive.png","Reactive", NULL ) );
    addItem( Capacitor::libraryItem() );
    addItem( elCapacitor::libraryItem() );
    addItem( Inductor::libraryItem() );
    addItem( Transformer::libraryItem() );
    // Active
    //addItem( new LibraryItem( ("Active"), "", "","Active", NULL ) );
    //addItem( new LibraryItem( ("Rectifiers"), "Active", "rectifiers.png","Rectifiers", NULL ) );
    addItem( Diode::libraryItem() );
    addItem( Zener::libraryItem() );
    addItem( SCR::libraryItem() );
    addItem( Diac::libraryItem() );
    addItem( Triac::libraryItem() );
    //addItem( new LibraryItem( ("Transistors"), "Active", "transistors.png","Transistors", NULL ) );
    addItem( Mosfet::libraryItem() );
    addItem( BJT::libraryItem() );
    //addItem( new LibraryItem( ("Other Active"), "Active", "2to2.png","Other Active", NULL ) );
    addItem( OpAmp::libraryItem() );
    addItem( VoltReg::libraryItem() );
    addItem( MuxAnalog::libraryItem() );
    // Outputs
    //addItem( new LibraryItem( ("Outputs"), "", "","Outputs", NULL ) );
    //addItem( new LibraryItem( ("Leds"), "Outputs", "leds.png","Leds", NULL ) );
    addItem( Led::libraryItem() );
    addItem( LedRgb::libraryItem() );
    addItem( LedBar::libraryItem() );
    addItem( SevenSegment::libraryItem() );
    addItem( LedMatrix::libraryItem() );
    //addItem( Max72xx_matrix::libraryItem() );
    //addItem( WS2812::libraryItem() );
    //addItem( new LibraryItem( ("Displays"), "Outputs", "displays.png","Displays", NULL ) );
    //addItem( Hd44780::libraryItem() );
    //addItem( Aip31068_i2c::libraryItem() );
    //addItem( Pcd8544::libraryItem() );
    //addItem( Ks0108::libraryItem() );
    //addItem( Ssd1306::libraryItem() );
    //addItem( Ili9341::libraryItem() );
    //addItem( new LibraryItem( ("Motors"), "Outputs", "motors.png","Motors", NULL ) );
    addItem( DcMotor::libraryItem() );
    addItem( Stepper::libraryItem() );
    addItem( Servo::libraryItem() );
    //addItem( new LibraryItem( ("Other Outputs"), "Outputs", "1to1.png","Other Outputs", NULL ) );
    addItem( AudioOut::libraryItem() );
    addItem( Lamp::libraryItem() );
    // Micro
    //addItem( new LibraryItem( ("Micro"), "", "","Micro", NULL ) );
    //addItem( new LibraryItem( "AVR" , "Micro", "ic2.png","AVR", NULL ) );
    //addItem( new LibraryItem( "PIC" , "Micro", "ic2.png","PIC", NULL ) );
    //addItem( new LibraryItem( "I51" , "Micro", "ic2.png","I51", NULL ) );
    //addItem( new LibraryItem("MCS65", "Micro", "ic2.png","MCS65", NULL ) );
    //addItem( new LibraryItem("Z80"  , "Micro", "ic2.png","Z80", NULL ) );
    //addItem( Mcu::libraryItem() );
    //addItem( new LibraryItem( ("Arduino"), "Micro", "board.png","Arduino", NULL ) );
    //addItem( new LibraryItem( ("Shields"), "Micro", "shield.png","Shields", NULL ) );
    //addItem( new LibraryItem( ("Sensors"), "Micro", "1to2.png","Sensors", NULL ) );
    addItem( SR04::libraryItem() );
    addItem( Dht22::libraryItem() );
    //addItem( DS1621::libraryItem() );
    addItem( Ds18b20::libraryItem() );
    //addItem( new LibraryItem( ("Peripherals"), "Micro", "perif.png","Peripherals", NULL ) );
    //addItem( SerialPort::libraryItem() );
    //addItem( SerialTerm::libraryItem() );
    //addItem( TouchPad::libraryItem() );
    //addItem( KY023::libraryItem() );
    //addItem( KY040::libraryItem() );
    //addItem( DS1307::libraryItem() );
    //addItem( Esp01::libraryItem() );
    // Logic
    //addItem( new LibraryItem( ("Logic"), "", "","Logic", NULL ) );
    //addItem( new LibraryItem( ("Gates"), "Logic", "gates.png","Gates", NULL ) );
    addItem( Buffer::libraryItem() );
    addItem( AndGate::libraryItem() );
    addItem( OrGate::libraryItem() );
    addItem( XorGate::libraryItem() );
    //addItem( new LibraryItem( ("Arithmetic"), "Logic", "2to2.png","Arithmetic", NULL ) );
    addItem( BinCounter::libraryItem() );
    addItem( FullAdder::libraryItem() );
    addItem( ShiftReg::libraryItem() );
    //addItem( Function::libraryItem() );
    //addItem( new LibraryItem( ("Memory"), "Logic", "subc.png","Memory", NULL ) );
    addItem( FlipFlopD::libraryItem() );
    addItem( FlipFlopT::libraryItem() );
    addItem( FlipFlopRS::libraryItem() );
    addItem( FlipFlopJK::libraryItem() );
    addItem( LatchD::libraryItem() );
    //addItem( Memory::libraryItem() );
    //addItem( DynamicMemory::libraryItem() );
    //addItem( I2CRam::libraryItem() );
    //addItem( new LibraryItem( ("Converters"), "Logic", "1to2.png","Converters", NULL ) );
    addItem( Mux::libraryItem() );
    addItem( Demux::libraryItem() );
    addItem( BcdToDec::libraryItem() );
    addItem( DecToBcd::libraryItem() );
    addItem( BcdTo7S::libraryItem() );
    //addItem( I2CToParallel::libraryItem() );
    //addItem( new LibraryItem( ("Other Logic"), "Logic", "2to3.png","Other Logic", NULL ) );
    addItem( ADC::libraryItem() );
    addItem( DAC::libraryItem() );
    addItem( SevenSegmentBCD::libraryItem() );
    addItem( Lm555::libraryItem() );
    // Subcircuits
    //addItem( new LibraryItem( ("Ternary")    , "Logic", "subc.png","Ternary", NULL ) );
    //addItem( new LibraryItem( ("Digital Potentiometer"), "Logic", "subc.png","Digital Potentiometer", NULL ) );
    //addItem( new LibraryItem( ("IC 74")      , "Logic", "ic2.png","IC 74", NULL ) );
    //addItem( new LibraryItem( ("IC CD")      , "Logic", "ic2.png","IC CD", NULL ) );
    //addItem( new LibraryItem( ("USSR IC")    , "Logic", "ic2.png","USSR IC", NULL ) );
    //addItem( new LibraryItem( ("Other IC")   , "Logic", "ic2.png","Other IC", NULL ) );
    //addItem( new LibraryItem( ("Keys")       , "Logic", "ic2.png","Keys", NULL ) );
    //addItem( new LibraryItem( ("Led display"), "Logic", "7segbcd.png","Led display", NULL ) );
    //addItem( new LibraryItem( ("Tools")      , "Logic", "subc.png","Tools", NULL ) );
    addItem( SubCircuit::libraryItem() );
    // Connectors
    //addItem( new LibraryItem( ("Connectors"), "", "","Connectors", NULL ) );
    addItem( Bus::libraryItem() );
    addItem( Tunnel::libraryItem() );
    addItem( Socket::libraryItem() );
    //addItem( Header::libraryItem() );
    // Graphical
    //addItem( new LibraryItem( ("Graphical"), "", "","Graphical", NULL ) );
    //addItem( Image::libraryItem() );
    //addItem( TextComponent::libraryItem() );
    //addItem( Rectangle::libraryItem() );
    //addItem( Ellipse::libraryItem() );
    //addItem( Line::libraryItem() );
    // Other
    //addItem( new LibraryItem( ("Other"), "", "","Other", NULL ) );
    addItem( SubPackage::libraryItem() );
    //addItem( Dial::libraryItem() );
    addItem( Wire::libraryItem() );
}

void ItemLibrary::addItem( LibraryItem* item )
{
    if (!item) return;
    m_items.push_back(item);
}

LibraryItem* ItemLibrary::itemByName( const std::string name )
{
    for( LibraryItem* item : m_items )
        if( item->name() == name ) return item;

    return NULL;
}

// CLASS LIBRARYITEM *********************************************************
//连接器，用于连接UI和元件实例
LibraryItem::LibraryItem( std::string name, std::string category, std::string iconName,
                          std::string type, createItemPtr _createItem )
{
    m_name      = name;
    m_category  = category;
    m_iconfile  = iconName;
    m_type      = type;
    createItem  = _createItem;
}
LibraryItem::~LibraryItem() { }

