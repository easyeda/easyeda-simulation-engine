/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"

class ePin;
class eElement;

class eNode
{
    public:
        eNode( std::string id );
        ~eNode();

        //返回节点标识符
        std::string itemId() { return m_id; }

        //将一个引脚添加到节点
        void addEpin( ePin* epin );
        //从节点移除一个引脚
        void remEpin( ePin* epin );
        //清除所有节点与引脚的连接
        void clear();

        //当节点电压变化时的回调函数
        void voltChangedCallback( eElement* el );
        //从电压变化回调列表中移除一个元件。
        void remFromChangedCallback( eElement* el );

        //添加一个非线性元件到列表中
        void addToNoLinList( eElement* el );
        //void remFromNoLinList( eElement* el );

        //添加一个连接到节点
        void addConnection( ePin* epin, int node );
        //在节点上赋值电导
        void stampAdmitance( ePin* epin, double admit );

        //添加单独电导
        void addSingAdm( ePin* epin, int node, double admit );
        //赋值单独电导
        void stampSingAdm( ePin* epin, double admit );

        //创建电流、当前值？
        void createCurrent( ePin* epin );
        //电流填值
        void stampCurrent( ePin* epin, double current );

        //返回节点编号
        int  getNodeNumber() { return m_nodeNum; }
        //设置节点编号
        void setNodeNumber( int n ) { m_nodeNum = n; }

        //设置节点组
        void setNodeGroup( int n ){ m_nodeGroup = n; }

        //返回节点电压
        double getVolt() { return m_volt; }
        //设置节点电压
        void   setVolt( double volt );
        //返回电压改变标志
        bool voltchanged() { return m_voltChanged; }
        //设置改变电压的标志
        void setVoltChanged( bool changed ){ m_voltChanged = changed; }

        //初始化节点
        void initialize();
        //矩阵填值
        void stampMatrix();

        //设置节点是否可以自行计算电压
        void setSingle( bool single ) { m_single = single; } // This eNode can calculate it's own Volt
        // //设置节点是否有节点开关
        // void setSwitched( bool switched ){ m_switched = switched; } // This eNode has switches attached

        //更新连接到节点的连接器
        void updateConnectors();


        //返回连接到节点的所有引脚列表。
        std::vector<ePin*> getEpins() { return m_ePinList; }

        //返回节点的所有连接。
        std::vector<int>& getConnections();

        eNode* nextCH;

    private:
        //表示连接到节点的引脚、节点编号和值的内部类。
        class Connection
        {
            public:
                Connection( ePin* e, int n=0, double v=0 ){ epin = e; node = n; value = v; }
                ~Connection(){;}

                Connection* next;   //指向下一个连接对象，这一步让整个对象形成链表
                ePin*  epin;        //指向连接到节点的引脚
                int    node;        //连接的节点编号。
                double value;       // 是与连接相关的值。
        };
        //表示需要在节点电压变化时回调的电子元件
        class CallBackElement
        {
            public:
                CallBackElement( eElement* el ) { element = el; }
                ~CallBackElement(){;}

                CallBackElement* next;  //指向链表中的下一个 CallBackElement 对象。
                eElement* element;      //指向需要回调的电子元件的指针。
        };

        //标记节点状态发生了变化。
        inline void changed();

        //处理只有一个连接的节点的电压计算。(即整个电路只有一条路径)
        inline void solveSingle();

        //清除整个 CallBackElement 链表。
        void clearElmList(CallBackElement* first);
        //清除整个Connection 链表
        void clearConnList( Connection* first );

        std::string m_id;               //节点的唯一标识符。

        std::vector<ePin*> m_ePinList;  //存储连接到节点的引脚的列表。

        CallBackElement* m_voltChEl;    //电压变化时的回调元件列表。
        CallBackElement* m_nonLinEl;    //非线性元件列表。

        Connection* m_firstAdmit;   // Stamp full admitance in Admitance Matrix   在导纳矩阵中填值的完整导纳连接列表。
        Connection* m_firstSingAdm; // Stamp single value   in Admitance Matrix   在导纳矩阵中填值的单独导纳连接列表。
        Connection* m_firstCurrent; // Stamp value in Current Vector              在电流向量中填值的连接列表。利用类似于链表的写法
        Connection* m_nodeAdmit;    //节点导纳连接。

        std::vector<int> m_nodeList;//节点连接列表。

        double m_totalCurr;  //节点的总电流。
        double m_totalAdmit; //节点的总导纳。
        double m_volt;       //节点的电压。

        int m_nodeNum;       //节点编号。
        int m_nodeGroup;     //节点组编号。

        bool m_currChanged;  //电流是否变化的标志。
        bool m_admitChanged; //导纳是否变化的标志。
        bool m_voltChanged;  //电压是否变化的标志。
        bool m_changed;      //节点状态是否变化的标志。
        bool m_single;       //节点是否可以自行计算电压的标志。
        // bool m_switched;     //节点是否连接有开关的标志。
};


