/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "component.h"

class LibraryItem;

//元件库
class ItemLibrary
{
    public:
        ItemLibrary();
        ~ItemLibrary();

 static ItemLibrary* self() { return m_pSelf; }

        std::vector<LibraryItem*> items() { return m_items; }  //外部调用链表的函数接口，链表存放着所有Library对象

        LibraryItem* itemByName( const std::string name );   //寻找特定对象

        void addItem( LibraryItem* item );   //添加新对象到链表中
        
        void loadItems();  //创建对象

    protected:
 static ItemLibrary* m_pSelf;

        std::vector<LibraryItem*> m_items;
};


//表示单个元件的类，包含元件的元数据和创建函数，是用来返回一个component类的函数指针，负责创建实例
class LibraryItem
{
    public:
        LibraryItem( std::string name, std::string category, std::string iconName,
                     std::string type, createItemPtr createItem );
        
        ~LibraryItem();

        std::string name()     { return m_name; }
        std::string category() { return m_category; }
        std::string iconfile() { return m_iconfile; }
        std::string type()     { return m_type; }

        createItemPtr createItemFnPtr() { return createItem; }  //外部调用私有变量：函数指针 的途径

    private:
        std::string m_name;  
        std::string m_category;
        std::string m_iconfile;
        std::string m_type;

        createItemPtr createItem;
};

