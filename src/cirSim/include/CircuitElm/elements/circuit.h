/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

//#ifdef _WIN32
//#include <windows.h>
//#elif __APPLE__
//#include <mach-o/dyld.h>
//#else
//#include <unistd.h>
//#endif

#include "pch.h"
#include "component.h"
#include "connector.h"
#include "pin.h"

#define COMP_STATE_NEW "__COMP_STATE_NEW__"

class SubPackage;
class Simulator;
class Node;
class ItemLibrary;
class ComponentSelector;

class Circuit
{
    friend class SubCircuit;
    friend class Simulator;

    //Q_OBJECT

    public:
        Circuit(std::string netlist);
        ~Circuit();

 static Circuit* self() { return m_pSelf; }


        ////删除电路或者元器件
        //void removeItems();
        void removeComp( Component* comp) ;
        void compRemoved( bool removed ) { m_compRemoved = removed; }
        void removeNode( Node* node );
        void removeConnector( Connector* conn );
        void clearCircuit();

        //添加或报错元件改变
        void saveChanges();
        void addCompChange(std::string component, std::string property, std::string undoVal);
        void saveCompChange(std::string component, std::string property, std::string undoVal);
        //计算所有改变
        void calcCircuitChanges();

        //设置修改状态
        void setChanged();

        // 接受键值
        //void accepKeys( bool a ) { m_acceptKeys = a; }

        // 加载电路
        void loadCircuit(const std::string& netlist);
        //// 保存电路
        //bool saveCircuit( std::string fileName );

        // 创建组件
        Component* createItem( std::string name, std::string id, bool map=true );

         std::string newConnectorId() { return m_conNumber; }
        // 创建新的连接器
        void newconnector( Pin* startpin, bool save=true );
         // 关闭连接器
        void closeconnector( Pin* endpin, bool save=false );
        // 删除新的连接器
        void deleteNewConnector();
        void updateConnectors();
        // 获取新的连接器
        Connector* getNewConnector() { return m_newConnector; }


        // 添加节点
        void addNode( Node* node );
        // 获取组件列表
        std::unordered_set<Component*>* compList() { return &m_compList; }
        //获取连接器列表
        std::unordered_set<Connector*>* conList()  { return &m_connList; }
        // 获取节点列表
        std::unordered_set<Node*>*      nodeList() { return &m_nodeList; }
        // 获取组件ID映射表
        std::unordered_map<std::string, CompBase*>* compMap() { return &m_compMap;}

        //根据ID获取组件
        Component* getCompById( std::string id );
        //根据名称获取原始ID
        std::string& origId( std::string name ) { return m_idMap.at(name); }

        // 连接器是否已经开始
        bool is_constarted() { return m_conStarted ; }

        // 电路板
        SubPackage* getBoard() { return m_board; }
        // 设置电路板
        void setBoard( SubPackage* b ) { m_board = b; }


        //// 判断是否忙碌
        //bool isBusy()  { return m_busy || m_pasting | m_deleting; }
        // 判断是否子电路
        bool isSubc()  { return m_createSubc; }

        // 添加引脚
        void addPin( Pin* pin, std::string pinId ) { m_pinMap[ pinId ] = pin; m_LdPinMap[ pinId ] = pin; }
        // 移除引脚
        void remPin( std::string pinId ) 
        {
            m_pinMap.erase(pinId); // 从映射中移除元素
        }
        //删除引脚
        void delPin(std::string pinId)
        {
            auto it = m_pinMap.find(pinId);
            if (it != m_pinMap.end())
            {
                delete it->second; // 删除动态分配的 Pin 对象
            }
        }
        // 更新引脚
        void updatePin( ePin* epin, std::string& oldId, std::string& newId );
        //获取引脚
         Pin* getPin( std::string pin ){ return m_pinMap[pin]; }

        // 根据名称获取序列号
        std::string getSeqNumber( std::string& name );
        // 替换引脚名称中的ID
        std::string replaceId( std::string& pinName );

        
        //修改电路元件属性值
        bool upDateCmp(const std::vector<std::string>& message);

        // 获取文件路径
        const std::string getFilePath() const { return m_filePath; }
        // 设置文件路径
        void setFilePath(std::string f) { m_filePath = f; }

        //获取可执行文件路径
        static std::string getExecutablePath();
        //可执行文件所在目录
        static std::string getExecutableDir();

        //获取数据文件路径
        static std::string getDataFilePath(const std::string& file);

        //设置主项目目录
        static void setProFileDir();
        //获取主项目数据文件路径
        static std::string getProFilePath(const std::string& file);
        
        //设置数据文件夹目录
        static void setUserFileDir();
        //获取数据文件路径
        static std::string getUserFilePath(const std::string& file);


        Simulator* m_simulator;                 //指向仿真器对象的指针
        ItemLibrary* Factor;                    //指向元件库的指针
        ComponentSelector* m_components;        //元件选择xml库指针
        
    private:
        static Circuit*  m_pSelf;

        void loadStrDoc( std::string doc );
        //static std::vector<std::string> split(const std::string& str, char delimiter);      
        //bool saveString( std::string &fileName, std::string doc );
        //std::string& circuitHeader();
        //std::string& circuitToString();

        //void updatePinName( std::string* name );

        std::string m_filePath;           //当前打开的电路文件的路径。
        static std::string m_userDir;     //用户数据文件目录
        static std::string m_proDir;      //用户主目录
        std::string m_backupPath;         //当前电路的备份文件路径。
        std::string m_cirmes;             //最开始接受的电路图

        Connector*   m_newConnector;    //指向新创建的连接器对象的指针。
        CompBase*    m_newComp;         //指向新创建的组件对象的指针。

        int m_seqNumber;                //用于为新组件分配序列号的计数器。
        std::string m_conNumber;        //用于为新连接器分配序列号的计数器。  -------------------------------------eda中暂定没啥用后续考虑是否删除
        int m_error;                    //表示电路编辑器中的错误状态或代码。


        bool m_loading;        //指示是否正在加载电路文件的标志
        bool m_conStarted;     //指示是否已经开始创建连接器的标志。
        bool m_changed;
        bool m_redo;           //重做标志
        bool m_busy;           //繁忙标志

        bool m_createSubc;     //指示是否正在创建子电路的标志。
        bool m_compRemoved;    //指示是否有组件被删除的标志
        int m_cicuitBatch;     //可能用于批处理电路编辑操作的计数器。
        bool m_deleting;



        std::unordered_set<Component*> m_compList;     // 存储所有组件的集合。
        std::mutex m_mutex;                            // 互斥锁，用于保护组件列表
        std::unordered_set<Connector*> m_connList;     // 存储所有连接器的集合。
        std::unordered_set<Node*>      m_nodeList;     // 存储所有节点的集合。

        SubPackage* m_board;

        std::unordered_map<std::string, Pin*>      m_pinMap;       // 将引脚ID映射到引脚对象的哈希表。
        std::unordered_map<std::string, Pin*>      m_LdPinMap;     // 在加载/粘贴/导入时将引脚ID映射到引脚对象的哈希表。
        std::unordered_map<std::string, std::string>   m_idMap;    // 在粘贴操作中将组件的序列号映射到新序列号的哈希表。
        std::unordered_map<std::string, CompBase*> m_compMap;      // 将组件ID映射到组件对象的哈希表




        //--- Undo/Redo ----------------------------------
        struct compChange
        {      // Component Change to be performed by Undo/Redo to complete a Circuit change
            std::string component;  // 元件名称
            std::string property;   // 属性名称
            std::string undoValue;  // Property value for Undo step
            std::string redoValue;  // Property value for Redo step
        };
        struct circChange
        {       // Circuit Change to be performed by Undo/Redo to restore circuit state
            std::vector<compChange> compChanges;
            int size() { return compChanges.size(); }
            void clear() { compChanges.clear(); }
        };

        inline void clearCircChanges() { m_circChange.clear(); }
        void deleteRemoved();
        //void restoreState();

        int m_maxUndoSteps;
        int m_undoIndex;

        circChange m_circChange;
        std::vector<circChange> m_undoStack;

        std::unordered_set<CompBase*>  m_removedComps; // removed component list;
        std::unordered_set<Connector*> m_oldConns;
        std::unordered_set<Component*> m_oldComps;
        std::unordered_set<Node*>      m_oldNodes;
        std::unordered_map<CompBase*, std::string> m_compStrMap;
};

