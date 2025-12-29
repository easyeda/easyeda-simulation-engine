/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by easyEDAJLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <stdexcept>

#include "circuit.h"
#include "simulator.h"
#include "itemlibrary.h"

#include "comProperty.h"
//#include "connectorline.h"
#include "componentselector.h"
#include "node.h"
#include "utils.h"
#include "subcircuit.h"
#include "subpackage.h"
//#include "mcu.h"
#include "simulator.h"
#include "e-node.h"
#include "shield.h"
#include "linkable.h"
#include "analogclock.h"



Circuit* Circuit::m_pSelf = NULL;
std::string Circuit::m_userDir = "data";
std::string Circuit::m_proDir = " ";

Circuit::Circuit(std::string netlist)
{
    // std::cout << "net: " << netlist << std::endl;
    m_simulator = new Simulator();
    Factor = new ItemLibrary();
    m_components = new ComponentSelector();
    m_filePath = "";
    m_pSelf = this;

    m_loading    = false;
    m_conStarted = false;
    m_createSubc = false;
    m_redo = false;

    m_board = NULL;
    m_newConnector = NULL;
    m_seqNumber = 0;
    //m_conNumber = " ";

    setUserFileDir();
    setProFileDir();
    ComponentSelector::self()->LoadCompSetAt(m_userDir);
    loadCircuit(netlist);
}

Circuit::~Circuit()
{
    delete m_simulator;
    delete Factor;
    delete m_components;
    //m_bckpTimer.stop();
    m_undoStack.clear();
    //清除引脚
    for (auto& pair : m_pinMap)
    {
        delete pair.second;
    }
    for(auto& pair: m_compMap)
    {
        delete pair.second;
    }
    clearCircuit();
    //QFile file( m_backupPath );
    //if( !file.exists() ) return;
    //QFile::remove( m_backupPath ); // Remove backup file
}

Component* Circuit::getCompById( std::string id )
{
    for( Component* comp : m_compList ) if( comp->getUid() == id ) return comp;
    return NULL;
}

std::string Circuit::getSeqNumber( std::string& name )
{
    std::istringstream iss(name);
    std::string word;
    std::vector<std::string> words;

    // Split the string by '-'
    while (std::getline(iss, word, '-')) 
    {
        words.push_back(word);
    }

    // Start at the second word, the first must be the name
    for (size_t i = 1; i < words.size(); ++i) {
        std::string& word = words[i];
        char* end;
        //long num = std::strtol(word.c_str(), &end, 10); // Convert to long

        // If the conversion was successful and the entire string was consumed
        if (end == word.c_str() + word.size()) {
            return word; // This is the old seqNumber
        }
    }
    return "";
}

std::string Circuit::replaceId( std::string& pinName )
{
    std::istringstream iss(pinName);
    std::string word;
    std::vector<std::string> words;
    while (std::getline(iss, word, '-'))
    {
        words.push_back(word);
    }
    // Start at the second word, the first must be the name
    for (size_t i = 1; i < words.size(); ++i) 
    {
        char* end;
        std::strtol(words[i].c_str(), &end, 10); // Attempt to convert to int

        // If the conversion was successful (the entire string was an integer)
        if (end == words[i].c_str() + words[i].size()) {
            auto it = m_idMap.find(words[i]);
            if (it != m_idMap.end()) 
            {
                words[i] = it->second; // Replace with the value from idMap
            }
            break; // Stop after the first replacement
        }
    }

    // Join the words back into a string with '-' as the delimiter
    std::ostringstream oss;
    for (size_t i = 0; i < words.size(); ++i) 
    {
        if (i > 0) oss << '-'; // Don't prepend '-' to the first word
        oss << words[i];
    }
    return oss.str();
}

bool Circuit::upDateCmp(const std::vector<std::string>& message)
{
    // 检查message是否有足够元素
    if (message.size() < 1) return false;
    for (Component* comp : m_compList) 
    {
        if (comp->getUid() != message[0]) continue;
        // 处理开关等状态器件
        if (message[0].find("Push") != std::string::npos) {
            std::lock_guard<std::mutex> lock(m_mutex);
            comp->updateState(message.size() > 1 ? message[1] : "");
            return true;
        } else if(message[0].find("Switch") != std::string::npos){
            std::lock_guard<std::mutex> lock(m_mutex);
            comp->updateState();
            return true;
        }
        // 修改属性值
        auto groups = comp->properties();
        if (!groups || message.size() < 3) return false;
        for (const auto& group : *groups) {
            auto propList = group.propList;
            if (propList.empty()) continue;
            
            for (auto* prop : propList) {
                if (prop->name() != message[1]) continue;
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    comp->ctl = true;
                    prop->setValStr(message[2]);
                }
                return true;
            }
        }
    }
    return false;
}

std::string Circuit::getExecutablePath()
{
    char buffer[1024];
    std::string path;

#ifdef _WIN32
    GetModuleFileName(NULL, buffer, sizeof(buffer));
    path = std::string(buffer);
#elif __APPLE__
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        path = std::string(buffer);
    }
    else {
        char* largerBuffer = new char[size];
        if (_NSGetExecutablePath(largerBuffer, &size) == 0) {
            path = std::string(largerBuffer);
        }
        delete[] largerBuffer;
    }
#else
    ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if (count != -1) {
        path = std::string(buffer, count);
    }
#endif

    return path;
}

std::string Circuit::getExecutableDir()
{
    std::filesystem::path exePath = getExecutablePath();
    return exePath.parent_path().string();
}

std::string Circuit::getDataFilePath(const std::string& file)
{
    std::string path;

    // 检查 Circuit 是否存在，并获取其文件路径
    if (Circuit::self()) 
    {
        std::string circPath = Circuit::self()->getFilePath();
        if (!circPath.empty()) 
        {
            std::filesystem::path circuitDir = std::filesystem::path(circPath).parent_path();
            path = (circuitDir / ("data/" + file)).string();
            if (std::filesystem::exists(path)) return path; // 文件在 Circuit 数据文件夹中
        }
    }

    // 检查用户数据文件夹
    path = getUserFilePath(file);
    if (path.empty() || !std::filesystem::exists(path)) 
    {
        // 检查 SimulIDE 数据文件夹
        path = getProFilePath("data/" + file);
    }

    return path;
}

void Circuit::setProFileDir()
{
    // std::cout << "set user profiledir" << std::endl;
    m_proDir = getExecutableDir();
}

std::string Circuit::getProFilePath(const std::string& file)
{
    if (m_proDir.empty())
    {
        std::string str(" ");
        return str;
    }
    std::filesystem::path userDirPath(m_proDir);
    std::filesystem::path filePath = userDirPath / file;
    std::string absolutePath = std::filesystem::absolute(filePath).string();
    return absolutePath;
}

void Circuit::setUserFileDir()
{
    // std::cout << "set user dir" << std::endl;
    std::string exeDir = getExecutableDir();
    std::filesystem::path tempFilePath = std::filesystem::path(exeDir) / "data";
    m_userDir = tempFilePath.string();
}

std::string Circuit::getUserFilePath(const std::string& file)
{
    if (m_userDir.empty())
    {
        std::string str(" ");
        return str;
    }
    std::filesystem::path userDirPath(m_userDir);
    std::filesystem::path filePath = userDirPath / file;
    std::string absolutePath = std::filesystem::absolute(filePath).string();
    return absolutePath;
}


void Circuit::loadCircuit(const std::string& netlist)
{
    //if( m_conStarted ) return;

    //m_busy = true;
    //m_loading = true;
    // m_filePath = fileName;
    m_error = 0;
    std::string doc;
    if(!netlist.empty()){
        doc = netlist;
    }
    if (!doc.empty()) 
    {
        loadStrDoc(doc);
    }
    else 
    {
        std::cerr << " file is emty !" << std::endl; // Set an error code if the document is empty
    }


    //m_busy = false;
    m_loading = false;

    if (m_error != 0) clearCircuit();
    else 
    {
        //m_graphicView->zoomToFit();
        std::cout << "Circuit Loaded: " << std::endl;
    }
}


void Circuit::loadStrDoc( std::string doc )
{

    std::vector<Linkable*> linkList;   // Linked  Component list   存储已连接的组件列表。
    std::unordered_set<Component*> compList;   // Pasting Component list   存储正在粘贴的组件集合。
    std::unordered_set<Connector*> conList;    // Pasting Connector list   存储正在粘贴的连接器集合。
    std::unordered_set<Node*> nodeList;        // Pasting node list        存储正在粘贴的节点集合。

    m_newComp = NULL;
    Component* lastComp = NULL;
    std::vector<ShieldSubc*> shieldList;   //存储指向 ShieldSubc 类型对象的指针。ShieldSubc 类型可能代表电路中的一个子电路或模块

    int rev = 0;
    m_busy  = true;
    if(!m_redo ) m_LdPinMap.clear();    //如果不是撤销（undo）或重做（redo）操作，则清空引脚映射

    std::istringstream iss(doc);
    std::string line;
    while(std::getline(iss, line))   //遍历文件的每一行进行
    {
        //行为<circuit应该进行的操作
        if (line.find("<circuit") == 0 )  //&& !m_pasting
        {
            line = line.substr(9, line.length() - 11);
            std::istringstream iss(line);
            std::string prop;
            while (std::getline(iss, prop, ' ')) 
            {
                auto pos = prop.find('=');
                if (pos == std::string::npos) continue;

                std::string name = prop.substr(0, pos);
                std::string val = prop.substr(pos + 2, prop.length() - pos - 3); // Remove quotes

                if (name == "stepSpeed") m_simulator->setSpeed(std::stoul(val));
                else if (name == "NLsteps") m_simulator->setMaxNlSteps(std::stoul(val));
                else if (name == "reaStep") AnalogClock::self()->setPeriod( std::stoull(val));
                else if (name == "currentModelEnable") m_simulator->setCurrentModel(val == "true");
                else if (name == "rev")
                {
                    if(val == "")
                    {
                        break;
                    }
                    try 
                    {
                        rev = std::stoi(val);
                        //std::cout << "Converted value: " << rev << std::endl;
                    }
                    catch (const std::invalid_argument& e)  //非有效整数
                    {
                        rev = 0;
                    }
                    catch (const std::out_of_range& e)       //值溢出
                    {
                        std::cerr << "Out of range: " << val << " is out of the range for an int" << std::endl;
                    }
                }
            }
        }
        //处理电路描述文件中的 <mainCompProps> 标签
        else if( line.find("<mainCompProps") != std::string::npos)
        {
            if (!lastComp) continue;
            SubCircuit* subci = static_cast<SubCircuit*>(lastComp);
            Component* mComp = subci->getMainComp();
            if (!mComp) continue;

            std::string propName = "";
            std::istringstream iss(line);
            std::string prop;
            while (std::getline(iss, prop, '"')) {
                if (prop.back() == '=') {
                    propName = prop.substr(0, prop.size() - 1);
                    continue;
                }
                else if (prop.back() == '/' || prop.back() == '>') {
                    continue;
                }
                if (propName == "MainCompId") {
                    std::string compName = prop;
                    mComp = subci->getMainComp(compName);
                    if (!mComp) std::cerr << "ERROR: Could not create Main Component:" << compName << std::endl;
                }
                else if (mComp) {
                    mComp->setPropStr(propName, prop);
                }
                propName = "";
            }
        }
        else if(line.find("<item") == 0)
        {
            std::string uid, newUid, type, label, newNum;   //存储UID，新UID，类型，标签，序号

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
                else if(prop.back() == '\r')
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
            newUid = uid;
            if (type.empty()) 
            {
                std::cerr << "ERROR: Component with no type:" << label << " " << uid << std::endl;
                continue;
            }
            //if( m_pasting ) // Create new id
            //{
            //    if( type == "Connector" ) newUid = "Connector-"+newConnectorId();
            //    else
            //    {
            //        newNum = newSceneId();
            //        if( type == "Subcircuit" || type == "MCU" )
            //            newUid = uid.split("-").first()+"-"+newNum;
            //        else newUid = type+"-"+newNum;
            //    }
            
            //else newUid = uid;

            if( type == "Connector" )
            {
                Pin* startpin = NULL;
                Pin* endpin   = NULL;
                std::string startpinid, endpinid;
                //std::vector<std::string> pointList;

                std::string name;
                std::string val;
                for(const auto& prop : properties)
                {
                    name = prop.first;
                    val = prop.second;

                    if (name == "startpinid") startpinid = val;
                    else if (name == "endpinid") endpinid = val;
                    //else if (name == "uid") uid = val;
                }
                if (m_redo)
                {
                    try{
                        startpin = m_pinMap.at(startpinid);
                        endpin = m_pinMap.at(endpinid);
                    } catch (const std::out_of_range& e){
                        std::cerr << "The pinid of " << startpinid + " or " +  endpinid << " does not exist !" << std::endl;
                    }

                }
                else 
                {
                    try{
                        startpin = m_LdPinMap.at(startpinid);
                        endpin = m_LdPinMap.at(endpinid);
                    } catch (const std::out_of_range& e){
                        std::cerr << "The pinid of " << startpinid + " or " +  endpinid << " does not exist !" << std::endl;
                    }
                }
                // 通过 ID 在电路中查找引脚
                if (!startpin)
                {
                    try
                    {
                        startpin = m_pinMap.at(startpinid);
                    }
                    catch (const std::out_of_range& e)
                    {
                        std::cerr << "The startpinid of " << startpinid << " does not exist !" << std::endl;
                    }
                }
                if (!endpin)
                {
                    try
                    {
                        endpin = m_pinMap.at(endpinid);
                    }
                    catch (const std::out_of_range& e)
                    {
                        std::cerr << "The endpinid of " << endpinid << " does not exist !" << std::endl;
                    }
                }
                // 如果找到的起始引脚已经有连接器，将其设置为 NULL
                if (startpin && startpin->connector()) startpin = nullptr;
                // 如果找到的结束引脚已经有连接器，将其设置为 NULL
                if (endpin && endpin->connector()) endpin = nullptr;

                if( startpin && endpin )    // Create Connector
                {
                    //设置连接器名称
                    if (newUid.empty()) newUid = "connector-" + newConnectorId();
                    Connector* con = new Connector(type, newUid, startpin, endpin);
                    //con->setPointList(pointList); // 设置连接器的连接点列表
                    conList.insert(con);    // 将新创建的连接器添加到集合中
                    m_newComp = con;   // 设置当前新组件
                    // // 更新连接器计数器---------------------------------------------------------前后端分离后，使用的是eda编辑器导线标准id
                    // std::string number = split(newUid, '-').back();
                    // if (number > m_conNumber) m_conNumber = number;
                }
                else // 如果起始或结束引脚未找到，输出错误信息
                {
                    if (!startpin) std::cerr << "ERROR: null startpin in Connector " << uid << " " << startpinid << std::endl;
                    if (!endpin) std::cerr << "ERROR: null endpin in Connector " << uid << " " << endpinid << std::endl;
                }
                //else if( !m_pasting /*&& !m_undo && !m_redo*/ )// Start or End pin not found
                //{
                //    if (!startpin) std::cerr << "ERROR: null startpin in Connector " << uid << " " << startpinid << std::endl;
                //    if (!endpin) std::cerr << "ERROR: null endpin in Connector " << uid << " " << endpinid << std::endl;
                //}   
            }
            else if( type == "Node")
            {
                std::string name;
                std::string val;
                bool has_num = false;
                for(const auto& prop : properties) {
                    if (prop.first == "Num") {
                        val = prop.second;
                        has_num = true;
                        break; 
                    }
                }
                Node* joint = nullptr; 
                try {
                    if (!has_num) {  
                        joint = new Node(type, newUid);
                    } else {          
                        if (!val.empty()) { 
                            int num = std::stoi(val); 
                            joint = new Node(type, newUid, static_cast<uint16_t>(num));
                        } else {
                            joint = new Node(type, newUid); 
                        }
                    }
                } catch (const std::exception& e) { 
                    std::cerr << "Error creating Node: " << e.what() << std::endl;
                    return; 
                }
                int number = std::stoi(split(joint->getUid(), '-').back());   //获取节点编号，即序列号
                if( number > m_seqNumber ) m_seqNumber = number;              // Adjust item counter: m_seqNumber
                //addItem( joint );                                           //添加节点到场景中
                nodeList.insert( joint );                                     //添加节点到节点列表
                m_newComp = joint;                                            // m_newComp 是一个指向新创建组件的指针，跟踪最近创建或操作的组件。
                m_compMap[newUid] = joint;                                    // m_compMap 是一个映射新 UID 到组件的 map，这样做可以方便地通过 UID 查找对应的节点对象。
            }
            else
            {
                bool oldArduino = false;
                if( type == "Arduino" )
                {
                    oldArduino = true;
                    type = "Subcircuit";
                    newUid.erase(0, newUid.find("Arduino ") + std::string("Arduino ").length()); // Remove "Arduino " from newUid
                }
                else if( type == "AVR" )
                {
                    type = "MCU";
                    newUid.erase(newUid.find("at"), std::string("at").length()); // Remove "at" from newUid
                }
                else if( type == "PIC" )
                {
                    type = "MCU";
                    newUid.erase(newUid.find("pic"), std::string("pic").length()); // Replace "pic" with "p" in newUid
                    newUid.insert(newUid.find("p"), "p");
                }
                else if( type == "Frequencimeter" ) type = "FreqMeter";
                lastComp = NULL;
                //创建元器件，把所有非元器件分类排除后，剩下的type则都为元件类型。
                Component* comp = createItem( type, newUid );
                if(type != "Wire"){
                    int i=1;
                }
                if( comp )
                {
                    m_newComp = comp;
                    lastComp = comp;
                    //if (m_pasting) m_idMap[getSeqNumber(uid)] = newNum; // Map simu id to new id
                    //Mcu* mcu = nullptr;
                    //if (oldArduino)     //处理MCU部分
                    //{
                    //    SubCircuit* subci = static_cast<SubCircuit*>(comp); // 假设 SubCircuit 类型存在
                    //    mcu = static_cast<Mcu*>(subci->getMainComp()); // 假设 SubCircuit 有 getMainComp 方法
                    //}
                    if (comp->itemType() == "Subcircuit") // 处理子电路部分
                    {
                        ShieldSubc* shield = static_cast<ShieldSubc*>(comp);                  // 假设 ShieldSubc 类型存在
                        if (shield->subcType() >= Chip::Shield) shieldList.push_back(shield); // 假设 ShieldSubc 有 subcType 方法
                    }
                    comp->setPropStr((std::string)"label", label); // 组件的标签属性为 label。

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
                            //std::string temp = properties[qpn];
                            prop->setValStr(properties[qpn]);                        // 设置属性的值
                            properties.erase(qpn);                                   //从 properties 映射中移除已设置的属性。
                        }
                    }
                    std::string propName;   //属性名称
                    std::string value;      //值
                    //确保即使在属性名称发生变化或不再直接被识别的情况下，也能够正确地将属性值设置到组件上
                    for (const auto& prop : properties) // Properties not recognized (old versions)
                    {
                        propName = prop.first;
                        value = prop.second;
                        if (!comp->setPropStr(propName, value)) // SUBSTITUTIONS
                        {
                            if (propName == "Propagation_Delay_ns")
                            {
                                propName = "Tpd_ps";
                                value += "000";
                            }
                            else
                            {
                                Component::substitution(propName); // 假设 Component 类有 substitution 方法
                            }

                            //if (!comp->setPropStr(propName, value) && oldArduino && mcu)
                            //{
                            //    mcu->setPropStr(propName, value); // 假设 Mcu 类有 setPropStr 方法
                            //}
                        }
                    }

                    int number = std::stoi(split(comp->getUid(), '-').back());
                    if (number > m_seqNumber) m_seqNumber = number; // Adjust item counter: m_seqNumber
                    //addItem(comp); // Add component to the scene
                    //if (m_pasting) comp->setIdLabel(newUid); // 假设 Component 类有 setIdLabel 方法
                    compList.insert(comp); // 创建组件列表
                    if (comp->m_isLinker) // 
                    {
                        Linkable* l = dynamic_cast<Linkable*>(comp); // 假设 Linkable 类型存在
                        if (l->hasLinks()) linkList.push_back(l); // 假设 Linkable 类有 hasLinks 方法
                    }
                    std::cout << "Creating Component: " << type << " " << uid << std::endl;
                }
                else std::cerr << "ERROR Creating Component: " << type << " " << uid << std::endl;
            }   
        }   
    }
    //if( m_pasting )
    //{
    //    for( Component* comp : compList ) { comp->setSelected( true ); comp->move( m_deltaMove ); }
    //    for( Component* comp : nodeList ) { comp->setSelected( true ); comp->move( m_deltaMove ); }
    //    for( Connector* con  : conList )  { con->setSelected( true );  con->move( m_deltaMove ); }
    //}
    //else for( Component* comp : compList ) { comp->moveSignal(); }

    for( Linkable* l : linkList )
        l->createLinks( &compList );

    m_compList.insert(compList.begin(), compList.end());
    m_nodeList.insert(nodeList.begin(), nodeList.end());
    m_connList.insert(conList.begin(), conList.end());

    // Take care about unconnected Joints
    if (!m_redo) 
    {
        for (Node* joint : nodeList) 
            joint->checkRemove(); // Only removed if some missing connector
    }
    for( ShieldSubc* shield : shieldList ) shield->connectBoard();

    //setAnimate( m_animate ); // Force Pin update

    //if( m_pasting ) m_idMap.clear();
    m_busy = false;
    //QApplication::restoreOverrideCursor();
    //update();
}



//用于创建电子元器件的实例
Component* Circuit::createItem( std::string type, std::string id, bool map )
{
    Component* comp = NULL;
    for( LibraryItem* item : ItemLibrary::self()->items() )
    {
        if( !item->createItemFnPtr() ) continue; // Is category
        if( item->type() != type ) continue;

        comp = item->createItemFnPtr()( type, id );   //item->createItemFnPtr()返回创建函数的指针，再传入（tpye,id)实现实例创建
    }
    if( map ) m_compMap[id] = comp;
    return comp;
}

//void Circuit::removeItems()                     // Remove Selected items
//{
//    if( m_conStarted ) return;
//    m_busy = true;
//    if( m_simulator->isRunning() ) CircuitWidget::self()->powerCircOff();
//
//    beginUndoStep(); // Save current state
//
//    QList<Connector*> conns;
//    QList<Component*> comps;
//
//    for( QGraphicsItem* item : selectedItems() )    // Find all items to be removed
//    {
//        if( item->type() == QGraphicsItem::UserType+2 )      // ConnectorLine: add Connector to list
//        {
//            ConnectorLine* line = qgraphicsitem_cast<ConnectorLine*>( item );
//            Connector* con = line->connector();
//            if( !conns.contains( con ) ) conns.append( con );
//        }
//        else if( item->type() == QGraphicsItem::UserType+1 ) // Component: add Component to list
//        {
//            Component* comp = qgraphicsitem_cast<Component*>( item );
//            comps.append( comp );
//        }
//    }
//    for( Connector* conn : conns ) removeConnector( conn );         // Remove Connectors (does not delete)
//    for( Component* comp: comps ){
//        if( comp->itemType() == "Node" ) removeNode( (Node*)comp ); // Remove Nodes (does not delete)
//        else                             removeComp( comp );        // Remove Components (does not delete)
//    }
//    endUndoStep();                                           // Calculates items actually created/removed
//
//    for( QGraphicsItem* item : selectedItems() ) item->setSelected( false );
//    m_busy = false;
//}

void Circuit::removeComp( Component* comp )
{
    //if( comp->parentItem() )
    //    return; // subcircuit
    m_compRemoved = false;
    comp->remove();
    if( !m_compRemoved ) return;

    if( m_compList.contains( comp ) ) m_compList.erase( comp );
    //removeItem( comp );
    m_compMap.erase( comp->getUid() );
    m_removedComps.insert( comp );
}

void Circuit::removeNode( Node* node )
{
    if (m_nodeList.find(node) == m_nodeList.end()) 
        return;
    if( m_deleting ) return;
    m_nodeList.erase(node);
    m_compMap.erase(node->getUid());
    //removeItem( node );
    m_removedComps.insert( node );
}

void Circuit::removeConnector( Connector* conn )
{
    if (m_connList.find(conn) == m_connList.end()) return; // 检查 conn 是否在 m_connList 中
    conn->remove(); // 执行清理工作
    m_connList.erase(conn); // 从集合中移除 conn

    auto uid = conn->getUid(); 
    m_compMap.erase(uid); // 从映射中移除 conn 的 UID

    m_removedComps.insert(conn); // 将 conn 添加到已移除组件的集合中
}

void Circuit::

clearCircuit() // Remove everything ( Clear Circuit )
{
    if( m_conStarted ) return;
    //m_deleting = true;
    if( m_simulator->isRunning() )  Simulator::self()->stopSim();

    for( Component* comp : m_compList )
    {
        comp->remove();
        //if( comp->scene() ) removeItem( comp );
        delete comp;
    }
    for( Node* node : m_nodeList )
    {
        //if( node->scene() ) removeItem( node );
        delete node;
    }
    //m_deleting = false;
}

void Circuit::setChanged()
{
    m_changed = true;
    //std::string title = MainWindow::self()->windowTitle();
    //if( !title.endsWith('*') ) MainWindow::self()->setWindowTitle(title+'*');
}

void Circuit::saveChanges()
{
    if( m_conStarted || m_circChange.size() == 0 ) return;
    setChanged();

    while( m_undoStack.size() > (m_undoIndex+1) ) 
    {
        m_undoStack.pop_back();
    }

    m_undoStack.push_back( m_circChange );
    if( m_undoStack.size() > m_maxUndoSteps )
    {
        m_undoStack.erase(m_undoStack.begin());
        m_undoIndex--;
    }
    m_undoIndex++;

    clearCircChanges();
    m_cicuitBatch = 0;  // Ends all CicuitChanges
    deleteRemoved();    // Delete Removed Components;

    /// qDebug() << "Circuit::saveChanges ---------------------------"<<m_undoIndex<<m_undoStack.size()<<endl;
}

void Circuit::deleteRemoved()
{
    for( CompBase* comp : m_removedComps ) delete comp;
    m_removedComps.clear();
}


void Circuit::calcCircuitChanges()   // Calculates created/removed
{
    /// std::endl:: << "Circuit::calcCicuitChanges Removed:" << std::endl;
        // 使用 lambda 函数来计算集合差异
    auto set_difference = [](const auto& set1, const auto& set2) 
    {
        std::unordered_set<typename std::decay<decltype(*set1.begin())>::type> result;
        std::for_each(set1.begin(), set1.end(), [&set2, &result](const auto& elem) 
        {
            if (set2.find(elem) == set2.end()) 
            {
                result.insert(elem);
            }
        });
        return result;
    };
    // Items Removed
    auto removedConns = set_difference(m_oldConns, m_connList);
    for (Connector* conn : removedConns) 
    {
        addCompChange(conn->getUid(), COMP_STATE_NEW, m_compStrMap[conn]);
    }   

    auto removedNodes = set_difference(m_oldNodes, m_nodeList);
    for (Node* node : removedNodes) 
    {
        addCompChange(node->getUid(), COMP_STATE_NEW, m_compStrMap[node]);
    } 

    auto removedComps = set_difference(m_oldComps, m_compList);
    for (Component* comp : removedComps) 
    {
        addCompChange(comp->getUid(), COMP_STATE_NEW, m_compStrMap[comp]);
    }    
    //std::cout << "Circuit::calcCicuitChanges Created:" << std::endl;
    // Items Created
    auto createdComps = set_difference(m_compList, m_oldComps);
    for (Component* comp : createdComps) 
    
    {
        addCompChange(comp->getUid(), COMP_STATE_NEW, "");
    }

    auto createdNodes = set_difference(m_nodeList, m_oldNodes);
    for (Node* node : createdNodes) 
    {
        addCompChange(node->getUid(), COMP_STATE_NEW, "");
    }

    auto createdConns = set_difference(m_connList, m_oldConns);
    for (Connector* conn : createdConns) 
    {
        addCompChange(conn->getUid(), COMP_STATE_NEW, "");
    }
}



void Circuit::saveCompChange( std::string component, std::string property, std::string undoVal )
{
    clearCircChanges();
    addCompChange( component, property, undoVal );
    saveChanges();
}

void Circuit::addCompChange( std::string component, std::string property, std::string undoVal )
{
    if( m_loading || m_deleting ) return;                      /// qDebug() << "Circuit::addCompChange      " << component << property;// << value;
    compChange cChange{ component, property, undoVal, "" };
    m_circChange.compChanges.push_back( cChange );
}
//
//void Circuit::restoreState()
//{
//    if( m_simulator->isRunning() ) CircuitWidget::self()->powerCircOff();
//    m_busy = true;
//
//    circChange& step = m_undoStack[ m_undoIndex ];
//
//    int iStep, i;
//    if( m_undo ) { iStep = -1; i = step.compChanges.size()-1; }
//    else         { iStep =  1; i = 0; }
//
//    while( i>=0 && i < step.compChanges.size() )
//    {
//        compChange* cChange = &step.compChanges[i];
//        i += iStep;
//        std::string propName   = cChange->property;
//        std::string propVal    = m_undo ? cChange->undoValue : cChange->redoValue;
//        CompBase* comp     = m_compMap.value( cChange->component );             /// qDebug() << "Circuit::restoreState"<< cChange->component << propName << comp;
//
//        if( propName == COMP_STATE_NEW )  // Create/Remove Item
//        {
//            if( propVal.isEmpty() )       // Remove item
//            {
//                if( !comp ) continue;
//                if( m_undo && cChange->redoValue.isEmpty() ) cChange->redoValue = comp->toString();
//
//                if     ( comp->itemType() == "Connector" ) removeConnector( (Connector*)comp );
//                else if( comp->itemType() == "Node"      ) removeNode( (Node*)comp );
//                else                                       removeComp( (Component*)comp );
//            }
//            else loadStrDoc( propVal );   // Create Item
//        }
//        else if( comp )                   // Modify Property
//        {
//            if( m_undo && cChange->redoValue.isEmpty() ) cChange->redoValue = comp->getPropStr( propName );
//            comp->setPropStr( propName, propVal );
//        }
//    }
//    m_busy = false;
//    deleteRemoved();                      // Delete Removed Components;
//    for( Connector* con : m_connList ) {
//        if( m_board && m_board->m_boardMode ) con->setVisib( false );
//        else{
//            con->startPin()->isMoved();
//            con->endPin()->isMoved();
//        }
//    }
//    update();
//}




void Circuit::newconnector( Pin* startpin, bool save )
{
   //if( save ) beginUndoStep();

    m_conStarted = true;

    std::string type = "Connector";
    std::string id = type + "-" + newConnectorId(); // 假设 newConnectorId 函数返回新连接器的 ID

    m_newConnector = new Connector(type, id, startpin);
    m_connList.insert(m_newConnector);

    //QPoint p1 = startpin->scenePos().toPoint();
    //QPoint p2 = startpin->scenePos().toPoint();
    //m_newConnector->addConLine( p1.x(), p1.y(), p2.x(), p2.y(), 0 );
}

void Circuit::closeconnector( Pin* endpin, bool save )
{
    m_conStarted = false;
    m_newConnector->closeCon( endpin );
    //if( save ) endUndoStep();
}

void Circuit::deleteNewConnector()
{
    if (!m_conStarted) return;
    removeConnector(m_newConnector); // 有 removeConnector 函数来删除连接器
    m_conStarted = false;

}

void Circuit::updateConnectors()
{ /*for( Connector* con : m_connList ) con->updateLines()*/; }

void Circuit::addNode( Node* node )
{
    m_nodeList.insert(node); // 假设 m_nodeList 是一个存储 Node* 的容器，如 std::set 或 std::unordered_set

    // 使用 emplace 方法直接在容器中构造键值对
    m_compMap.emplace(node->getUid(), node);
}


void Circuit::updatePin( ePin* epin, std::string& oldId, std::string& newId )
{
    remPin( oldId );
    Pin* pin = static_cast<Pin*>( epin );
    addPin( pin, newId );
}




