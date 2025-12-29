/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "chip.h"

class Tunnel;
class LibraryItem;

class SubCircuit : public Chip
{
    public:
        SubCircuit( std::string type, std::string id );
        ~SubCircuit();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void remove() override;

        virtual void setLogicSymbol( bool ls ) override;

        std::vector<Tunnel*> getPinTunnels();
        Component* getMainComp( std::string name="" );

        virtual std::string toString() override;

    protected:
        void loadSubCircuit( std::string file );

        virtual Pin* addPin( std::string id, std::string type, std::string label,int pos) override;

        virtual Pin* updatePin( std::string id, std::string type, std::string label, int pos) override;

        std::unordered_map<std::string, Component*> m_mainComponents;
        static std::string m_subcDir;      // Subcircuit Path

        std::unordered_set<Component*>       m_compList;
        std::vector<Tunnel*>          m_subcTunnels;
        std::unordered_map<std::string, Tunnel*> m_pinTunnels;
};

