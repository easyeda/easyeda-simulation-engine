/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "wavegen.h"
#include "iopin.h"
#include "simulator.h"
#include "circuit.h"
#include "itemlibrary.h"

#include "doubleProp.h"
#include "stringProp.h"
#include "boolProp.h"
#include "intProp.h"

#define tr(str) simulideTr("WaveGen",str)

Component* WaveGen::construct( std::string type, std::string id )
{ return new WaveGen( type, id ); }

LibraryItem* WaveGen::libraryItem()
{
    return new LibraryItem(
        "Wave Gen.",
        "Sources",
        "wavegen.png",
        "WaveGen",
        WaveGen::construct );
}

WaveGen::WaveGen( std::string type, std::string id )
       : ClockBase( type, id )
{
    m_bipolar  = false;
    m_floating = false;
    m_voltBase = 0;
    m_phaseShift = 0;
    m_lastVout = 0;
    m_minSteps = 100;
    m_waveType = Sine;
    //m_wavePixmap = NULL;

    m_pin.resize(2);
    m_pin[1] = m_gndPin = new IoPin(id+"-gndnod", 0, this, source );
    //m_gndPin->setVisible( false );
    
    setDuty( 50 );

    Simulator::self()->addToUpdateList( this );

    remPropGroup( "Main" );
    addPropGroup( { "Main", {
new stringProp <WaveGen>("Wave_Type", "Wave Type"  ,""      , this, &WaveGen::waveType,  &WaveGen::setWaveType,0,"enum" ),
new doubleProp<WaveGen>("Freq"      , "Frequency"  ,"Hz"    , this, &WaveGen::freq,      &WaveGen::setFreq ),
new doubleProp<WaveGen>("Phase"     , "Phase shift","_º"    , this, &WaveGen::phaseShift,&WaveGen::setPhaseShift ),
new intProp <WaveGen>("Steps", "Minimum Steps", "", this, &WaveGen::minSteps, &WaveGen::setMinSteps ),
new doubleProp<WaveGen>("Duty"      , "Duty"       ,"_\%"   , this, &WaveGen::duty,      &WaveGen::setDuty ),
new stringProp <WaveGen>("File"     , "File"       ,""      , this, &WaveGen::fileName,  &WaveGen::setFile ),
new boolProp<WaveGen>("Always_On"   , "Always On"  ,""      , this, &WaveGen::alwaysOn,  &WaveGen::setAlwaysOn )
    },0} );
    addPropGroup( { "Electric", {
new boolProp<WaveGen>("Bipolar"     , "Bipolar"        ,"" , this, &WaveGen::bipolar,   &WaveGen::setBipolar, propNoCopy ),
new boolProp<WaveGen>("Floating"    , "Floating"       ,"" , this, &WaveGen::floating,  &WaveGen::setFloating, propNoCopy ),
new doubleProp<WaveGen>("Semi_Ampli", "Semi Amplitude" ,"V", this, &WaveGen::semiAmpli, &WaveGen::setSemiAmpli ),
new doubleProp<WaveGen>("Mid_Volt"  , "Middle Voltage" ,"V", this, &WaveGen::midVolt,   &WaveGen::setMidVolt )
    },0} );

    setWaveType( "Sine" );
}
WaveGen::~WaveGen()
{
    //delete m_wavePixmap;
}

bool WaveGen::setPropStr( std::string prop, std::string val )
{
    if( prop =="Volt_Base" ) m_voltBase = std::stod(val); //  Old: TODELETE
    else return ClockBase::setPropStr( prop, val );
    return true;
}

void WaveGen::initialize()
{
    Simulator::self()->cancelEvents( this );
    AnalogClock::self()->remClkElement( this );

    if     ( m_waveType == Wav    ) m_eventTime = m_psPerCycleInt;
    else if( m_waveType == Square ) m_eventTime = m_psPerCycleDbl-m_halfW;
    else if( m_waveType == Random ) m_eventTime = m_psPerCycleInt/3;
    else{
        m_eventTime = 0;
        if( m_isRunning ) AnalogClock::self()->addClkElement( this );
    }
    if( m_isRunning && m_eventTime ) Simulator::self()->addEvent( m_eventTime, this );
}

void WaveGen::stamp()
{
    // ClockBase::stamp();
    m_phaseTime = m_psPerCycleInt*m_phaseShift/360;
    m_lastVout = m_vOut = 0;
    m_index = 0;

    //双极性同下
    if( m_bipolar && m_floating )
    {
        m_outPin->skipStamp( true );
        m_gndPin->skipStamp( true );

        m_outPin->setEnodeComp( m_gndPin->getEnode() );
        m_gndPin->setEnodeComp( m_outPin->getEnode() );

        m_outPin->createCurrent();
        m_gndPin->createCurrent();

        m_outPin->setImp( cero_doub );
        m_gndPin->setImp( cero_doub );
    } 
    else{
        m_outPin->skipStamp( false );
        m_gndPin->skipStamp( false );

        m_outPin->setImp( cero_doub );
        m_gndPin->setImp( cero_doub );
    }

    
}

void WaveGen::runEvent()
{

    m_time = std::fmod( Simulator::self()->circTime() - m_phaseTime, m_psPerCycleDbl );   
    
    //根据 m_waveType 的值调用相应的波形生成函数
    if     ( m_waveType == Sine )     genSine();
    else if( m_waveType == Saw )      genSaw();
    else if( m_waveType == Triangle ) genTriangle();
    else if( m_waveType == Square )   genSquare();
    else if( m_waveType == Random )   genRandom();
    else if( m_waveType == Wav )      genWav();

    if( m_vOut != m_lastVout )
    {
        m_lastVout = m_vOut;

        //判断是否双极性,源码双极性使用了两个电压源，就相当于一个图形就是两个独立电压源，
        //是错误的这里双极性不可用可忽略
        if( m_bipolar )
        {
            double volt = 2 * m_voltage * (m_vOut-0.5); 
            m_outPin->setVoltage( m_voltBase + volt);
            m_gndPin->setVoltage(-(m_voltBase + volt));
        }
        else 
            m_outPin->setVoltage( m_voltBase + 2 *m_voltage * m_vOut);
        
    }
    if( m_eventTime ) Simulator::self()->addEvent( m_eventTime, this );
}


//生成归一化的正弦波输出，并准备下一步的执行。
void WaveGen::genSine()
{
    //将时间步转换为弧度
    m_time = degreesToRadians( (double)m_time*360/m_psPerCycleDbl );
    //利用正弦函数生成波形
    m_vOut = sin( m_time )/2 + 0.5;
}

void WaveGen::genSaw()
{
    m_vOut = m_time/m_psPerCycleDbl;
}

void WaveGen::genTriangle()
{
    if( m_time >= m_halfW ) m_vOut = 1-(m_time-m_halfW)/(m_psPerCycleDbl-m_halfW);
    else                    m_vOut = m_time/m_halfW;
}

void WaveGen::genSquare()
{
    if( m_vOut == 1 )
    {
        m_vOut = 0;
        m_eventTime = m_psPerCycleDbl-m_halfW;
    }else{
        m_vOut = 1;
        m_eventTime = m_halfW;
    }
    m_eventTime += getRemainer();
}

void WaveGen::genRandom()
{
    m_vOut = (double)rand()/(double)RAND_MAX;
}

void WaveGen::genWav()
{
    m_vOut = m_data.at( m_index );
    m_index++;
    if( m_index >= m_data.size() ) m_index = 0;
}

void WaveGen::setMinSteps( int steps )
{
    if( steps < 10 ) steps = 10;
    m_minSteps = steps;
    WaveGen::setFreq( m_freq );
}
void WaveGen::setDuty( double duty )
{
    if( duty > 100 ) duty = 100;
    m_duty = duty;
    m_halfW = m_psPerCycleDbl*m_duty/100;
}

void WaveGen::setFreq( double freq )
{

    double psPerCycleDbl = 1e6*1e6/freq;

    uint64_t minimum = AnalogClock::self()->getStep()*m_minSteps; // Minimum 100 steps per wave cycle

    if( psPerCycleDbl < minimum ) // Scale Step
    {
        double divider = minimum/psPerCycleDbl;
        AnalogClock::self()->setDivider( std::ceil( divider ) );
    }

    m_psPerCycleDbl = psPerCycleDbl;
    m_psPerCycleInt = psPerCycleDbl;

    m_freq = freq;
    m_remainder = 0;

    setRunning( m_isRunning || m_alwaysOn );

    setDuty( m_duty );

    if     ( m_waveType == Square ) m_eventTime = m_psPerCycleDbl-m_halfW;
    else if( m_waveType == Random ) m_eventTime = m_psPerCycleInt/3;
}

void WaveGen::setLinkedValue(double v, int i)
{
    if( i ) setDuty( v );
    else    setFreq( v );
}

//设置峰峰幅度
void WaveGen::setSemiAmpli( double v )
{
    m_voltage = v;
}

void WaveGen::setMidVolt( double v )
{
    m_voltBase = v;
}

void WaveGen::setBipolar(bool b )
{
    if( m_bipolar == b ) return;
    m_bipolar = b;

    if( !b ) m_gndPin->removeConnector();

    udtProperties();
}

void WaveGen::setFloating( bool f )
{
    if( m_floating == f ) return;
    m_floating = f;

    if( Simulator::self()->isRunning() )  Simulator::self()->stopSim(); 

    udtProperties();
}

void WaveGen::setWaveType( std::string type )
{
    Simulator::self()->pauseSim();
    m_waveTypeStr = type;


    std::string pixmapPath;
    if     ( type == "Sine"    ) { m_waveType = Sine;}
    else if( type == "Saw"     ) { m_waveType = Saw;}
    else if( type == "Triangle") { m_waveType = Triangle;}
    else if( type == "Square"  ) { m_waveType = Square;}
    else if( type == "Random"  ) { m_waveType = Random;}
    else if( type == "Wav"     ) { m_waveType = Wav;}

    udtProperties();
}

void WaveGen::udtProperties()
{
    //if( !m_propDialog ) return;

    bool showFile = false;
    bool showDuty = false;
    bool showSteps = true;

    switch( m_waveType ) {
    case Triangle: showDuty = true; break;
    case Square:   showDuty = true; showSteps = false; break;
    case Wav:      showFile = true; showSteps = false; break;
    default: break;
    }
    //m_propDialog->showProp("File", showFile );
    //m_propDialog->showProp("Duty", showDuty );
    //m_propDialog->showProp("Steps", showSteps );

    //m_propDialog->showProp("Mid_Volt", !m_bipolar || !m_floating );
    //m_propDialog->showProp("Floating", m_bipolar );
}

//void WaveGen::slotProperties()
//{
//    Component::slotProperties();
//    udtProperties();
//}
//
//void WaveGen::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
//{
//    if( m_waveType == Wav )
//    {
//        QAction* loadAction = menu->addAction( QIcon(":/load.svg"),tr("Load Wav File") );
//        QObject::connect( loadAction, &QAction::triggered, [=](){ slotLoad(); } );
//
//        menu->addSeparator();
//    }
//    Component::contextMenu( event, menu );
//}
//
//void WaveGen::slotLoad()
//{
//    std::string fil = m_background;
//    if( fil.isEmpty() ) fil = Circuit::self()->getFilePath();
//
//    const std::string dir = fil;
//
//    std::string fileName = QFileDialog::getOpenFileName( 0l, tr("Load Image"), dir,
//                       tr("Wav files (*.wav);;All files (*.*)"));
//
//    if( fileName.isEmpty() ) return; // User cancels loading
//
//    setFile( fileName );
//}

void WaveGen::setFile( std::string fileName )
{
    if( Simulator::self()->isRunning() )  Simulator::self()->stopSim();

    m_fileName = fileName;
    if (fileName.empty()) return;

    // 使用 C++17 文件系统库获取电路文件的目录路径
    std::filesystem::path circuitDir = std::filesystem::path(Circuit::self()->getFilePath()).parent_path();
    std::filesystem::path fileNameAbs = circuitDir / fileName;

    // 检查文件是否存在
    if (!std::filesystem::exists(fileNameAbs))
    {
        std::cerr << "WaveGen::setFile Error: file doesn't exist:\n" << fileNameAbs << "\n";
        return;
    }

    // 使用标准 fstream 打开文件
    std::fstream WAVFile(fileNameAbs, std::ios::binary | std::ios::in | std::ios::out);
    if (!WAVFile.is_open())
    {
        std::cerr << "WaveGen::setFile Could not open:\n" << fileNameAbs << "\n";
        return;
    }

    Simulator::self()->pauseSim();

    char strm[4];
    int32_t dataSize = 0;
    while (true) // Read Header
    {
        WAVFile.read(strm, 4); // 4 File Format = "RIFF"
        if (std::string(strm, 4) != "RIFF") break;
        WAVFile.read(strm, 4); // 4 File Size (ignored)
        WAVFile.read(strm, 4); // 4 File Type = "WAVE"
        if (std::string(strm, 4) != "WAVE") break;
        WAVFile.read(strm, 4); // 4 Format section header = "fmt "
        if (std::string(strm, 4) != "fmt ") break;
        WAVFile.read(strm, 4); // 4 Size of Format section (ignored)
        WAVFile.read(strm, 2); // 2 Format type
        m_audioFormat = convertFromLittleEndian<uint16_t>(strm);
        WAVFile.read(strm, 2); // 2 Number of channels
        m_numChannels = convertFromLittleEndian<uint16_t>(strm);
        WAVFile.read(strm, 4); // 4 Sample rate
        m_sampleRate = convertFromLittleEndian<uint32_t>(strm);
        WAVFile.read(strm, 4); // 4 Byte rate (ignored)
        WAVFile.read(strm, 2); // 2 Block size (bytes)
        m_blockSize = convertFromLittleEndian<uint16_t>(strm);
        WAVFile.read(strm, 2); // 2 Bits per sample
        m_bitsPerSample = convertFromLittleEndian<uint16_t>(strm);
        WAVFile.read(strm, 4); // 4 Data section header = "data"
        std::string section(strm, 4);
        while (section != "data")
        {
            std::cerr << "WaveGen::setFile Warning: Section not supported: " << section << "\n";
            WAVFile.read(strm, 4); // 4 Size of Format section
            uint32_t size = convertFromLittleEndian<uint32_t>(strm);
            WAVFile.ignore(size); // Skip the unsupported section data
            WAVFile.read(strm, 4);
            section = std::string(strm, 4);
        }
        WAVFile.read(strm, 4); // Size of Data section
        dataSize = convertFromLittleEndian<int32_t>(strm);
        break;
    }
    if( dataSize )                  // Read samples
    {
        int bytes = m_blockSize/m_numChannels;

        if( m_audioFormat == 1 ) // PCM
        {
           std::cerr << "WaveGen::setFile Audio format: PCM" << m_bitsPerSample << "bits"<<m_numChannels<<"Channels";
            if( bytes == 1 ){
                m_minValue = 0;
                m_maxValue = 255;
            }else if( bytes == 2 ){
                m_minValue = -32768;
                m_maxValue = 32767;
            }
            else {std::cerr << "WaveGen::setFile Error: PCM format"<<bytes<<"bytes"; WAVFile.close(); return;}
        }
        else if( m_audioFormat == 3 ) // IEEE_FLOAT
        {
            if( bytes == 4 || bytes == 8 ){
               std::cout << "WaveGen::setFile Audio format: IEEE_FLOAT" << m_bitsPerSample << "bits"<<m_numChannels<<"Channels";
                m_minValue = -1;
                m_maxValue = 1;
            }
            else {std::cout << "WaveGen::setFile Error: IEEE_FLOAT format"<<bytes<<"bytes"; WAVFile.close(); return;}
        }
        else {std::cout << "WaveGen::setFile Error: Audio format not supported:"; WAVFile.close(); return;}

        std::vector<char>sample(bytes);
        m_data.clear();
        //m_data.resize( m_numChannels );

        for( int block=0; block < dataSize/m_blockSize; ++block )
        {
            for( int ch=0; ch < m_numChannels; ++ch )
            {
                WAVFile.read(sample.data(), bytes);
                if( ch != 0 ) continue;

                if( m_audioFormat == 1 ) // PCM
                {
                    int16_t data = convertFromLittleEndian<int16_t>(sample.data());
                    m_data.emplace_back( normalize( data ) );
                }
                else if( m_audioFormat == 3 ) // IEEE_FLOAT
                {
                    if( bytes == 4 ){         // 32 bits
                        int16_t data = convertFromLittleEndian<float>(sample.data());
                        m_data.emplace_back( normalize( data ) );
                    }
                    else if( bytes == 8 ){    // 64 bits
                        int16_t data = convertFromLittleEndian<double>(sample.data());
                        m_data.emplace_back( normalize( data ) );
                    }
                }
            }
        }
        setFreq( m_sampleRate );
        // setSteps( 1 );
       std::cout << "WaveGen::setFile Success Loaded wav file:\n" << fileNameAbs;
    }
    else std::cout << "WaveGen::setFile Error reading wav file:\n" << fileNameAbs;
   std::cout << "\n";

    WAVFile.close();
}

inline double WaveGen::normalize( double data )
{
    if     ( data > m_maxValue ) return 1;
    else if( data < m_minValue ) return 0;

    data = data-m_minValue;
    data /= (m_maxValue-m_minValue);
    return data;
}

double WaveGen::degreesToRadians(double degrees)
{
    return degrees * (M_PI / 180.0);
}
