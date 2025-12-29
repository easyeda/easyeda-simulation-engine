/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "subcircuit.h"
#include "itemlibrary.h"
//#include "mainwindow.h"
#include "componentselector.h"
//#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "tunnel.h"
#include "node.h"
#include "e-node.h"
#include "utils.h"
//#include "mcu.h"
#include "linkable.h"

#include "logicsubc.h"
#include "board.h"
#include "shield.h"
#include "module.h"

#include "boolProp.h"

//#define tr(str) simulideTr("SubCircuit",str)

std::string SubCircuit::m_subcDir = "";

Component* SubCircuit::construct( std::string type, std::string id )
{
    m_error = 0;
    m_subcDir = "";

    std::string name;

    std::vector<std::string> list = split(id,'-');

    if( list.size() > 1 ) name = list.at( list.size()-2 ); // for example: "atmega328-1" to: "atmega328"

    list = split(name, '-');
    if( list.size() > 1 )  // Subcircuit inside Subcircuit: 1_74HC00 to 74HC00
    {
        const std::string& n = list.front();
        char* endptr = nullptr;
        long num = std::strtol(n.c_str(), &endptr, 10);
        bool ok = (*endptr == '\0'); // Check if the entire string was a valid number

        if (ok) 
        {
            name = list[1];
        }
    }
    std::string dataFile = ComponentSelector::self()->getXmlFile( name );

    if( dataFile == "" ) // Component is not in SimulIDE, search in Circuit folder
    {
        m_subcDir = ComponentSelector::self()->getFileDir( name ); // Found in folder (no xml file)
        if( m_subcDir.empty() )                                  // Try to find a "data" folder in Circuit folder
        {
            std::filesystem::path file(Circuit::self()->getFilePath());
            // 获取绝对路径
            std::filesystem::path absoluteFile = std::filesystem::absolute(file);
            // 获取父目录路径
            std::filesystem::path circuitDir = absoluteFile.parent_path();
            std::filesystem::path temp = circuitDir / "data" / name;
            m_subcDir = temp.string();
        }
    }
    else
    {
        TiXmlDocument  domDoc = fileToDomDoc( dataFile, "SubCircuit::construct");
        if (domDoc.Error()) return nullptr; // m_error = 1;

        TiXmlElement* root = domDoc.RootElement();
        if (!root) return nullptr;
        TiXmlNode* rNode = root->FirstChild();
        bool found = false;

        while(rNode)
        {
            TiXmlElement* itemSet = rNode->ToElement();
            if (!itemSet) 
            {
                rNode = rNode->NextSibling();
                continue;
            }
            TiXmlNode* node = itemSet->FirstChild();
            std::string folder = "";
            const char* folderAttr = itemSet->Attribute("folder");
            if (folderAttr) 
                folder = folderAttr;

            while(node)
            {
                TiXmlElement* element = node->ToElement();
                if (!element) 
                {
                    node = node->NextSibling();
                    continue;
                }
                const char* nameAttr = element->Attribute("name");
                if(nameAttr && name == nameAttr)
                {
                    if (element->Attribute("folder")) 
                        folder = element->Attribute("folder");
                    m_subcDir = Circuit::self()->getDataFilePath( folder+"/"+name );
                    found = true;
                }
                if( found ) 
                    break;
                node = node->NextSibling();
            }
            if( found ) 
                break;
            rNode = rNode->NextSibling();
        }
    }
    if( m_subcDir.empty() )
    {
        std::cerr << "SubCircuit::construct: No Circuit files found for " << name << std::endl;
        return NULL;
    }
    std::string pkgeFile  = m_subcDir+"/"+name+".package";
    std::string pkgFileLS = m_subcDir+"/"+name+"_LS.package";
    std::string subcFile  = m_subcDir+"/"+name+".sim1";

    //文件是否存在
    bool dip = std::filesystem::exists( pkgeFile );
    bool ls  = std::filesystem::exists( pkgFileLS );

    if( !dip )
    {        // Check if package file exist, if not try LS
        if( !ls )
        {
            std::cerr << "SubCircuit::construct: No package files found for " << name << std::endl << pkgeFile << std::endl;
            return NULL;
        }
        pkgeFile = pkgFileLS;
    }
    TiXmlDocument    domDoc1 = fileToDomDoc( pkgeFile, "SubCircuit::construct" );
    TiXmlElement*   root1  = domDoc1.RootElement();

    std::string subcTyp = "None";
    if(root1->Attribute("type")) 
        subcTyp = removeSubstring(root1->Attribute("type"), "subc");

    SubCircuit* subcircuit = NULL;
    if     ( subcTyp == "Logic"  ) 
        subcircuit = new LogicSubc( type, id );
    else if( subcTyp == "Board"  ) 
        subcircuit = new BoardSubc( type, id );
    else if( subcTyp == "Shield" ) 
        subcircuit = new ShieldSubc( type, id );
    else if( subcTyp == "Module" ) 
        subcircuit = new ModuleSubc( type, id );
    else                           
        subcircuit = new SubCircuit( type, id );

    if( m_error != 0 )
    {
        subcircuit->remove();
        m_error = 0;
        return NULL;
    }else{
        Circuit::self()->m_createSubc = true;
        subcircuit->m_pkgeFile = pkgeFile;
        subcircuit->initChip();
        if( m_error == 0 ) subcircuit->loadSubCircuit( subcFile );

        if( dip && ls ) // If no both files exist, this prop. is not needed
        subcircuit->addPropGroup( { "Main", {
            new boolProp<SubCircuit>( "Logic_Symbol", "Logic Symbol","", subcircuit, &SubCircuit::logicSymbol, &SubCircuit::setLogicSymbol ),
            },groupNoCopy} 
        );
        Circuit::self()->m_createSubc = false;
    }
    if( m_error > 0 )
    {
        Circuit::self()->compList()->erase( subcircuit );
        delete subcircuit;
        m_error = 0;
        return NULL;
    }
    return subcircuit;
}

LibraryItem* SubCircuit::libraryItem()
{
    return new LibraryItem(
        "Subcircuit",
        "",         // Category Not dispalyed
        "",
        "Subcircuit",
        SubCircuit::construct );
}

SubCircuit::SubCircuit( std::string type, std::string id )
          : Chip( type, id )
{
    //m_lsColor = QColor( 235, 240, 255 );
    //m_icColor = QColor( 20, 30, 60 );
    //m_mainComponent = NULL;
}
SubCircuit::~SubCircuit(){}

void SubCircuit::loadSubCircuit( std::string file )
{
    m_dataFile = file;
    std::string doc = fileToString( file, "SubCircuit::loadSubCircuit" );

    std::string oldFilePath = Circuit::self()->getFilePath();
    Circuit::self()->setFilePath( file );             // Path to find subcircuits/Scripted in our data folder

    //std::vector<std::string> graphProps;
    //for( propGroup pg : m_propGroups ) // Create list of "Graphical" poperties (We don't need them)
    //{
    //    if( (pg.name != "CompGraphic") ) continue;
    //    for( comProperty* prop : pg.propList ) graphProps.push_back( prop->name() );
    //    break;
    //}

    std::string numId = m_id;
    numId = split(numId ,'-').back();
    Circuit* circ = Circuit::self();

    std::vector<Linkable*>  linkList;   // Linked  Component list

    std::vector<std::string> docLines = split(doc,'\n');
    for( std::string line : docLines )
    {
        if(line.find("<item") == 0)
        {
            std::string uid, newUid, type, label;

            std::string name;  //属性名
            std::istringstream iss(line);
            std::string prop;  //存储属性值
            std::unordered_map<std::string, std::string> properties;  //存储其他属性。
            //以双引号为分隔符
            while (std::getline(iss, prop, '\"'))
            {
                //属性片段的最后一个字符是等号 =，则将其前面的部分保存为属性名 name。
                if (prop.size() > 1 && prop.back() == '=')
                {
                    name = prop.substr(0, prop.size() - 1);

                    // 如果截取的 name 的首个字符是空格字符，就将其去掉
                    if (!name.empty() && name[0] == ' ')
                    {
                        name.erase(0, 1); // 移除第一个字符（空格）
                    }

                    if (name == "<item itemtype")
                    {
                        name = "itemtype";
                    }
                    continue;
                }
                //大于号直接忽略
                else if (prop.back() == '>')
                {
                    continue;
                }
                //大于号直接忽略
                else if (prop.back() == '\r')
                {
                    continue;
                }
                else
                {
                    //将属性值的实例注入
                    if (name == "itemtype")
                        type = prop;
                    else if (name == "uid")
                        uid = prop;
                    else if (name == "CircId")
                        uid = prop;
                    else if (name == "objectName")
                        uid = prop;
                    else if (name == "label")
                        label = prop;
                    else if (name == "id")
                        label = prop;
                    else
                        properties[name] = prop;
                }
            }
            newUid = numId+"_"+uid;

            if( type == "Connector" )
            {
                Pin* startPin = NULL;
                Pin* endPin = NULL;
                std::string startpinid, endpinid, enodeId;
                std::vector<std::string> pointList;

                std::string name = "";
                std::string val = "";
                for(const auto& prop : properties )
                {
                    //if( name.empty() ) 
                    //{ 
                    //    name = prop.first; 
                    //    continue; 
                    //}
                    name = prop.first;
                    val = prop.second;

                    if     ( name == "startpinid") 
                        startpinid = numId+"_"+val;
                    else if( name == "endpinid"  ) 
                        endpinid = numId+"_"+val;
                    else if( name == "enodeid"   ) 
                        enodeId    = val;
                    name = "";
                }
                startpinid = replaceString(startpinid,"Pin-", "Pin_"); // Old TODELETE
                endpinid = replaceString(endpinid,"Pin-", "Pin_"); // Old TODELETE

                startPin = circ->m_LdPinMap[startpinid];
                endPin   = circ->m_LdPinMap[endpinid];

                if( startPin && endPin )    // Create Connector
                {
                    startPin->setConPin( endPin );
                    endPin->setConPin( startPin );
                }
                else // Start or End pin not found
                {
                    if (!startPin) std::cerr << "\n   ERROR!!  SubCircuit::loadSubCircuit: " << m_name << m_id + " null startPin in " << type << uid << startpinid;
                    if (!endPin)   std::cerr << "\n   ERROR!!  SubCircuit::loadSubCircuit: " << m_name << m_id + " null endPin in " << type << uid << endpinid;
            }   }
            else if( type == "Package" ) { ; }
            else{
                Component* comp = NULL;

                if( type == "Node" ) comp = new Node( type, newUid );
                else                 comp = circ->createItem( type, newUid, false );

                if (comp)
                {
                    // 假设 Component 类有 properties 方法返回属性列表
                    auto groups = comp->properties();                   // 按正确的顺序设置属性  获取组件的属性列表。
                    for (const auto& group : *groups)                   //为每个属性组设置属性值
                    {
                        auto propList = group.propList;                 //获取属性组中的属性列表。
                        if (propList.empty()) continue;
                        for (auto* prop : propList)
                        {
                            std::string qpn = prop->name();                          //获取属性的名称
                            if (properties.find(qpn) == properties.end()) continue;  //如果属性名称不在 properties 映射中，跳过当前循环。
                            std::string temp = properties[qpn];
                            prop->setValStr(properties[qpn]);                        // 设置属性的值
                            properties.erase(qpn);                                   //从 properties 映射中移除已设置的属性。
                        }
                    }
                    //comp->setParentItem( this );

                    //if( m_subcType >= Board && comp->isGraphical() )
                    //{
                    //    QPointF pos = comp->boardPos();
                    //    if( pos == QPointF(-1e6,-1e6 ) ) // Don't show Components not placed
                    //    {
                    //        pos = QPointF( 0, 0 );
                    //        comp->setVisible( false );
                    //    }
                    //    comp->moveTo( pos );
                    //    comp->setRotation( comp->boardRot() );
                    //    comp->setHflip( comp->boardHflip() );
                    //    comp->setVflip( comp->boardVflip() );
                    //    if( !this->collidesWithItem( comp ) ) // Don't show Components out of Board
                    //    {
                    //        comp->moveTo( QPointF( 0, 0 ) );
                    //        comp->setVisible( false );
                    //    }
                    //    comp->setHidden( true, true, true ); // Boards: hide non graphical
                    //}
                    //else{
                    //    comp->moveTo( QPointF(20, 20) );
                    //    comp->setVisible( false );     // Not Boards: Don't show any component
                    //}

                    //if( comp->itemType() == "MCU" )
                    //{
                    //    comp->removeProperty("Logic_Symbol");
                    //    Mcu* mcu = (Mcu*)comp;
                    //    std::string program = mcu->program();
                    //    if( !program.isEmpty() ) mcu->load( m_subcDir+"/"+program );
                    //}
                    if (comp->isMainComp()) m_mainComponents[uid] = comp; // This component will add it's Context Menu and properties

                    m_compList.insert(comp);

                    if (comp->m_isLinker)
                    {
                        Linkable* l = dynamic_cast<Linkable*>(comp);
                        if (l->hasLinks()) linkList.push_back(l);
                    }

                    if (type == "Tunnel") // Make Tunnel names unique for this subcircuit ----------------------------------------------------------------------------------暂定
                    {
                        Tunnel* tunnel = static_cast<Tunnel*>(comp);
                        tunnel->setTunnelUid(tunnel->name());
                        tunnel->setName(m_id + "-" + tunnel->name());
                        m_subcTunnels.push_back(tunnel);
                    }
                }
                else 
                    std::cerr << "SubCircuit:" << m_name << m_id << " ERROR Creating Component: " << type << uid << label << std::endl;
    }   }   }
    for( Linkable* l : linkList )
        l->createLinks( &m_compList );

    Circuit::self()->setFilePath( oldFilePath ); // Restore original filePath
}

Pin* SubCircuit::addPin( std::string id, std::string type, std::string label, int pos)
{
    if( m_initialized && m_pinTunnels.contains( m_id+"-"+id ) )
    {
        return updatePin( id, type, label,pos);
    }
    else
    {
        //QColor color = Qt::black;
        //if( !m_isLS ) color = QColor( 250, 250, 200 );

        Tunnel* tunnel = new Tunnel( "Tunnel", m_id+"-"+id );
        Circuit::self()->compList()->erase( tunnel );
        m_compList.insert( tunnel );

        std::string pId = m_id+"-"+id;
        //tunnel->setParentItem( this );
        //tunnel->setAcceptedMouseButtons( Qt::NoButton );
        //tunnel->setShowId( false );
        tunnel->setTunnelUid( id );
        tunnel->setName( pId ); // Make tunel name unique for this component
        //tunnel->setPos( xpos, ypos );
        tunnel->setPacked( true );
        m_pinTunnels.insert(std::make_pair(pId, tunnel));

        Pin* pin = tunnel->getPin();
        /// pin->setObjectName( pId );
        pin->setId( pId );
        pin->setInverted( type == "inverted" || type == "inv" );
        addSignalPin( pin );
        /// connect( this, &SubCircuit::moved, pin, &Pin::isMoved, Qt::UniqueConnection );

        //tunnel->setRotated( angle >= 180 );      // Our Pins at left side
        //if     ( angle == 180) tunnel->setRotation( 0 );
        //else if( angle == 90 ) tunnel->setRotation(-90 ); // QGraphicsItem 0º i at right side
        //else                   tunnel->setRotation( angle );

        //pin->setLength( length );
        //pin->setSpace( space );
        //pin->setLabelColor( color );
        //pin->setLabelText( label );
        //pin->setFlag( QGraphicsItem::ItemStacksBehindParent, false );
        return pin;
    }
}

Pin* SubCircuit::updatePin( std::string id, std::string type, std::string label, int pos)
{
    Pin* pin = NULL;
    Tunnel* tunnel = m_pinTunnels.at( m_id+"-"+id );
    if( !tunnel )
    {
       std::cerr <<"SubCircuit::updatePin Pin Not Found:"<<id<<type<<label;
        return NULL;
    }
    //tunnel->setPos( xpos, ypos );
    //tunnel->setRotated( angle >= 180 );      // Our Pins at left side

    //if     ( angle == 180) tunnel->setRotation( 0 );
    //else if( angle == 90 ) tunnel->setRotation( -90 ); // QGraphicsItem 0º i at right side
    //else                   tunnel->setRotation( angle );

    pin  = tunnel->getPin();
    type = toLower(type);

    //if( m_isLS ) pin->setLabelColor( QColor( 0, 0, 0 ) );
    //else         pin->setLabelColor( QColor( 250, 250, 200 ) );

    if( type == "unused" || type == "nc" )
    {
        pin->setUnused( true );
        //if( m_isLS )
        //{
        //    pin->setVisible( false );
        //    pin->setLabelText( "" );
        //}
    }
    pin->setInverted( type == "inverted" || type == "inv" );
    //pin->setLength( length );
    //pin->setSpace( space );
    //pin->setLabelText( label );
    //pin->setVisible( true );
    //pin->setFlag( QGraphicsItem::ItemStacksBehindParent, (length<8) );
    //pin->isMoved();

    return pin;
}



void SubCircuit::setLogicSymbol( bool ls )
{
    if( !m_initialized ) return;
    if( m_isLS == ls ) return;

    Chip::setLogicSymbol( ls );
    if( m_isLS != ls ) return; // Change not done

    if( m_isLS )               // Don't show unused Pins in LS
    {
        for (const auto& pair : m_pinTunnels) 
        {
            std::string tNam = pair.first;  // 获取键名
            Tunnel* tunnel = pair.second;   // 获取对应的值，假设为 Tunnel* 类型

            Pin* pin = tunnel->getPin();
            if (pin && pin->unused()) 
            {
                // pin->setVisible(false);
                // pin->setLabelText("");
            }
        }
    }
    //if( m_subcType >= Board ) // Don't show graphical components in LS if Board
    //{
    //    for( Component* c : m_compList )
    //        if( c->isGraphical() ) c->setVisible( !m_isLS );
    //}
}

std::vector<Tunnel*> SubCircuit::getPinTunnels()
{
    std::vector<Tunnel*> tunnels;
    for (const auto& pair : m_pinTunnels) 
    {
        tunnels.push_back(pair.second);
    }
    return tunnels;
}

Component* SubCircuit::getMainComp( std::string name )
{
    if (name.empty() && !m_mainComponents.empty()) 
    {
        auto it = m_mainComponents.begin(); // 获取 unordered_map 的迭代器
        return it->second; // 返回第一个键对应的值，假设 Component* 类型
    }
    return m_mainComponents.at(name);
}

void SubCircuit::remove()
{
    for( Component* comp : m_compList ) comp->remove();
    m_pin.clear();
    Component::remove();
}

//void SubCircuit::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
//{
//    event->accept();
//
//    for( Component* mainComp : m_mainComponents.values() )
//    {
//        std::string compType = mainComp->getUid();
//        int pos  = compType.indexOf("_")+1;
//        int len  = compType.lastIndexOf("-")-pos;
//        compType = compType.mid( pos, len );
//
//        QMenu* submenu = menu->addMenu( QIcon(":/subc.png"), compType );
//
//        mainComp->contextMenu( NULL, submenu );
//    }
//    menu->addSeparator();
//    Component::contextMenu( event, menu );
//}

std::string SubCircuit::toString()
{
    std::string item = CompBase::toString();
    std::string end = " />\n";

    if( !m_mainComponents.empty() )
    {
        item.erase(item.find(end), end.length());
        item += ">";

        for(auto& pair : m_mainComponents )
        {
            std::string uid = pair.first;
            Component* mainComponent = pair.second;

            item += "\n<mainCompProps MainCompId=\""+uid+"\" ";
            for( propGroup pg : *mainComponent->properties() )
            {
                if( pg.flags & groupNoCopy ) 
                    continue;

                for( comProperty* prop : pg.propList )
                {
                    std::string val = prop->toString();
                    if( val.empty() ) continue;
                    item += prop->name() + "=\""+val+"\" ";
            }   }
            item += "/>\n";
        }
        item += "</item>\n";
    }
    return item;
}
