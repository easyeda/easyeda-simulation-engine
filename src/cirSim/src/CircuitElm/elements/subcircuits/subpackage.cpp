/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "subpackage.h"
#include "packagepin.h"
#include "itemlibrary.h"
//#include "circuitwidget.h"
#include "circuit.h"
#include "node.h"
#include "utils.h"

#include "stringProp.h"
#include "boolProp.h"
#include "intProp.h"

//#define tr(str) simulideTr("SubPackage",str)

std::string SubPackage::m_lastPkg = "";

Component* SubPackage::construct( std::string type, std::string id )
{ return new SubPackage( type, id ); }

LibraryItem* SubPackage::libraryItem()
{
    return new LibraryItem(
        "Package",
        "Other",
        "resistordip.png",
        "Package",
        SubPackage::construct );
}

SubPackage::SubPackage( std::string type, std::string id )
          : Chip( type, id )
{
    m_subcType = Chip::None;
    m_width  = 4;
    m_height = 8;
    //m_area = QRect(0, 0, m_width*8, m_height*8);

    m_changed = false;
    m_fakePin = false;
    m_isLS    = true;
    //m_graphical = true;
    m_boardMode = false;
    m_name = split(m_id,'-').front();

    //m_lsColor = QColor( 210, 210, 255 );
    //m_icColor = QColor( 40, 40, 120 );
    //m_color = m_lsColor;

    //m_boardModeAction = new QAction( tr("Board Mode") );
    //m_boardModeAction->setCheckable( true );
    //QObject::connect( m_boardModeAction, &QAction::triggered, [=](){ boardModeSlot(); } );
    m_boardMode = false;
    
    //setAcceptHoverEvents( true );
    //setZValue(-3 );
    
    m_pkgeFile = "";

    addPropGroup( { "Main", {
new stringProp <SubPackage>("SubcType"    ,"Type"  ,""      , this, &SubPackage::subcTypeStr,&SubPackage::setSubcTypeStr,0,"enum" ),
//new intProp <SubPackage>("Width"       ,"Width" ,"_Cells", this, &SubPackage::width,      &SubPackage::setWidth ,0,"uint" ),
//new intProp <SubPackage>("Height"      ,"Height","_Cells", this, &SubPackage::height,     &SubPackage::setHeight,0,"uint"  ),
new stringProp <SubPackage>("Name"        ,"Name"        ,"", this, &SubPackage::name,       &SubPackage::setName ),
new stringProp <SubPackage>("Package_File","Package File","", this, &SubPackage::package,    &SubPackage::setPackage),
new boolProp<SubPackage>("Logic_Symbol","Logic Symbol","", this, &SubPackage::logicSymbol,&SubPackage::setLogicSymbol ),
    }, 0} );
}
SubPackage::~SubPackage()
{
    //delete m_boardModeAction;
}

void SubPackage::setSubcTypeStr( std::string s )
{
    int index = getEnumIndex( removeSubstring(s,"subc") );
    subcType_t type = (subcType_t)index;
    if( m_subcType == type ) return;

    if( type >= Board )
    {
        if( Circuit::self()->getBoard() ) // Only one board Package can be in the circuit
        {
            std::cerr << "SubPackage::setSubcTypeStr: ERROR: Only one Board allowed";
            return;
        }
        Circuit::self()->setBoard( this );
    }
    else if( Circuit::self()->getBoard() == this ) 
        Circuit::self()->setBoard( NULL );

    m_subcType = type;
}
void SubPackage::compSelected( Component* comp )
{
    if( comp) comp->setMainComp( !comp->isMainComp() );
}

void SubPackage::boardModeSlot()
{
    m_boardMode = true;                          /*m_boardModeAction->isChecked();-----------------------------暂时不清楚这个的含义先弄个true*/
    //setBoardMode( m_boardMode );
    Component::m_boardMode = m_boardMode;
    //Circuit::self()->update();
}

void SubPackage::remove()
{
    if( m_changed )
    {
        std::string input;
        std::cout << "\nPackage has been modified.\n"
            "Do you want to save your changes? (yes/no)\n";
        std::cin >> input;

        if (input == "yes" || input == "y") 
        {
            slotSave();
        }

    }
    Circuit::self()->compRemoved( true );
}


Pin* SubPackage::addPin( std::string id, std::string type, std::string label, int pos)
{
    PackagePin* pin = new PackagePin( m_id+"-"+id, pos-1, this ); // pos in package starts at 1

    pin->setInverted( type == "inverted" || type == "inv" );
    //pin->setFlag( QGraphicsItem::ItemStacksBehindParent, false );

    m_pkgePins.push_back( pin );
    return pin;
}

void SubPackage::editFinished( int )
{
    /// FIXME UNDOREDO: if( m_changed ) Circuit::self()->saveState();
}

void SubPackage::deleteEventPin()
{
    if( !m_eventPin ) return;
    m_changed = true;

    auto it = std::remove(m_pkgePins.begin(), m_pkgePins.end(), m_eventPin);
    m_pkgePins.erase(it, m_pkgePins.end());
    //m_signalPin.removeOne( m_eventPin ); // ToDelete
    delete m_eventPin;
    m_eventPin = NULL;
    
    //Circuit::self()->update();
}

void SubPackage::setPinId( std::string id )
{
    m_eventPin->setPinId( replaceString(id,"-","") );
    m_changed = true;
}

void SubPackage::setPinName( std::string name )
{
    //m_eventPin->setLabelText( name );
    m_changed = true;
}

void SubPackage::invertPin( bool invert )
{
    m_eventPin->setInverted( invert );
    //Circuit::self()->update();
    m_changed = true;
}

void SubPackage::unusePin( bool unuse )
{
    m_eventPin->setUnused( unuse );
    //Circuit::self()->update();
    m_changed = true;
}

std::string SubPackage::package()
{
    return m_pkgeFile;
    //Circuit::self()->update();
}

void SubPackage::setPackage( std::string package )
{
    m_pkgeFile = package;

    m_pkgePins.clear();
    for( Pin* pin : m_pin ) 
        deletePin( pin );

    Chip::initChip();

    // 使用 insert 将 m_unusedPins 的内容添加到 m_pkgePins 中
    m_pkgePins.insert(m_pkgePins.end(), m_unusedPins.begin(), m_unusedPins.end());

    //m_name = QFileInfo( m_pkgeFile ).baseName().remove(".package").remove("_LS");
    //m_label.setPlainText( m_name );
    
    setLogicSymbol( m_isLS );
    //Circuit::self()->update();
    m_changed = false;
}

void SubPackage::setLogicSymbol( bool ls )
{
    if( ls == m_isLS ) return;
    m_isLS = ls;
}

void SubPackage::slotSave()
{
    std::string pkgeFile = std::filesystem::absolute(Circuit::self()->getFilePath() + "/" + m_pkgeFile).string();

    std::string dir = pkgeFile;
    std::string fileName;
    std::cout << "Save Package: ";
    std::getline(std::cin, fileName);

    if (fileName.empty())
        return;

    savePackage(fileName);
}

void SubPackage::loadPackage()
{
    std::string dir;

    if (m_lastPkg.empty())
    {
        dir = std::filesystem::absolute(Circuit::self()->getFilePath()).string() + "/" + m_pkgeFile;
    }
    else
    {
        dir = std::filesystem::path(m_lastPkg).parent_path().string();
    }
    std::string fileName;
    std::cout << "Load Package File: ";
    std::getline(std::cin, fileName);

    if( fileName.empty() ) return; // User cancels loading

    Circuit::self()->saveCompChange( m_id, "Package_File", fileName );
    setPackage( fileName );

    std::filesystem::path pdir = std::filesystem::absolute(Circuit::self()->getFilePath()).parent_path();
    m_pkgeFile = std::filesystem::relative(fileName, pdir).string();
    m_lastPkg = fileName;
}

void SubPackage::savePackage( std::string fileName )
{
    std::string outputFileName = fileName;
    if (outputFileName.substr(outputFileName.find_last_of(".") + 1) != "package") 
    {
        outputFileName.append(".package");
    }
    std::ofstream file(outputFileName);

    if(!file.is_open())
    {
        std::cerr << "Cannot write file " << outputFileName << std::endl;
        return;
    }

    // Write the XML content
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "");
    doc.LinkEndChild(decl);

    TiXmlElement* packageElement = new TiXmlElement("packageB");
    packageElement->SetAttribute("name", m_name.c_str());
    packageElement->SetAttribute("width", std::to_string(m_width).c_str());
    packageElement->SetAttribute("height", std::to_string(m_height).c_str());
    //packageElement->SetAttribute("background", m_background);
    packageElement->SetAttribute("type", subcTypeStr().c_str());
    
    int pP = 1;
    for (Pin* pin : m_pkgePins) 
    {
        TiXmlElement* pinElement = pinEntry(pin);
        packageElement->LinkEndChild(pinElement);
        pP++;
    }
    doc.LinkEndChild(packageElement);

    doc.SaveFile(outputFileName.c_str());
    file.close();

    // Simulate cursor change (this part is platform-specific and may need adjustments)
    // std::cout << "Simulating cursor change..." << std::endl;

    // Update internal state
    m_pkgeFile = outputFileName;
    m_lastPkg = outputFileName;
    m_changed = false;
}

TiXmlElement* SubPackage::pinEntry( Pin* pin )
{
    // 创建一个新的 TiXmlElement 对象
    TiXmlElement* pinElement = new TiXmlElement("pin");

    // 设置 id 属性
    std::string tmp = "id=\"";
    tmp += pin->pinId().substr(pin->pinId().find_last_of('-') + 1);
    std::string id = replaceString(tmp, " ", "") + "\"";
    pinElement->SetAttribute("id", id.c_str());

    // 设置 label 属性
    std::string labelText = pin->getLabelText();
    labelText = replaceString(labelText, "<", "&#x3C;");
    labelText = replaceString(labelText, "=", "&#x3D;");
    labelText = replaceString(labelText, ">", "&#x3E;");
    pinElement->SetAttribute("label", labelText.c_str());

    // 设置 type 属性
    std::string type;
    if (pin->inverted()) type = "inv";
    if (pin->unused()) type = "nc";
    pinElement->SetAttribute("type", type.c_str());

    // 返回 TiXmlElement 对象
    return pinElement;
}

std::string SubPackage::adjustSize( std::string str, int size )
{
    while( str.length() < size ) str.append(" ");
    return str;
}
