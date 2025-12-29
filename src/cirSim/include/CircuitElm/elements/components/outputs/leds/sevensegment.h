/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"
#include "ledsmd.h"

class LibraryItem;
//七段数码管
class SevenSegment : public Component, public eElement
{
    public:
        SevenSegment( std::string type, std::string id );
        ~SevenSegment();

 static Component* construct( std::string type, std::string id );
 static LibraryItem *libraryItem();

        std::string colorStr();
        void setColorStr( std::string color );

        int numDisplays() { return m_numDisplays; }
        void setNumDisplays( int dispNumber );
        
        bool isComCathode() { return m_commonCathode; }
        void setComCathode( bool isCommonCathode );
        
        double threshold() { return m_threshold; }
        void   setThreshold( double threshold );
        
        double maxCurrent() { return m_maxCurrent; }
        void   setMaxCurrent( double current );

        double resistance() { return m_resistance; }
        void   setResistance( double res );

        virtual void stamp() override;

        virtual std::string getElementStatusData() override;

        virtual std::vector<std::string> getEnumUids( std::string ) override;
        virtual std::vector<std::string> getEnumNames( std::string ) override;

        //virtual void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

    private:
        void createDisplay( int n );
        void deleteDisplay( int n );
        void resizeData( int displays );

        bool m_commonCathode;  //共阴极/阳极
        
        int  m_numDisplays;  //数码管心事位数
        double m_threshold;  //数码管导通电压阈值
        double m_maxCurrent; //数码管最大工作电流
        double m_resistance; //数码管等效电阻

        std::vector<Pin*>   m_commonPin; //数码管公共引脚
        std::vector<ePin*>  m_cathodePin; //数码管阴极引脚
        std::vector<ePin*>  m_anodePin;   //数码管阳极引脚
        std::vector<LedSmd*> m_segment;   //数码管led对象
        eNode* m_enode[8];                //数码管节点
};


