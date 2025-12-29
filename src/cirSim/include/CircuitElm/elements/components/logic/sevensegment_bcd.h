/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "bcdbase.h"

class LibraryItem;

//BCD 到 7 段显示器
class SevenSegmentBCD : public BcdBase
{
    public:
        SevenSegmentBCD( std::string type, std::string id );
        ~SevenSegmentBCD();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void updateStep() override;
        virtual void voltChanged() override;

        bool isShowEnablePin() { return m_showEnablePin; }
        void setShowEnablePin( bool show );

        bool isShowDotPin() { return m_showDotPin; }
        void setShowDotPin( bool show );

        bool setLinkedTo( Linkable* li ) override;
        virtual void setLinkedValue( double v, int i=0  ) override;

    private:
        bool m_showEnablePin;
        bool m_showDotPin;

        IoPin *m_dotPin;
        IoPin *m_enablePin;
};

