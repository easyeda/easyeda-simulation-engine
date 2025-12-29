/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "compbase.h"
#include "updatable.h"

class Pin;
class eNode;
class Label;
class Connector;
//class ConnectorLine;
class Linkable;

//元件类
class Component :public Updatable, public CompBase
{

    public:

        Component( std::string type, std::string id );
        ~Component();

        virtual bool setPropStr( std::string prop, std::string val ) override;
        static void substitution( std::string& propName );



        bool isMainComp() { return m_isMainComp; }
        void setMainComp( bool m ) { m_isMainComp = m; }

        std::string invertPins() { return m_invertedPins; }
        void setInvertPins( std::string pins );

        virtual std::vector<Pin*> getPins() { return m_pin; }
        virtual Pin* getPin( std::string pinName ){ return nullptr; }



        virtual void registerEnode( eNode*, int n=-1 ) {;}

        virtual void pinMessage( int ){;}

        virtual void remove();

        void deletePin( Pin* pin );
        void addSignalPin( Pin* pin );
        void remSignalPin( Pin* pin );

        // Link components
        virtual void setLinkedValue( double v, int i=0 ){;}
        virtual void setLinkedString( std::string str, int i=0 ){;}
         virtual bool setLinkedTo( Linkable* li );
        bool isLinked() { return m_linkedTo != nullptr; }

        bool m_isLinker;
        int m_linkNumber;
        bool ctl;                 //控制属性注入，暂时只有电压

        //自己加的用于更新状态器件
        static bool m_boardMode;
        virtual void updateState(){;}
        virtual void updateState(std::string mes){;}
        virtual void updateState(int index){;}


        protected:
        virtual void findHelp(){;}


        bool m_isMainComp;
        bool m_showId;
        bool m_warning;
        bool m_crashed;

        double  m_opCount;
        

 static int m_error;


        Label* m_idLabel;
        Label* m_valLabel;
        
        Linkable* m_linkedTo;

        std::vector<Connector*> m_conMoveList;
        std::vector<Component*> m_compMoveList;

        std::vector<Pin*> m_pin;
        std::vector<Pin*> m_signalPin;

        std::string m_invertedPins;
};

typedef Component* (*createItemPtr)( std::string type, std::string id );


