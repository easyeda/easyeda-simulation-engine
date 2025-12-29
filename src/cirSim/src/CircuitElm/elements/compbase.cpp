/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "compbase.h"
#include "circuit.h"
#include "utils.h"
#include <algorithm>


//#include "propdialog.h"
#include "comProperty.h"

CompBase::CompBase( std::string type, std::string id )
{
    m_id   = id;
    m_type = type;

    //m_propDialog = NULL;
}
CompBase::~CompBase()
{
    for (auto& pair : m_propHash) 
    {
        delete pair.second; // 删除指向 ComProperty 的指针
    }
    m_propHash.clear(); // 清空映射
}

void CompBase::loadProperties( const TiXmlElement* el ) // Set properties in correct order
{
    std::unordered_map<std::string, std::string> properties;

    // 遍历所有属性并添加到 properties 映射中
    const TiXmlAttribute* attribute = el->FirstAttribute();
    while (attribute) 
    {
        properties[attribute->Name()] = attribute->Value();
        attribute = attribute->Next();
    }

    // 遍历所有属性组
    for (const propGroup& group : m_propGroups) 
    {
        if (group.propList.empty()) continue;

        // 遍历属性组中的每个属性
        for (comProperty* prop : group.propList) 
        {
            std::string pn = prop->name();
            auto it = properties.find(pn);
            if (it == properties.end()) continue;

            prop->setValStr(it->second);
            properties.erase(it);
        }
    }
}

void CompBase::remPropGroup( std::string name )
{
    for (auto it = m_propGroups.begin(); it != m_propGroups.end(); ) {
        if (it->name == name) {
            for (comProperty* p : it->propList) {
                m_propHash.erase(p->name());
            }
            it = m_propGroups.erase(it); // erase 返回新的有效迭代器
        }
        else {
            ++it; // 只有在不删除元素时才递增迭代器
        }
    }
}

void CompBase::addPropGroup( propGroup pg, bool list )
{
    m_propGroups.push_back( pg );

    if( list )
        for( comProperty* p : pg.propList ) m_propHash[p->name()] = p;
}

void CompBase::addProperty( std::string group, comProperty* p )
{
    for (auto& pg : m_propGroups) 
    {
        if (pg.name != group) continue;

        pg.propList.push_back(p);
        m_propHash[p->name()] = p;
        return;
    }
}

void CompBase::removeProperty( std::string prop )
{
    for (auto& pg : m_propGroups) {
        for (auto it = pg.propList.begin(); it != pg.propList.end(); ) {
            comProperty* p = *it;
            if (p->name() != prop) {
                ++it;
                continue;
            }
            it = pg.propList.erase(it); // erase 返回新的有效迭代器
            m_propHash.erase(prop);
            delete p;
            return; // 退出函数，因为属性已被删除
        }
    }
}


bool CompBase::setPropStr( std::string prop, std::string val )
{
    auto it = m_propHash.find(prop);
    if (it != m_propHash.end()) 
    {
        comProperty* p = it->second;
        p->setValStr(val);
        // if (m_propDialog) m_propDialog->updtValues(); // 如果有对话框更新逻辑，需要替换为适当的 C++ 代码
        return true;
    }
    else 
    {
        return false;
    }
}
std::string CompBase::getPropStr( std::string prop )
{
    auto it = m_propHash.find(prop);
    if (it != m_propHash.end()) 
    {
        return it->second->getValStr();
    }
    else 
    {
        return "";
    }
}

std::string CompBase::toString() // Used to save circuit
{
    std::string item = "\n<item ";
    for( propGroup pg : m_propGroups )
    {
        if( !Circuit::self()->getBoard() )     // Not a Subcircit Board
        {
            if( pg.name == "Board") continue;  // Don't save Board properties
        }

        for( comProperty* prop : pg.propList )
        {
            std::string val = prop->toString();
            if( val.empty() ) continue;
            item += prop->name() + "=\""+val+"\" ";
    }   }
    item += "/>\n";

    return item;
}

int CompBase::getEnumIndex( std::string prop )
{
    // 这里需要替换为适当的字符串到整数的转换逻辑
    // int index = std::stoi(prop, &ok); // 假设 stoi 可以用来检查转换是否成功
    // if (!ok) {
    auto it = std::find(m_enumUids.begin(), m_enumUids.end(), prop);
    int index = (it != m_enumUids.end()) ? std::distance(m_enumUids.begin(), it) : 0;
    // }
    if (index < 0 || index >= static_cast<int>(m_enumUids.size())) index = 0;
    return index;
}
