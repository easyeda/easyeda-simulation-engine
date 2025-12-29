/***************************************************************************
 *   Copyright (C) 202 by Santiago González                                *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"

class LibraryItem;
class eNode;
class Pin;

class Tunnel : public Component
{
    public:
        Tunnel( std::string type, std::string id );
        ~Tunnel();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        std::string name() { return m_name; }
        void setName( std::string name );

        bool isBus();
        void setIsbus( bool b );

        //bool rotated() { return m_rotated; }
        //void setRotated( bool rot );

        virtual void remove() override;

        virtual void registerEnode( eNode* enode, int n=-1 ) override;

        std::string tunnelUid() { return m_tunUid; }
        void setTunnelUid( std::string uid ) { m_tunUid = uid; }

        void setEnode( eNode* node, int n=-1 );
        void setPacked( bool p );
        void removeTunnel();
        bool m_show;

        Pin* getPin() { return m_pin[0]; }
 static eNode* getEnode( std::string n );

 static void clearTunnels();

    protected:

        //void showHide( bool show );
        void setGroupName( std::string name, bool single );

        int m_size;

        std::string m_name;
        std::string m_tunUid;

        bool m_rotated;
        bool m_blocked;
        bool m_packed;


 static std::unordered_map<std::string, std::vector<Tunnel*>*> m_tunnels;
};

