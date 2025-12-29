/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "logiccomponent.h"

//触发器的基础功能
class FlipFlopBase : public LogicComponent
{
    public:
        FlipFlopBase( std::string type, std::string id );
        ~FlipFlopBase();

        bool srInv() { return m_srInv; }
        void setSrInv( bool inv );

        bool pinsRS() { return m_useRS; }
        void usePinsRS( bool rs );

        virtual void stamp() override;
        void updateStep() override;
        virtual void voltChanged() override;
        virtual void runEvent() override{ IoComponent::runOutputs(); }

        bool sPinState();
        bool rPinState();

    protected:
        virtual void calcOutput(){;}
        bool m_useRS;
        bool m_srInv;
        bool m_Q0;

        int m_dataPins;

        IoPin* m_setPin;
        IoPin* m_rstPin;
};
