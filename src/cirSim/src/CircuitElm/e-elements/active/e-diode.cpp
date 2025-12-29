/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/
// Based on Falstad Circuit Simulator Diode model: https://falstad.com

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "e-diode.h"
#include "e-pin.h"
#include "e-node.h"
#include "simulator.h"
#include "utils.h"

std::unordered_map<std::string, diodeData_t> eDiode::m_diodes;
std::unordered_map<std::string, diodeData_t> eDiode::m_zeners;
std::unordered_map<std::string, diodeData_t> eDiode::m_leds;

eDiode::eDiode( std::string id )
      : eResistor( id )
{
    m_vt = 0.025865;
    m_vzCoef = 1/m_vt;
    m_maxCur = 1;
}
eDiode::~eDiode(){}

void eDiode::stamp()
{
    m_converged = true;
    m_admit   = m_bAdmit;
    m_voltPN  = 0;
    m_current = 0;

    eResistor::stamp();

    eNode* node = m_ePin[0]->getEnode();
    if( node ) node->addToNoLinList( this );

    node = m_ePin[1]->getEnode();
    if( node ) node->addToNoLinList( this );

    m_ePin[0]->createCurrent();
    m_ePin[1]->createCurrent();
}

void eDiode::voltChanged()
{
    double voltPN = m_ePin[0]->getVoltage() - m_ePin[1]->getVoltage();

    if( m_changed ) m_changed = false;
    else if( std::fabs( voltPN - m_voltPN ) < .01 ) 
    { 
        m_step = 0; m_converged = true; return;     // Converged
    } 
    m_converged = false;
    Simulator::self()->notCorverged();

    m_step += .01;
    double gmin = m_bAdmit*std::exp( m_step );
    if( gmin > .1 ) gmin = .1;

    if( voltPN > m_vCriti && std::fabs(voltPN - m_voltPN) > m_vScale*2 ) // check new voltage; has current changed by factor of e^2?
    {
        voltPN = limitStep( voltPN, m_voltPN, m_vScale, m_vCriti );
    }
    else if( m_bkDown != 0 && voltPN < 0 )
    {
        voltPN = -voltPN - m_zOfset;
        double vold = -m_voltPN - m_zOfset;

        if( voltPN > m_vzCrit && std::fabs(voltPN - vold) > m_vt*2 )
            voltPN = limitStep( voltPN, vold, m_vt, m_vzCrit );
        voltPN = -(voltPN+m_zOfset);
    }
    m_voltPN = voltPN;

    double eval = std::exp( voltPN*m_vdCoef );

    if( m_bkDown == 0 || voltPN >= 0  )  // No breakdown Diode or Forward biased Zener
    {
        m_admit   = m_satCur * m_vdCoef*eval + gmin;
        m_current = m_satCur * (eval-1);
    }
    else
    {                               
        // Reverse biased Zener or Diode with breakdown
        double expCoef = std::exp( (-voltPN-m_zOfset)*m_vzCoef );
        m_admit   = m_satCur * ( m_vdCoef*eval + m_vzCoef*expCoef ) + gmin;
        m_current = m_satCur * ( eval-1 - expCoef ) ;
    }
    eResistor::stampAdmit();

    double stCurr = m_current - m_admit*voltPN;
    m_ePin[0]->stampCurrent(-stCurr );
    m_ePin[1]->stampCurrent( stCurr );
}

inline double eDiode::limitStep( double vnew, double vold, double scale, double vc )
{
    if( vold > 0 )
    {
        double arg = 1 + (vnew-vold)/scale;
        if( arg > 0 ) vnew = vold + scale*std::log( arg );
        else          vnew = vc;
    }
    else             
        vnew = scale*std::log( vnew/scale );

    return vnew;
}

void eDiode::SetParameters( double sc, double ec, double bv, double sr )
{
    m_satCur = sc;
    m_emCoef = ec;
    m_bkDown = bv;
    setResSafe( sr );

    updateValues();
}

void eDiode::setThreshold( double vCrit )
{
    if( vCrit < 0.01 ) return;
    m_satCur = m_vScale/(std::exp(vCrit/m_vScale)*std::sqrt(2));
    updateValues();
}

void eDiode::setSatCur( double satCur )
{
    if( satCur <= 0 ) return;
    m_satCur = satCur;
    updateValues();
}

void eDiode::setEmCoef( double emCoef )
{
    if( emCoef < 0.01 ) return;
    m_emCoef = emCoef;
    updateValues();
}

void  eDiode::setBrkDownV( double bkDown )
{
    if( bkDown >= 0 ) m_bkDown = bkDown;
    else              m_bkDown = -bkDown;
    updateValues();
}

void eDiode::updateValues()
{
    m_vScale = m_emCoef*m_vt;
    m_vdCoef = 1/m_vScale;
    m_vCriti = m_vScale*std::log( m_vScale/(std::sqrt(2)*m_satCur) );
    m_zOfset = m_bkDown - m_vt*std::log(-(1-0.005/m_satCur) );
    m_vzCrit = m_vt*std::log( m_vt/(std::sqrt(2)*m_satCur) );
    m_bAdmit = m_satCur*1e-2;
    m_changed = true;
}


void eDiode::getModels() // Static
{
    m_diodes["Custom"] = { 0, 0, 0, 0 };
    m_diodes["Diode Default"] = { 171.4352819281, 2, 0, 0.05 };
    m_zeners["Custom"] = { 0, 0, 0, 0 };
    m_zeners["Zener Default"] = { 171.4352819281, 2, 5.6, 0.05 };
    m_leds["Custom"] = { 0, 0, 0, 0 };
    m_leds["RGY Default"] = { 0.0932, 3.73, 0, 0.042 };

    //std::string modelsFile = MainWindow::self()->getDataFilePath( "diodes.model" );   -----------------------------通用获取文件路径，后续再研究
    std::string modelsFile = "E:\\文档\\公司\\cirsim_sim\\cirSim\\cirSim\\resources\\data";

    TiXmlDocument domDoc(modelsFile.c_str());
    if (!domDoc.LoadFile())
    {
        std::cerr << "Failed to load file: " << modelsFile << std::endl;
        return;
    }

    TiXmlElement* itemset = domDoc.RootElement()->FirstChildElement("itemset");
    while (itemset)
    {
        const char* typeAttr = itemset->Attribute("type");
        std::string type = typeAttr ? typeAttr : "";
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);

        TiXmlElement* item = itemset->FirstChildElement("item");
        while (item)
        {
            const char* nameAttr = item->Attribute("name");
            std::string name = nameAttr ? nameAttr : "";
            diodeData_t data;
            item->Attribute("satCurr_nA", &data.satCur);
            item->Attribute("emCoef", &data.emCoef);
            item->Attribute("brkDown", &data.brkDow);
            item->Attribute("resist", &data.resist);

            if (type == "diode") m_diodes[name] = data;
            else if (type == "zener") m_zeners[name] = data;
            else if (type == "led") m_leds[name] = data;

            item = item->NextSiblingElement("item");
        }
        itemset = itemset->NextSiblingElement("itemset");
    }
}

void eDiode::setModel( std::string model )
{
    m_model = model;
    if (model == "Custom") return;

    auto diodeIt = m_diodes.find(model);
    if (diodeIt != m_diodes.end())
    {
        setModelData(diodeIt->second);
        return;
    }

    auto zenerIt = m_zeners.find(model);
    if (zenerIt != m_zeners.end())
    {
        setModelData(zenerIt->second);
        return;
    }

    auto ledIt = m_leds.find(model);
    if (ledIt != m_leds.end())
    {
        setModelData(ledIt->second);
        return;
    }
}

void eDiode::setModelData( diodeData_t data )
{
    SetParameters( data.satCur*1e-9, data.emCoef, data.brkDow, data.resist );
}

