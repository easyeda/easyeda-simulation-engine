/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "logiccomponent.h"

class IoPin;


class Gate : public LogicComponent
{
    public:
        Gate( std::string type, std::string id, int inputs );
        ~Gate();

        std::vector<comProperty*> outputProps();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        bool initHigh() { return m_initState; }
        void setInitHigh( bool s) { m_initState = s; }

        void setNumInputs( int pins );

    protected:
        virtual bool calcOutput( int inputs );

        bool m_out;
        bool m_initState;

        int m_minInputs;
};

