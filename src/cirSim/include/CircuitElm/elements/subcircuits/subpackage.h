/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "chip.h"
#include "linkable.h"

class LibraryItem;
//class QAction;

class SubPackage : public Chip,public Linkable
{
        friend class Circuit;

    public:
        SubPackage( std::string type, std::string id );
        ~SubPackage();
        
    static Component* construct( std::string type, std::string id );
    static LibraryItem* libraryItem();
        
        std::string  package();
        void setPackage( std::string package );

        void setEventPin( Pin* pin ) { m_eventPin = pin; }

        void savePackage( std::string fileName );
        virtual void setSubcTypeStr( std::string s ) override;
        virtual void setLogicSymbol( bool ls ) override;

        virtual void remove() override;

        virtual void compSelected( Component* comp ) override;  // Use link mechanism to select main components

        void invertPin( bool invert );
        void setPinId( std::string id );
        void setPinName( std::string name );
        void boardModeSlot();
        void mainComp() { Linkable::startLinking(); }
        void unusePin( bool unuse );
        void deleteEventPin();
    
    protected:
        void loadPackage();
        void slotSave();
        void editFinished( int r );

        virtual Pin* addPin( std::string id, std::string type, std::string label,int pos) override;

    private:
        TiXmlElement* pinEntry( Pin* pin );
        std::string adjustSize( std::string str, int size );

 static std::string m_lastPkg;

        bool m_fakePin; // Data for drawing pin when hovering

        bool m_circPosSaved;
        bool m_boardMode;
        
        Pin* m_eventPin;
        std::vector<Pin*> m_pkgePins;
};

