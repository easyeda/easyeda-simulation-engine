/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "logiccomponent.h"

class LibraryItem;

//BCD 到十进制数字的转换功能
class BcdToDec : public LogicComponent
{
    public:
        BcdToDec( std::string type, std::string id );
        ~BcdToDec();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();
        
        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override{ IoComponent::runOutputs(); }

        bool is16Bits() { return m_16Bits; }
        void set_16bits( bool set );

   private:
        bool m_16Bits;
};


