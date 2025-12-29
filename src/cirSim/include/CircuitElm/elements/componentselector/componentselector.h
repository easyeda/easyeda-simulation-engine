/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "itemlibrary.h"


namespace fs = std::filesystem;

class ComponentSelector
{
    public:
        ComponentSelector(  );
        ~ComponentSelector();

 static ComponentSelector* self() { return m_pSelf; }

        //加载指定目录下的组件集。
        void LoadCompSetAt(const fs::path& compSetDir);

        //根据组件名称获取其对应的 XML 文件路径。
        std::string getXmlFile( std::string compName ) 
        {
            auto it = m_xmlFileList.find(compName);
            if (it != m_xmlFileList.end()) 
            {
                return it->second;
            }
            else 
            {
                throw std::out_of_range("Key not found in map");
            }
        }
        //根据组件名称获取其所在的文件夹路径。
        std::string getFileDir( std::string compName ) 
        { 
            auto it = m_dirFileList.find(compName);
            if (it != m_dirFileList.end())
            {
                return it->second;
            }
            else
            {
                throw std::out_of_range("Key not found in map");
            }
        }

        //根据给定的过滤条件搜索组件。
        void search( std::string filter );

        std::unordered_map<std::string, std::string> getShortCuts() { return m_shortCuts; }


    private:
 static ComponentSelector* m_pSelf;

        //加载指定的 XML 文件
        void loadXml( const std::string &setFile );

        std::vector<std::string> m_components;                      //组件列表
        std::unordered_map<std::string, std::string> m_categories;
        std::unordered_map<std::string, std::string> m_catTr;

        std::unordered_map<std::string, std::string> m_xmlFileList;//xml文件路径列表
        std::unordered_map<std::string, std::string> m_dirFileList;//文件目录列表
        std::unordered_map<std::string, std::string> m_shortCuts;

        //manCompDialog m_mcDialog;

        //ItemLibrary m_itemLibrary;
};

