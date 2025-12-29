/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "chip.h"

#include "simulator.h"
#include "connector.h"
#include "circuit.h"
#include "utils.h"
#include "pin.h"

#define tr(str) simulideTr("Chip",str)

Chip::Chip( std::string type, std::string id )
    : Component( type, id )
    , eElement( id )
{
    m_id = id;
    std::vector<std::string> list;
    std::stringstream ss(id);
    std::string token;

    while (std::getline(ss, token, '-'))
    {
        list.push_back(token);
    }

    if( list.size() > 1 ) m_name = list.at( list.size()-2 ); // for example: "atmega328-1" to: "atmega328"

    std::vector <std::string> m_enumUids = {
        "None",
        "Logic",
        "Board",
        "Shield",
        "Module"
    };

    std::vector <std::string> m_enumNames = {
        "None",
        "Logic",
        "Board",
        "Shield",
        "Module"
    };

    m_subcType = None;
    m_isLS = false;
    m_initialized = false;
    m_pkgeFile = "";
    m_backData   = NULL;
    
    //m_lsColor = QColor( 255, 255, 255 );
    //m_icColor = QColor( 50, 50, 70 );

//    QFont f;
//    f.setFamily("Ubuntu Mono");
//    f.setWeight( 65 );
//#ifdef Q_OS_UNIX
//    f.setLetterSpacing( QFont::PercentageSpacing, 120 );
//#else
//    //f.setLetterSpacing( QFont::AbsoluteSpacing, -1 );
//    f.setWeight( 100 );
//    //f.setStretch( 99 );
//#endif
//    f.setPixelSize(5);

    //m_label.setFont( f );
    //m_label.setDefaultTextColor( QColor( 125, 125, 110 ) );
    //m_label.setAcceptedMouseButtons( 0 );
    //m_label.setRotation( -90 );
    //m_label.setVisible( true );
    
    //setLabelPos( m_area.x(), m_area.y()-20, 0);
    //setShowId( true );
}
Chip::~Chip()
{
    //if( m_backPixmap ) delete m_backPixmap;
}

void Chip::initChip()
{
    m_error = 0;

    std::filesystem::path fileNameAbs = m_pkgeFile;

    if (!std::filesystem::exists(fileNameAbs))  // Check if package file exist, if not try LS or no LS
    {
        if( m_initialized ) { m_error = 1; return; }
        if (m_pkgeFile.ends_with("_LS.package")) 
        {
            m_pkgeFile.replace(m_pkgeFile.find("_LS.package"), 11, ".package");
        }
        else if (m_pkgeFile.ends_with(".package")) 
        {
            m_pkgeFile.replace(m_pkgeFile.find(".package"), 8, "_LS.package");
        }
        else {
            m_error = 1;
            std::cerr << "Chip::initChip: No package files found." << std::endl;
            return;
        }
        fileNameAbs = m_pkgeFile;
    }
    TiXmlDocument domDoc = fileToDomDoc(fileNameAbs.string(), "Chip::initChip");
    TiXmlElement* root = domDoc.RootElement();

    if (std::string(root->Value()) == "packageB")
    {
        m_isLS = m_pkgeFile.ends_with("_LS.package");

        //if( m_isLS ) m_color = m_lsColor;
        //else         m_color = m_icColor;

        root->Attribute("width", &m_width);
        root->Attribute("height", &m_height);
        //m_area = QRect( 0, 0, 8*m_width, 8*m_height );

        for( Pin* pin : m_unusedPins ) if( pin ) deletePin( pin );
        m_unusedPins.clear();
        m_ePin.clear();
        m_pin.clear();

        if (const char* type = root->Attribute("type")) setSubcTypeStr(type);
        //if (const char* background = root->Attribute("background")) setBackground(background);
        //if (m_subcType >= 0) setTransformOriginPoint(0, 0);
        if (const char* name = root->Attribute("name")) 
        {
            std::string nameStr = name;
            std::transform(nameStr.begin(), nameStr.end(), nameStr.begin(), ::tolower);   //将字符全部转换为小写
            if (nameStr != "package") m_name = nameStr;
        }
        initPackage( root );
        setName( m_name );
    }
    else
    {
        std::cerr << "Chip::initChip: Error: Not a valid Package file: " << m_pkgeFile << std::endl;
        m_error = 3;
        return;
    }
    m_initialized = true;
}

void Chip::setName( std::string name )
{
    m_name = name;
    //m_label.setPlainText( m_name );
    //m_label.adjustSize();
    //m_label.setY( m_area.height()/2+m_label.textWidth()/2 );
    //m_label.setX( ( m_area.width()/2-m_label.boundingRect().height()/2 ) );
}

//这里有些属性应该可以删减
void Chip::initPackage(TiXmlElement* root )
{
    int chipPos = 0;
    for (TiXmlElement* element = root->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (std::string(element->Value()) == "pin") {
            const char* typeAttr = element->Attribute("type");
            const char* labelAttr = element->Attribute("label");
            const char* idAttr = element->Attribute("id");

            if (typeAttr && labelAttr && idAttr) {
                std::string type = typeAttr;
                std::string label = labelAttr;
                std::string id = idAttr;
                id.erase(std::remove(id.begin(), id.end(), ' '), id.end()); // Remove spaces

                int xpos = 0, ypos = 0, angle = 0, length = 0, space = 0;
                element->Attribute("xpos", &xpos);
                element->Attribute("ypos", &ypos);
                element->Attribute("angle", &angle);
                element->Attribute("length", &length);
                element->Attribute("space", &space);

                chipPos++;
                addNewPin(id, type, label, chipPos);
            }
        }
    }
}

void Chip::addNewPin( std::string id, std::string type, std::string label, int pos )
{
    if( type == "unused" || type == "nc" )
    {
        Pin* pin = new Pin(m_id+"-"+id, pos-1, this ); // pos in package starts at 1

        //pin->setSpace( space );

        pin->setUnused( true ); // Chip::addPin is only for unused Pins
        if( m_isLS )
        {
            //pin->setVisible( false );
            label = "";
        }
        //pin->setLabelText( label );
        //pin->setLength( length );
        //pin->setFlag( QGraphicsItem::ItemStacksBehindParent, false );

        m_unusedPins.push_back( pin );
    }
    else
    {
        Pin* pin = addPin( id, type, label, pos );
        m_ePin.emplace_back( pin );
        m_pin.emplace_back( pin );
    }
}

void Chip::setLogicSymbol( bool ls )
{
    if( m_initialized && (m_isLS == ls) ) return;

    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    /// Undo/Redo stack for Properties ??
    /// Circuit::self()->addCompState( this, "Logic_Symbol" );
    
    if(  ls && m_pkgeFile.ends_with(".package"))     m_pkgeFile.replace(m_pkgeFile.find(".package"), 8, "_LS.package");
    if( !ls && m_pkgeFile.ends_with("_LS.package"))  m_pkgeFile.replace(m_pkgeFile.find("_LS.package"), 11, ".package");

    m_error = 0;
    Chip::initChip();
    
    //if( m_error == 0 ) Circuit::self()->update();
    /// else               Circuit::self()->unSaveState();
}

//void Chip::setBackground( std::string bck )
//{
//    m_background = bck;
//
//    if( m_backPixmap )
//    {
//        delete m_backPixmap;
//        m_backPixmap = NULL;
//    }
//    if( bck.startsWith("color") )
//    {
//        bck.remove("color").remove("(").remove(")").remove(" ");
//        std::stringList rgb = bck.split(",");
//        if( rgb.size() < 3 ) return;
//
//        m_color = QColor( rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt() );
//    }
//    else if( bck != "" ){
//        QDir    circuitDir = QFileInfo( Circuit::self()->getFilePath() ).absoluteDir();
//        std::string pixmapPath = circuitDir.absoluteFilePath( "data/"+m_name+"/"+bck );
//        if( QFile::exists( pixmapPath ) ) m_backPixmap = new QPixmap( pixmapPath );     // Image in Circuit data/name folder
//        else{
//            pixmapPath = MainWindow::self()->getDataFilePath("images/"+bck );
//            if( QFile::exists( pixmapPath ) ) m_backPixmap = new QPixmap( pixmapPath ); // Image in some data/images folder (Circuit, user or SimulIDE)
//        }
//    }
//    update();
//}

//void Chip::findHelp()
//{
//    std::string helpFile = changeExt( m_dataFile, "txt" );
//    if( QFileInfo::exists( helpFile ) ) m_help = fileToString( helpFile, "Chip::findHelp" );
//    else                                m_help = MainWindow::self()->getHelp( m_name, false );
//}

//TiXmlDocument Chip::fileToDomDoc(const std::string& filePath, const std::string& context)
//{
//    TiXmlDocument doc(filePath.c_str());
//    if (!doc.LoadFile()) 
//    {
//        std::cerr << context << ": Failed to load file " << filePath << std::endl;
//    }
//    return doc;
//}