/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"

//CompBase 类提供了一系列用于管理组件属性的方法，以及一些基本的组件信息

class comProperty;

//设置属性组的标志
enum groupFlags{
    groupHidden = 1,     //表示属性组是隐藏的
    groupNoCopy = 1<<1, //表示属性组不可被复制
};

//表示属性组
struct propGroup
{
    std::string name;  //名字
    std::vector<comProperty*> propList;  //属性列表
    int flags;  //标志
};

class CompBase
{
    public:
        CompBase(std::string type, std::string id);
        virtual ~CompBase();

        //添加和移除属性组、添加和移除单个属性，以及从 XML 元素加载属性。
        void addPropGroup( propGroup pg, bool list=true );
        void remPropGroup( std::string name );
        void addProperty( std::string group, comProperty* p );
        void removeProperty( std::string prop );
        void loadProperties(const TiXmlElement* el);
        //指向属性组向量的指针。
        std::vector<propGroup>* properties() { return &m_propGroups; } // Circuit needs pointer bcos properties can change (ex: loadHex changes Config)

        //获取枚举类型的 UID 和名称，以及获取枚举索引。
        virtual std::vector<std::string> getEnumUids( std::string )  { return m_enumUids; }
        virtual std::vector<std::string> getEnumNames( std::string ) { return m_enumNames; }
        int getEnumIndex( std::string prop );

        //设置和获取属性的字符串值。
        virtual bool    setPropStr( std::string prop, std::string val );
        virtual std::string getPropStr( std::string prop );

        //转换为字符串函数 
        virtual std::string toString();

        //获取和设置组件的唯一标识符。
        std::string getUid() { return m_id; }
        void setUid( std::string uid ) { m_id = uid; }

        //获取和设置组件的类型。
        std::string itemType()  { return m_type; }
        void setItemType( std::string ) {;}

        //用于检查组件是否被隐藏。
        virtual bool isHidden() { return false;}

    protected:
        std::string m_id;  //元件ID
        std::string m_type;  //元件类型
        std::string m_help;  //帮助信息

        //属性组的向量和属性哈希表。
        //PropDialog* m_propDialog;
        std::vector<propGroup> m_propGroups;
        std::unordered_map<std::string, comProperty*> m_propHash;

        //枚举 UID 和名称的向量。
        std::vector<std::string> m_enumUids;
        std::vector<std::string> m_enumNames;
};
