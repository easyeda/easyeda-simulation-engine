/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

// #pragma once

// #include <string>
// #include <vector>

// #include "iocomponent.h"
// //#include "scriptmodule.h"

// class LibraryItem;
// class CustomButton;
// class QGraphicsProxyWidget;

// //通用功能的实现和声明。
// class Function : public IoComponent       /*, public ScriptModule*/
// {
//     public:
//         Function( std::string type, std::string id );
//         ~Function();
        
//         static Component* construct( std::string type, std::string id );
//         static LibraryItem* libraryItem();

//         virtual void stamp() override;
//         virtual void voltChanged() override;
//         virtual void runEvent() override { IoComponent::runOutputs(); }

//         std::string functions() { return m_funcList.join(","); }
//         void setFunctions( std::string f );

//         virtual void remove() override;
        
//         void setNumInputs( int inputs );
//         void setNumOutputs( int outs );

//         bool   getInputState( int pin );
//         double getInputVoltage( int pin );
//         void   setOutputState( int pin, bool s );
//         void   setOutputVoltage( int pin, double v );
//         double getOutputVoltage( int pin );
        
//     //public slots:
//     //    void onbuttonclicked( int i );
//     //    void loadData();
//     //    void saveData();
//     //    
//     //protected:
//     //    virtual void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu ) override;

//     private:
//         void updateFunctions();
//         void updateArea( uint ins, uint outs );

//         asIScriptFunction* m_voltChanged;
//         QStringList m_funcList;

//         QList<CustomButton*> m_buttons;
//         QList<QGraphicsProxyWidget*> m_proxys;

//         std::string m_lastDir;
// };

