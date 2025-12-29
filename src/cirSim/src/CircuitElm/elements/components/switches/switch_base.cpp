/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "switch_base.h"
#include "simulator.h"
#include "circuit.h"
#include "label.h"
#include "custombutton.h"


SwitchBase::SwitchBase( std::string type, std::string id )
          : MechContact( type, id )
{
    //m_graphical = true;
    m_changed = true;
    m_key = "";

    m_ePin.resize(2);

    m_button = new CustomButton(1);               //-------------------------------------------开关
}
SwitchBase::~SwitchBase(){
    //QObject::disconnect( m_keyEventConn );
}

void SwitchBase::updateStep()
{
    if( m_changed )
    {
        setSwitch( m_closed );
        m_changed = false;
}   }

void SwitchBase::rotateText( double angle )
{
    //double rot = m_proxy->rotation();
    //m_proxy->setRotation( rot + angle );
}

void SwitchBase::onbuttonclicked()
{
    if(m_closed != m_button->isChecked()){
        m_closed = m_button->isChecked();
    }
    m_changed = true;
}

void SwitchBase::setKey( std::string key )
{
    if( key.size()>1 ) 
        key = key.substr(0, 1);
    // 检查字符串是否为小写
    bool isLower = std::all_of(key.begin(), key.end(), [](unsigned char c) 
        {
        return std::islower(c);
        });

    // 如果是小写，则转换为大写
    if (isLower)
    {
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c)
            {
                return std::toupper(c);
            });
    }

    // 将处理后的key赋值给m_key
    m_key = key;
    //m_button->setText( key );
}
