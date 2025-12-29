/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "ledmatrix.h"
#include "itemlibrary.h"
//#include "circuitwidget.h"
#include "connector.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"

#include "doubleProp.h"
#include "stringProp.h"
#include "boolProp.h"
#include "intProp.h"

//#define tr(str) simulideTr("LedMatrix",str)

Component* LedMatrix::construct( std::string type, std::string id )
{ return new LedMatrix( type, id ); }

LibraryItem* LedMatrix::libraryItem()
{
    return new LibraryItem(
        "LedMatrix",
        "Leds",
        "ledmatrix.png",
        "LedMatrix",
        LedMatrix::construct);
}

LedMatrix::LedMatrix( std::string type, std::string id )
         : Component( type, id )
         , eElement( id )
{
    //m_graphical = true;

    m_rows = 8;
    m_cols = 8;
    m_resist  = 0.6;
    m_maxCurr = 0.02;
    m_threshold = 2.4;

    //m_color = QColor(0,0,0);
    m_verticalPins = false;
    createMatrix();

    addPropGroup( { "Main", {
        new stringProp <LedMatrix>("Color", "Color","", this
                  , &LedMatrix::colorStr, &LedMatrix::setColorStr,0,"enum" ),

        new intProp <LedMatrix>("Rows", "Rows","_Leds", this
                  , &LedMatrix::rows, &LedMatrix::setRows, propNoCopy,"uint" ),

        new intProp <LedMatrix>("Cols", "Columns","_Leds", this
                  , &LedMatrix::cols, &LedMatrix::setCols, propNoCopy,"uint" ),

        //new boolProp<LedMatrix>("Vertical_Pins", "Vertical Pins","", this
        //          , &LedMatrix::verticalPins, &LedMatrix::setVerticalPins, propNoCopy ),
    }, 0} );

    addPropGroup( { "Electric", {
        new doubleProp<LedMatrix>("Threshold", "Forward Voltage","V", this
                  , &LedMatrix::threshold, &LedMatrix::setThreshold ),

        new doubleProp<LedMatrix>("MaxCurrent", "Max Current","A", this
                  , &LedMatrix::maxCurrent, &LedMatrix::setMaxCurrent ),

        new doubleProp<LedMatrix>("Resistance", "Resistance","Ω", this
                  , &LedMatrix::res, &LedMatrix::setRes ),
    }, 0} );
}
LedMatrix::~LedMatrix(){}

void LedMatrix::stamp()
{
    for( int row=0; row<m_rows; ++row )
    {
        eNode* rowEnode = m_rowPin[row]->getEnode();
        for( int col=0; col<m_cols; ++col )
        {
            eNode* colEnode = m_colPin[col]->getEnode();
            LedSmd* lsmd = m_led[row][col];
            lsmd->getEpin(0)->setEnode( rowEnode );
            lsmd->getEpin(1)->setEnode( colEnode );
}   }   }

void LedMatrix::setupMatrix( int rows, int cols )
{
    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    deleteMatrix();
    m_rows = rows;
    m_cols = cols;
    createMatrix();

    //Circuit::self()->update();
}

void LedMatrix::createMatrix()
{
    //猜测为初始化显示led矩阵位置
    //if( m_verticalPins ) m_area = QRect( -4, -8, m_cols*8, m_rows*8+8 );
    //else                 m_area = QRect( -8, -8, m_cols*8+8, m_rows*8+8 );
    
    m_led.resize( m_rows, std::vector<LedSmd*>(m_cols) );
    m_rowPin.resize( m_rows );
    m_colPin.resize( m_cols );
    m_pin.resize( m_rows+m_cols );
    
    for( int row=0; row<m_rows; ++row )
    {
        std::string pinId = m_id;
        pinId.append( std::string("-pinRow"+std::to_string(row)));
        //QPoint nodpos;
        //int angle;
        //if( m_verticalPins ) { nodpos = QPoint( row*8, -16 ); angle = 90; }
        //else                 { nodpos = QPoint(-16, row*8 ); angle = 180; }

        m_rowPin[row] = new Pin(pinId, 0, this);
        m_pin[row] = m_rowPin[row];
        
        for( int col=0; col<m_cols; ++col )
        {
            std::string ledid = m_id;
            ledid.append( std::string( "-led"+std::to_string(row)+"_"+std::to_string(col) ) );
            LedSmd* lsmd = new LedSmd( "LEDSMD", ledid);

            //lsmd->setParentItem(this);
            //lsmd->setPos( col*8, row*8 );
            lsmd->setRes( m_resist );
            lsmd->setMaxCurrent( m_maxCurr );
            lsmd->setThreshold( m_threshold );
            lsmd->setColorStr( colorStr() );
            //lsmd->setFlag( QGraphicsItem::ItemIsSelectable, false );
            //lsmd->setAcceptedMouseButtons(0);
            m_led[row][col] = lsmd;
            //Circuit::self()->compList()->remove( lsmd );
    }   }
    for( int col=0; col<m_cols; ++col )
    {
        std::string pinId = m_id;
        pinId.append( std::string("-pinCol"+std::to_string(col)));
        //QPoint nodpos = QPoint( col*8, m_rows*8+8 );
        m_colPin[col] = new Pin(pinId, 1, this);
        m_pin[m_rows+col] = m_colPin[col];
}   }

void LedMatrix::deleteMatrix()
{
    for( int row=0; row<m_rows; ++row )
    {
        for( int col=0; col<m_cols; ++col ) delete m_led[row][col];
        deletePin( m_rowPin[row] );
    }
    for( int col=0; col<m_cols; col++ ) deletePin( m_colPin[col] );

    m_led.clear();
    m_rowPin.clear();
    m_colPin.clear();
    m_pin.clear();
}

std::string LedMatrix::colorStr()
{
    if( m_led[0][0] ) return m_led[0][0]->colorStr();
    else              return "Yellow";
}

void LedMatrix::setColorStr( std::string color )
{
    for( int row=0; row<m_rows; ++row )
        for( int col=0; col<m_cols; ++col )
            m_led[row][col]->setColorStr( color );
}

std::vector<std::string> LedMatrix::getEnumUids( std::string ){ return m_led[0][0]->getEnumUids(""); }
std::vector<std::string> LedMatrix::getEnumNames( std::string ){ return m_led[0][0]->getEnumNames(""); }

void LedMatrix::setRows( int rows )
{
    if( rows == m_rows ) return;
    if( rows < 1 ) rows = 1;
    setupMatrix( rows, m_cols );
}

void LedMatrix::setCols( int cols )
{
    if( cols == m_cols ) return;
    if( cols < 1 ) cols = 1;
    setupMatrix( m_rows, cols );
}

//void LedMatrix::setVerticalPins( bool v )
//{
//    if( v == m_verticalPins ) return;
//    m_verticalPins = v;
//    
//    if( v ){
//        for( int i=0; i<m_rows; ++i )
//        {
//            m_rowPin[i]->setPos( i*8, -16 );
//            m_rowPin[i]->setRotation( 90 );
//    }   }
//    else{
//        for( int i=0; i<m_rows; ++i )
//        {
//            m_rowPin[i]->setPos( -16, i*8 );
//            m_rowPin[i]->setRotation( 0 );
//    }   }
//    for( int i=0; i<m_rows; ++i ) m_rowPin[i]->isMoved();
//    
//    if( m_verticalPins ) m_area = QRect( -4, -8, m_cols*8, m_rows*8+8 );
//    else                 m_area = QRect( -8, -8, m_cols*8+8, m_rows*8+8 );
//    
//    update();
//}

void LedMatrix::setThreshold( double threshold ) 
{ 
    if( threshold < 1e-6 ) threshold = 1e-6;
    m_threshold = threshold;
    
    for( int row=0; row<m_rows; ++row )
    {
        for( int col=0; col<m_cols; ++col )
            m_led[row][col]->setThreshold( threshold ); 
}   }

void LedMatrix::setMaxCurrent( double current ) 
{
    if( current < 1e-6 ) current = 1e-6;
    m_maxCurr = current;
    
    for( int row=0; row<m_rows; ++row )
    {
        for( int col=0; col<m_cols; ++col )
            m_led[row][col]->setMaxCurrent( current );
}   }

void LedMatrix::setRes( double resist )
{
    if( resist == 0 ) resist = 1e-14;
    m_resist = resist;
    
    for( int row=0; row<m_rows; ++row )
    {
        for( int col=0; col<m_cols; ++col )
            m_led[row][col]->setRes( resist ); 
}   }


void LedMatrix::remove()
{
    deleteMatrix();
    Component::remove();
}

