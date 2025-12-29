/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "ledsmd.h"

class LibraryItem;

class LedBar : public Component
{
    public:
        LedBar( std::string type, std::string id );
        ~LedBar();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        std::string colorStr();
        void setColorStr( std::string color );

        int  size() { return m_size; }
        void setSize( int size );
        
        double threshold();
        void  setThreshold( double threshold );

        double maxCurrent();
        void  setMaxCurrent( double current );
        
        double res();
        void  setRes( double resist );
        
        bool grounded();
        void setGrounded( bool grounded );
        
        void createLeds( int c );
        void deleteLeds( int d );

        //virtual void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

        virtual void remove() override;

        virtual std::vector<std::string> getEnumUids( std::string ) override;
        virtual std::vector<std::string> getEnumNames( std::string ) override;
        
    private:
        std::vector<LedSmd*> m_led;
        int m_size;
};