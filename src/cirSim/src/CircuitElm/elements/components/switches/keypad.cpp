/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "keypad.h"
//#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "e-node.h"

#include "stringProp.h"
#include "intProp.h"

#define tr(str) simulideTr("KeyPad",str)

Component* KeyPad::construct( std::string type, std::string id )
{ return new KeyPad( type, id ); }

LibraryItem* KeyPad::libraryItem()
{
    return new LibraryItem(
        "KeyPad",
        "Switches",
        "keypad.png",
        "KeyPad",
        KeyPad::construct);
}

KeyPad::KeyPad( std::string type, std::string id )
      : Component( type, id )
      , eElement( id )
{
    //m_graphical = true;
    m_keyLabels = "123456789*0#";
    m_rows = 4;
    m_cols = 3;
    setupButtons();
    //setLabelPos(-8,-16, 0);

    addPropGroup( { "Main", {
new intProp<KeyPad>("Rows"      , "Rows"      ,"_Buttons", this, &KeyPad::rows,      &KeyPad::setRows , propNoCopy,"uint" ),
new intProp<KeyPad>("Cols"      , "Columns"   ,"_Buttons", this, &KeyPad::cols,      &KeyPad::setCols , propNoCopy,"uint" ),
new stringProp<KeyPad>("Key_Labels", "Key Labels",""            , this, &KeyPad::keyLabels, &KeyPad::setKeyLabels )
    },0} );
}
KeyPad::~KeyPad(){}

void KeyPad::stamp()
{
    for( int row=0; row<m_rows; row++ )
    {
        Pin* rowPin = m_pin[row];
        eNode* rowNode = rowPin->getEnode();
        // if( rowNode ) rowNode->setSwitched( true );
        
        for( int col=0; col<m_cols; col++ )
        {
            Pin* colPin = m_pin[m_rows+col];
            eNode* colNode = colPin->getEnode();
            // if( colNode ) colNode->setSwitched( true );
            
            //PushBase* button = m_buttons.at( row*m_cols+col );
            
            //ePin* epin0 = button->getEpin( 0 );
            //epin0->setEnode( rowNode );
            //
            //ePin* epin1 = button->getEpin( 1 );
            //epin1->setEnode( colNode );
}   }   }

void KeyPad::setupButtons()
{
    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();
    
    //m_area = QRectF(-12,-4, 16*m_cols+8, 16*m_rows+8 );
    
    //for( PushBase* button : m_buttons ) 
    //{
    //   m_buttons.removeOne( button );
    //   delete button;
    //}
    for( Pin* pin : m_pin ) deletePin( pin );
    m_pin.resize( m_rows + m_cols );
    
    int labelMax = m_keyLabels.size()-1;
    
    for( int row=0; row<m_rows; row++ )
    {
        //QPoint pinPos = QPoint( m_cols*16, 8+row*16 );
        Pin* pin = new Pin(m_id+"-Pin"+std::to_string(row), 0, this);
        //pin->setLength( 4 );
        m_pin[row] = pin;
        
        for( int col=0; col<m_cols; col++ )
        {
            std::string butId = m_id+"button"+std::to_string(row)+std::to_string(col);
            //PushBase* button = new PushBase( "PushBase", butId );
            //button->SetupButton();
            //button->setParentItem( this );
            //button->setPos( QPointF(col*16+12, 16+row*16 ) );
            //button->setFlag( QGraphicsItem::ItemIsSelectable, false );
            //m_buttons.append( button );
            
            int pos = row*m_cols+col;
            std::string buttonLabel = "";
            //if( pos <= labelMax ) buttonLabel = m_keyLabels.mid( pos, 1 );
            //button->setKey( buttonLabel );

            if( row == 0 )
            {
                int index = m_rows+col;
                //QPoint pinPos = QPoint( col*16, m_rows*16+8);
                Pin* pin = new Pin( m_id+"-Pin"+std::to_string(index), 0, this);
                //pin->setLength( 4 );
                m_pin[index] = pin;
            }
            //Circuit::self()->update();
    }   }
    //setflip();
}

void KeyPad::setRows( int rows )
{
    if( rows < 1 ) rows = 1;
    m_rows = rows;
    setupButtons();
}

void KeyPad::setCols( int cols )
{
    if( cols < 1 ) cols = 1;
    m_cols = cols;
    setupButtons();
}

void KeyPad::setKeyLabels( std::string keyLabels )
{
    m_keyLabels = keyLabels;
    int labelMax = m_keyLabels.size()-1;
    
    for( int row=0; row<m_rows; row++ ){
        for( int col=0; col<m_cols; col++ )
        {
            //PushBase* button = m_buttons.at( row*m_cols+col );
            
            int pos = row*m_cols+col;
            std::string buttonLabel = "";
            //if( pos <= labelMax ) buttonLabel = m_keyLabels.mid( pos, 1 );
            //button->setKey( buttonLabel );
}   }   }

void KeyPad::remove()
{
    //for( PushBase* button : m_buttons ) delete button;
    Component::remove();
}

