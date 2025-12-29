/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "circuit.h"
#include "componentselector.h"
//#include "mainwindow.h"

ComponentSelector* ComponentSelector::m_pSelf = NULL;

ComponentSelector::ComponentSelector( )
{
    m_pSelf = this;

}
ComponentSelector::~ComponentSelector(){}



void ComponentSelector::LoadCompSetAt(const fs::path& compSetDir)
{
    if (!fs::exists(compSetDir) || !fs::is_directory(compSetDir))
    {
        std::cerr << "Directory does not exist: " << compSetDir << std::endl;
        return;
    }

    std::vector<fs::path> xmlList;
    for (const auto& entry : fs::directory_iterator(compSetDir))
    {
        if (entry.path().extension() == ".xml")
        {
            xmlList.push_back(entry.path());
        }
    }

    //if( xmlList.isEmpty() ) return;                  // No comp sets to load

    std::cout << "\nLoading Component sets at:\n" << compSetDir << "\n";

    for (const auto& compSetPath : xmlList)
    {
        loadXml(compSetPath.string());
    }

    fs::path testDir = compSetDir / "test";
    if (fs::exists(testDir) && fs::is_directory(testDir))
    {
        std::vector<fs::path> dirList;
        for (const auto& entry : fs::directory_iterator(testDir))
        {
            if (entry.is_directory())
            {
                dirList.push_back(entry.path());
            }
        }
    }
    std::cout << "\n";
}

void ComponentSelector::loadXml(const std::string& setFile )
{
    //std::ifstream file(setFile);
    //if (!file.is_open())
    //{
    //    std::cerr << "Cannot read file " << setFile << "\n";
    //    return;
    //}
    TiXmlDocument doc(setFile.c_str());
    if (!doc.LoadFile()) 
    {
        std::cerr << "Cannot read file " << setFile << ": " << doc.ErrorDesc() << std::endl;
        return;
    }
    TiXmlElement* root = doc.RootElement();
    if (!root || std::string(root->Value()) != "itemlib")
    {
        std::cerr << "Error parsing file (itemlib): " << setFile << "\n";
        return;
    }

    for (TiXmlElement* itemSet = root->FirstChildElement("itemset"); itemSet; itemSet = itemSet->NextSiblingElement("itemset"))
    {
        std::string icon = "";
        if (itemSet->Attribute("icon"))
        {
            icon = itemSet->Attribute("icon");
            if (icon.rfind(":/", 0) != 0)
                icon = Circuit::self()->getDataFilePath("images/" + icon);
        }

        std::string category = itemSet->Attribute("category");
        std::string type = itemSet->Attribute("type") ? itemSet->Attribute("type") : "";
        std::string folder = itemSet->Attribute("folder") ? itemSet->Attribute("folder") : "";

        std::string parent = "";
        std::string catItem;

        size_t pos = 0;
        while ((pos = category.find('/')) != std::string::npos)
        {
            parent = category.substr(0, pos);
            category.erase(0, pos + 1);
            //catItem = getCategory(parent);
            if (catItem.empty())
            {
                std::string catTr = parent; // Translation may be applied here if needed
                //addCategory(catTr, parent, parent, icon);
            }
        }

        //std::string type = itemSet->Attribute("type");

        for (TiXmlElement* item = itemSet->FirstChildElement("item"); item; item = item->NextSiblingElement("item"))
        {
            icon = "";
            if (item->Attribute("icon"))
            {
                icon = item->Attribute("icon");
                if (icon.rfind(":/", 0) != 0)
                    icon = Circuit::self()->getDataFilePath("images/" + icon);
            }

            std::string name = item->Attribute("name");
            if (std::find(m_components.begin(), m_components.end(), name) == m_components.end())
            {
                m_components.push_back(name);
                m_xmlFileList[name] = setFile;   // Save xml File used to create this item
                if (item->Attribute("info"))
                    name += "???" + std::string(item->Attribute("info"));

 /*               addItem(name, catItem, icon, type);*/
            }
        }
    }

    std::string compSetName = std::filesystem::path(setFile).filename().string();
    std::cout << "        Loaded Component set:           " << compSetName << "\n";
}
