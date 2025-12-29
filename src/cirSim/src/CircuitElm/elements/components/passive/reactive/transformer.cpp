/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "transformer.h"
#include "itemlibrary.h"
//#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"

#include "doubleProp.h"
#include "stringProp.h"

#define tr(str) simulideTr("Transformer",str)

Component* Transformer::construct( std::string type, std::string id )
{ return new Transformer( type, id ); }

LibraryItem* Transformer::libraryItem()
{
    return new LibraryItem(
        "Transformer",
        "Reactive",
        "transformer.png",
        "Transformer",
        Transformer::construct );
}

Transformer::Transformer( std::string type, std::string id )
           : Component( type, id )
           , eElement( id )
{
    m_coupCoeff = 0.99;
    m_baseInd = 1;
    m_lenght = 0;

    m_primary.prefix = m_id+"-P";
    m_primary.angle = 180;
    m_primary.x     =-16;

    m_secondary.prefix = m_id+"-S";
    m_secondary.angle = 0;
    m_secondary.x     = 16;

    addPropGroup( { "Main", {
        new doubleProp<Transformer>("CoupCoeff", "Coupling Coefficient", "", this, &Transformer::coupCoeff, &Transformer::setCoupCoeff ),
        new doubleProp<Transformer>("BaseInd"  , "Base Inductance", "H", this, &Transformer::baseInd,   &Transformer::setBaseInd ),
        new stringProp <Transformer>("Primary"  ,"Primary"        , "" , this, &Transformer::primary,   &Transformer::setPrimary  , propNoCopy ),
        new stringProp <Transformer>("Secondary","Secondary"      , "" , this, &Transformer::secondary, &Transformer::setSecondary, propNoCopy ),
    },0 } );

    setPrimary("1");
    setSecondary("1");
}
Transformer::~Transformer() {}

void Transformer::stamp()
{
    m_coils.clear();

    setupInducts( &m_primary );
    setupInducts( &m_secondary );

    m_reacStep = AnalogClock::self()->getStep();
    double tStep = (double)m_reacStep/1e12;

    int i, j, k;
    int n = m_coils.size();

    // Create admitance matrix
    std::vector<std::vector<double>> admit(n, std::vector<double>(n, 0.0));
    for( int i=0; i<n; i++ ) admit[i][i] = m_coils.at(i)->inductance;
    for( int i=0; i<n; i++ )
    {
        eCoil* coili = m_coils.at(i);
        for( int j=0; j<i; ++j )
        {
            eCoil* coilj = m_coils.at(j);
            admit[i][j] = admit[j][i] = m_coupCoeff*std::sqrt( coili->inductance*coilj->inductance )*coili->sign*coilj->sign;
        }
    }

    // Invert matrix
    for( j=0; j<n; ++j )
    {
        for( i=0; i<j; ++i )
        {
            double q = admit[i][j];
            for( k=0; k<i; ++k ) q -= admit[i][k]*admit[k][j];
            admit[i][j] = q;
        }
        for( i=j; i<n; ++i )
        {
            double q = admit[i][j];
            for( k=0; k<j; ++k ) q -= admit[i][k]*admit[k][j];
            admit[i][j] = q;
        }
        if( j == n-1 ) continue;
        for( i=j+1; i<n; ++i ) admit[i][j] /= admit[j][j];
    }
    std::vector<double> b(n, 0.0);
    m_inva.resize( n, std::vector<double>( n, 0 ) );

    for( k=0; k<n; k++)
    {
        for( j=0; j<n; j++) b[j] = 0;
        b[k] = 1;

        double tot;
        for( i=k+1; i<n; ++i )
        {
            tot = b[i];
            for( int j=k; j<i; ++j ) tot -= admit[i][j]*b[j];
            b[i] = tot;
        }
        for( i=n-1; i>=0; --i )
        {
            tot = b[i];
            for( int j=i+1; j<n; ++j ) tot -= admit[i][j]*b[j];
            b[i] = tot/admit[i][i];
        }
        for( int i=0; i<n; i++ ) m_inva[i][k] = b[i]*tStep;
    }

    for( int i=0; i<n; i++ ) // Stamp coils before creating induced currents
    {
        m_coils.at(i)->setAdmit( m_inva[i][i] );
        m_coils.at(i)->stampCoil();
    }
    // Create induced current Sources
    for( int i=0; i<n; i++ )
    {
        eCoil* coili = m_coils.at(i);
        for( int j=0; j<n; j++ )
            if( i != j ) coili->addIductor( m_coils.at(j), m_inva[i][j] );
    }
    Simulator::self()->addEvent( m_reacStep, this );
}

void Transformer::runEvent()
{
    int n = m_coils.size();

    for( int i=0; i<n; i++ )
    {
        double current = 0;
        for( int j=0; j<n; ++j ) current += m_coils.at(j)->getVolt()*m_inva[i][j];
        m_coils.at(i)->stampCurrent( current );
    }
    Simulator::self()->addEvent( m_reacStep, this );
}

void Transformer::setupInducts( winding_t* w )  // Setup Inductances
{
    int pinIndex = -1;

    for( eCoil* coil : w->coils ) // connect coils to Pins
    {
        if( coil->index == 0 ) pinIndex++;

        coil->setEnode( 0, w->pins.at( pinIndex )->getEnode() );
        pinIndex++;
        coil->setEnode( 1, w->pins.at( pinIndex )->getEnode() );
        coil->setBaseInd( m_baseInd );

        m_coils.push_back( coil );
    }
}

void Transformer::setBaseInd( double i )
{
    if( m_baseInd == i ) return;
    if( Simulator::self()->isRunning() ) Simulator::self()->stopSim();  
    m_baseInd = i;
}


void Transformer::setPrimary( std::string p )
{
    if( m_primary.definition == p ) return;
    if( p.empty() ) p = " ";
    m_primary.definition = p;
    createCoils();
}

void Transformer::setSecondary( std::string s )
{
    if( m_secondary.definition == s ) return;
    if( s.empty() ) s = " ";
    m_secondary.definition = s;
    createCoils();
}

void Transformer::setCoupCoeff( double c )
{
    if( m_coupCoeff == c ) return;
    if( Simulator::self()->isRunning() ) Simulator::self()->stopSim();  
    if( c > 0.9999 ) c = 0.9999;
    if( c < 0.0001 ) c = 0.0001;
    m_coupCoeff = c;
}

void Transformer::createCoils()
{
    if( Simulator::self()->isRunning() ) Simulator::self()->stopSim();  

    std::string priTopo = m_primary.topology;
    std::string secTopo = m_secondary.topology;

    int priLenght = getLeght( &m_primary );
    int secLenght = getLeght( &m_secondary );

    if( priLenght > secLenght ) m_lenght = priLenght;
    else                        m_lenght = secLenght;

    m_pin.clear();

    if( priTopo == m_primary.topology   ) modify( &m_primary );
    else                                  create( &m_primary );

    if( secTopo == m_secondary.topology ) modify( &m_secondary );
    else                                  create( &m_secondary );
}

int Transformer::getLeght( winding_t* w )
{
    std::vector<std::string> sectsStr;
    std::istringstream iss(w->definition);
    std::string segment;

    // Split definition by ':'
    while (std::getline(iss, segment, ':')) 
    {
        if (!segment.empty()) 
        {
            sectsStr.push_back(segment);
        }
    }

    int nSections = sectsStr.size();
    w->topology.clear();
    int nCoils = 0;    //初始化一个整型变量nCoils，用来计算总的线圈数量。

    for (int s = 0; s < nSections; ++s) 
    {
        if (s > 0) w->topology.append(":");
        std::istringstream coilStream(sectsStr[s]);
        std::string coil;
        std::vector<std::string> coils;

        // Split section by ','
        while (std::getline(coilStream, coil, ',')) 
        {
            if (!coil.empty()) 
            {
                coils.push_back(coil);
            }
        }

        nCoils += coils.size();
        for (size_t c = 0; c < coils.size(); ++c) 
        {
            if (c > 0) w->topology.append(",");
            w->topology.append("c");
        }
    }

    w->nCoils = nCoils;
    if (nSections == 0) return 0;
    return nCoils * 3 + nSections - 1;
}


//更新变压器绕组的一些属性
void Transformer::modify( winding_t* w )
{
    for( Pin* pin : w->pins ) m_pin.emplace_back( pin );

    std::vector<eCoil*>* coils = &w->coils;

    int nCoils = w->nCoils;

    std::string defin = w->definition;
    std::vector<std::string> sectsStr;
    std::istringstream iss(defin);
    std::string segment;

    while (std::getline(iss, segment, ':')) 
    {
        if (!segment.empty()) 
        {
            sectsStr.push_back(segment);
        }
    }

    int nSections = sectsStr.size();
    if( nSections == 0 ) return; // No coils

    int coilSize = (m_lenght-nSections+1)/nCoils;
    //int y = ((m_lenght-(nCoils*coilSize+nSections-1)))*4; // Position of first Pin
    //w->start = y;

    int c = 0;
    int p = 0;
    for( int sectN=0; sectN<nSections; ++sectN )
    {
        //w->pins.at( p )->setY( y );
        p++;

        // 获取当前段的字符串，并按逗号 ',' 分割成不同的线圈
        std::string sectStr = sectsStr.at(sectN);
        std::vector<std::string> coilsStr;
        std::istringstream coilStream(sectStr);
        std::string coilStr;
        while (std::getline(coilStream, coilStr, ',')) 
        {
            if (!coilStr.empty() && coilStr != "0") 
            {
                coilsStr.push_back(coilStr);
            }
        }

        for( int coilN=0; coilN<coilsStr.size(); ++coilN )
        {
            //y += coilSize*8;
            //w->pins.at( p )->setY( y );
            p++;

            // 转换字符串到 double 并获取线圈的符号
            double rel = std::stod(coilsStr.at(coilN));
            int sign = rel >= 0 ? 1 : -1;

            // 更新线圈的属性
            eCoil* coil = coils->at(c);
            coil->sign = sign;
            coil->size = coilSize;
            coil->relation = rel;
            coil->setBaseInd(m_baseInd);
            c++;
        }
        //y += 8;
    }
    //int l = m_primary.nCoils   ? 12:4;
    //int r = m_secondary.nCoils ? 12:4;
    //m_area = QRect(-l, 0, l+r, 8*m_lenght );
    //Circuit::self()->update();
}

void Transformer::create( winding_t* w  )
{
    std::vector<Pin*>*   pins  = &w->pins;
    std::vector<eCoil*>* coils = &w->coils;

    for( Pin* pin : *pins ) deletePin( pin );
    pins->clear();
    for( eCoil* coil : *coils ) delete coil;
    coils->clear();

    int nCoils = w->nCoils;

    std::string defin = w->definition;
    // 移除 defin 中的所有空格
    defin.erase(std::remove(defin.begin(), defin.end(), ' '), defin.end());

    // 使用 stringstream 和 getline 来分割字符串
    std::vector<std::string> sectsStr;
    std::istringstream iss(defin);
    std::string segment;

    // Split definition by ':'
    while (std::getline(iss, segment, ':'))
    {
        if (!segment.empty())
        {
            sectsStr.push_back(segment);
        }
    }

    int nSections = sectsStr.size();
    if( nCoils == 0 || nSections == 0 ) return; // No coils

    //int angle = w->angle;
    //int x     = w->x;
    int coilSize = (m_lenght-nSections+1)/nCoils;
    //int y = ((m_lenght-(nCoils*coilSize+nSections-1)))*4; // Position of first Pin
    //w->start = y;

    for( int sectN=0; sectN<nSections; ++sectN )
    {
        // 创建一个基于段编号的ID
        std::string baseId = w->prefix + std::to_string(sectN);
        Pin* pin = new Pin(baseId+"0Pin", 0, this );
        //pin->setLength( 6 );
        pins->push_back( pin );
        m_pin.emplace_back( pin );

        std::string sectStr = sectsStr.at( sectN );
        std::vector<std::string> coilsStr;
        std::istringstream coilStream(sectStr);
        std::string coilStr;
        while (std::getline(coilStream, coilStr, ',')) 
        {
            if (!coilStr.empty() && coilStr != "0") 
            {
                coilsStr.push_back(coilStr);
            }
        }

        for( int coilN=0; coilN<coilsStr.size(); ++coilN )
        {
            //y += coilSize*8;
            std::string id = baseId+std::to_string( coilN+1 );

            pin = new Pin(id+"Pin", 0, this );
            //pin->setLength( 6 );
            pins->push_back( pin );
            m_pin.emplace_back( pin );

            // 解析线圈的相关性并创建eCoil对象
            double rel = std::stod(coilsStr.at(coilN));
            int sign = rel>=0 ? 1:-1;

            eCoil* coil = new eCoil( coilN, sign, coilSize, m_baseInd, rel, id+"Coil" );
            coils->push_back( coil );
        }
        //y += 8;
    }
    //int l = m_primary.nCoils   ? 12:4;
    //int r = m_secondary.nCoils ? 12:4;
    //m_area = QRect(-l, 0, l+r, 8*m_lenght );
    //Circuit::self()->update();
}

// Coil structure, example section with 3 coils: (VVVV = resistor)
//
// ┏━VVVV━━⚫ Pin0 -- eNode
// ┃
// ┗━VVVV━━⚫ Pin1 -- eNode
//                      |
// ┏━VVVV--- ePin ------
// ┃
// ┗━VVVV━━⚫ Pin2 -- eNode
//                      |
// ┏━VVVV--- ePin ------
// ┃
// ┗━VVVV━━⚫ Pin3 -- eNode
