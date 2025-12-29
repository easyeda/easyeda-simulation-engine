/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once
 
#include "pch.h"
#include "e-node.h"
#include "analogclock.h"
#include "e-element.h"
#include "circularBuffer.h"

#include "asio.hpp"

#define ASIO_STANDALONE

enum simState_t
{
    SIM_STOPPED=0,        //停止
    SIM_ERROR,            //报错
    SIM_STARTING,         //开始
    SIM_PAUSED,           //暂停   
    SIM_WAITING,          //警告
    SIM_RUNNING,          //运行
    SIM_DEBUG,            //调试
};

class BaseProcessor;
class Updatable;
class eElement;
class Socket;
class eNode;
class CircMatrix;
class MeterData;
class Meter;

struct ElementStatus{
    std::string id;
    CircularBuffer<std::pair<uint64_t, std::string>> datas;
    
    ElementStatus(const std::string& id, size_t bufferSize = 10000)
        : id(id), datas(bufferSize) {}
        
    ElementStatus(const ElementStatus& other)
        : id(other.id), datas(other.datas.maxSize())
    {
        auto dataVec = other.datas.getAll();
        for(const auto& item : dataVec) {
            datas.push(item);
        }
    }
};

struct OutputData{
        std::shared_ptr<const std::vector<MeterData*>> meterData;
        std::shared_ptr<const std::vector<ElementStatus*>> statusData;
};

class Simulator
{
    friend class eNode;
    friend class Scope;
    
    public:
    using DataUpdateCallback = std::function<void(const OutputData&)>;
   void registerDataCallback(DataUpdateCallback callback) {
    m_dataCallback = callback;
};
    private:
    DataUpdateCallback m_dataCallback;

    public:
        Simulator();  //构造仿真
        ~Simulator();  

        static Simulator* self() { return m_pSelf; }

         void addEvent( uint64_t time, eElement* el );    //添加事件
         void cancelEvents( eElement* el );               //取消事件
 
        void startSim( bool paused=false );               //开始仿真
        void pauseSim();                                  //暂停仿真
        void resumeSim();                                 //恢复仿真
        void stopSim();                                   //停止仿真

        void setSpeed(double speed){ 
            std::lock_guard<std::mutex> lock(simSpeedMutex);
            if(speed && speed <= 30){
                m_speed = 0.1 * speed;
            } else {
            m_speed = 3.0 * std::exp((speed - 30)/10.0);
            }
        };
        int getSpeed(){
            std::lock_guard<std::mutex> lock(simSpeedMutex);
            return static_cast<int>(m_speed * 20);
        };

        void setWarning( int warning ) { m_warning = warning; }   //设置警告
        

        uint64_t realPsPF();               //返回实际每帧仿真时间的皮秒数------------------------------存疑

        uint64_t psPerSec() { return 1e-6; }                // 返回每秒仿真的皮秒数------------------------------------存疑

        void  setSlopeSteps( int steps ) { m_slopeSteps = steps; }          //设置斜率步数
        int slopeSteps( ) { return m_slopeSteps; }                          //返回斜率步数

        void  setMaxNlSteps( uint32_t steps ) { m_maxNlstp = steps; }       //设置最大非线性步数
        uint32_t maxNlSteps( ) { return m_maxNlstp; }                       //返回最大非线性步数

         //返回仿真器是否正在运行
        bool isRunning() { 
            return (m_state >= SIM_STARTING); 
        }  
         //返回仿真器是否处于暂停状态                   
        bool isPaused()  {
            return (m_state == SIM_PAUSED); 
        }                       
        bool isPauseDebug() { return (m_state == SIM_PAUSED && m_debug == true); }  //这个仿真器是否处于暂停调试状态

        uint64_t circTime() { return m_circTime; }              //返回电路仿真时间

        void timerEvent();                                      // 处理定时器事件，猜测与更新仿真状态有关  

        simState_t simState() { return m_state; }               //返回仿真器当前状态

        inline void notCorverged() { m_converged = false; }     //标记仿真未收敛

        void addToEnodeList( eNode* nod );                      //将节点添加到仿真器的节点链表中

        void addToElementList( eElement* el );                  //将电路元素添加到仿真器的电路元素链表中
        void remFromElementList( eElement* el );                //将电路元素从仿真器的电路元素链表中移除

        
        void addToStatusElementList( eElement* el );                 
        void remFromStatusElementList( eElement* el );  
        void updateStatusData(uint64_t currentTime); 

        void addMeter(Meter* meter);                            //添加一个仪表到仪表集合中
        void remFromMeter( Meter *meter);                       //删除一个仪表
        std::vector<Meter *>  getMeters();                      //获取所有仪表
        OutputData getOutputData();                             //获取电路中所有仪表数据
        void updateoutPutData();

        std::vector<eNode*> getNodeList(){return m_eNodeList;}  //获取电路节点列表
        std::vector<eElement*> getElementList(){return m_elementList;}
        std::vector<eElement*> getStatusElementList(){return m_statusElementList;}

        void addToUpdateList( Updatable* el );                  //将可更新的电路元素添加到仿真器的可更新电路元素链表中
        void remFromUpdateList( Updatable* el );                //将可更新的电路元素从仿真器的可更新电路元素链表中移除

        void addToSocketList( Socket* el );                     //将端口添加到仿真器的端口链表中
        void remFromSocketList( Socket* el );                   //将端口从仿真器的端口链表中移除

        void setCurrentModel(bool status){
             std::lock_guard<std::mutex> lock(simSpeedMutex);
            currentModelEnable = status;
        }
        bool getCurrentModel(){
            return currentModelEnable;
        }

        std::mutex m_meterMutex; // 全局仪表锁
    private:
        static Simulator* m_pSelf;

        // Accelerate calls from eNode:
        inline void addToChangedNodes( eNode* nod ) { nod->nextCH = m_changedNode; m_changedNode = nod; }
        inline void addToChangedList( eElement* el ) { el->nextChanged = m_voltChanged; m_voltChanged = el; }
        inline void addToNoLinList( eElement* el ) { el->nextChanged = m_nonLinear;  m_nonLinear = el; }
        //这些内联函数用于将节点或电路元素添加到相应的变化列表中，可能与电压变化或非线性元素的处理有关。


        void createNodes();  //创建节点
        void resetSim();     //重置仿真
        void runCircuit();   //运行电路
        inline void solveCircuit();  //求解电路
        inline void solveMatrix();   //求解矩阵

        inline void clearEventList();  //清除事件列表


        eElement* m_firstEvent;
        //指向仿真中第一个待处理事件的指针。


        CircMatrix* m_matrix;
        //指向电路矩阵的指针，电路矩阵用于存储电路方程的系数和解。

        std::unordered_map<int, std::string> m_errors;
        std::unordered_map<int, std::string> m_warnings;
        //存储仿真过程中错误和警告信息的哈希表，键是错误或警告的代码，值是对应的描述字符串。

        std::vector<eNode*> m_eNodeList;
        //电路中所有节点的列表。

        eNode*    m_changedNode;
        eElement* m_voltChanged;
        eElement* m_nonLinear;
        //分别指向发生变化的节点、电压变化的元件和非线性元件的指针。



        std::vector<eElement*> m_elementList;
        std::vector<eElement*> m_statusElementList;
        std::vector<Updatable*> m_updateList;
        std::vector<Socket*> m_socketList;
        std::vector<Meter*> meterList;
        std::vector<ElementStatus*> m_statusElementData;
        std::future<void> m_dataUpdateFuture; // 异步数据更新任务
        std::promise<void> m_dataUpdatePromise; // 数据更新完成通知
        std::atomic<bool> m_dataUpdateComplete{true}; // 数据更新完成标志
        //分别存储电路中所有元件、仪表、状态器件以及需要更新的对象和插座（可能是连接点）的列表。

        std::mutex simStateMutex;                      
        simState_t m_state;
        simState_t m_oldState;
        //表示当前仿真状态和前一个仿真状态的枚举值,以及仿真状态锁

        std::mutex simSpeedMutex;
        double m_speed;
        //仿真速度，以及仿真速度锁

        bool m_debug;
        bool m_converged;
        bool m_pauseCirc;
        bool currentModelEnable;
        //分别表示是否启用调试模式、电路是否已收敛到稳定状态和是否暂停电路仿真的布尔值。

        int m_error;
        int m_warning;
        //int m_timerId;
        int m_timerTick_ms;
        int m_slopeSteps;
        //分别表示错误计数、警告计数、定时器ID、定时器的时间间隔（毫秒）和斜坡步骤数。

        // uint64_t m_fps;
        uint32_t m_NLstep;        //非线性元件的迭代次数
        uint32_t m_maxNlstp;      //最大迭代次数

        uint64_t m_circTime;      //表示电路仿真的总时间，从仿真开始到当前时刻的累计时间。
        uint64_t scopeTime;
        uint64_t m_currentTime;         //表示当前的时间步骤，用于跟踪仿真的时间进度。
        uint64_t m_lastTime;      //表示上一个时间步骤，用于比较和计算时间差。
        uint64_t m_lastRefT;      // 表示上一个参考时间点，用于计算时间间隔。
        uint64_t m_updtTime;      // 表示仿真更新所需的时间，包括计算和处理电路状态的时间。
        uint64_t lastRunTime;     //上次仿真单词迭代运行时间
        AnalogClock m_analogClock;



        OutputData m_buffers[2];  // 双缓冲区
        std::atomic<int> m_currentBufferIndex{0};
        mutable std::mutex m_garbageMutex;
        mutable std::vector<OutputData> m_garbageCollector;                     


        asio::io_context io;                                               //创建一个 I/O 上下文对象，用于管理异步 I/O 操作。
        std::unique_ptr<asio::steady_timer> m_timer;                       // 使用 unique_ptr 管理 asio::steady_timer 对象的生命周期。steady_timer 是用于设置定时器的类。
        //std::chrono::milliseconds m_timerTick;
        std::thread simThread;                                             // 创建一个 std::thread 对象，用于运行模拟或其他后台任务。
        std::chrono::high_resolution_clock::time_point m_startTime;        // 使用 high_resolution_clock 记录高精度的时间点。
        std::future<void> m_CircuitFuture;                                 //表示一个异步操作的结果。
        std::unique_ptr<asio::io_context::work> work; 

};



