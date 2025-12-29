/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "iocomponent.h"
#include "e-clocked_device.h"

class IoPin;

class LogicComponent : public IoComponent, public eClockedDevice
{
    public:
        LogicComponent( std::string type, std::string id );
        ~LogicComponent();

        virtual void stamp() override;

        void createOePin ( std::string d, std::string id ) { setOePin( createPin( d, id ) ); }
        void setOePin( IoPin* pin );
        void enableOutputs( bool en );
        void updateOutEnabled();
        bool outputEnabled();

        virtual void setInpHighV( double volt ) override;
        virtual void setInpLowV( double volt ) override;
        virtual void setInputImp( double imp ) override;

        bool tristate() { return m_tristate; }
        virtual void setTristate( bool t );

        std::string triggerStr() { return m_enumUids.at( (int)m_trigger ); }
        void setTriggerStr( std::string t );

        virtual std::vector<Pin*> getPins() override;

        virtual void remove() override;
    static const std::string m_triggerList;
    protected:
        bool m_outEnable;
        bool m_tristate;
        std::string m_triggerStr;
        IoPin*  m_oePin;
};
