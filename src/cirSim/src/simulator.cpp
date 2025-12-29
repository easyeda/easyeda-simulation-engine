/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "simulator.h"
#include "circuit.h"
#include "matrixsolver.h"
#include "updatable.h"
//#include "outpaneltext.h"
#include "circmatrix.h"
#include "e-element.h"
#include "socket.h"
#include "meter.h"
#include "ammeter.h"
#include "probe.h"

Simulator* Simulator::m_pSelf = NULL;
std::mutex mtx;  // 互斥锁
using namespace std::chrono;

Simulator::Simulator()
{
    m_pSelf = this;
    m_matrix = new CircMatrix();
    addToElementList( &m_analogClock );
    addToUpdateList( &m_analogClock );

    m_timerTick_ms = 20;   // 定时器的默认时间间隔为50毫秒
    scopeTime = 0;
    m_currentTime = 0;
    m_speed = 1;
    lastRunTime = 0;

    currentModelEnable = true;


    m_maxNlstp  = 100000;
    m_slopeSteps = 0;
    //设置非线性迭代的最大步骤数为100000，斜坡步骤数初始化为0。

    m_errors[0] = "";
    m_errors[1] = "Could not solve Matrix";
    m_errors[2] = "Add Event: NULL free event";
    m_errors[3] = "LAST_SIM_EVENT reached";

    m_warnings[1] = "NonLinear Not Converging";
    m_warnings[2] = "Probably Circuit Error";  // Warning if matrix diagonal element = 0.
    m_warnings[100] = "AVR crashed !!!";

    Meter* meter = nullptr;      //创建电压探针

    resetSim();  //调用 resetSim 方法来重置仿真环境，准备开始新的仿真。
    //CircuitWidget::self()->setMsg( " "+tr("Stopped")+" ", 1 ); //调用 CircuitWidget 的 setMsg 方法来设置初始状态消息，通常显示为“Stopped”，表示仿真尚未开始。
    m_startTime = std::chrono::high_resolution_clock::now(); // 开始时间
    m_state = SIM_STOPPED;

}


Simulator::~Simulator()
{
    io.stop();
    work.reset();  // 允许 io.run() 退出
    if (simThread.joinable()) simThread.join(); // 清理线程
    if (m_CircuitFuture.valid()) // 检查 future 是否有关联的异步任务
    {
        m_CircuitFuture.wait(); // 阻塞直到异步任务完成
    }
    delete m_matrix;
}

inline void Simulator::solveMatrix()
{
    /*std::cout << "enter solveMatrix" << std::endl;*/
    // auto start = std::chrono::high_resolution_clock::now();

    while( m_changedNode )
    {
        m_changedNode->stampMatrix();
        m_changedNode = m_changedNode->nextCH;
    }
    if( !m_matrix->solveMatrix() ) {
        m_warning = 2;             // Warning if diagonal element = 0.
    }
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // std::cout << "solveMatrix() execution time: " << duration.count() << " us" << std::endl;
}

void Simulator::timerEvent()  //update at m_timerTick_ms rate (50 ms, 20 Hz max)  每个定时器事件发生时调用
{
    // 检查暂停状态时要加锁
    {
        std::lock_guard<std::mutex> lock(simStateMutex);
        // || m_state <= SIM_WAITING   这个是否需要待定
        if(m_state <= SIM_PAUSED) return;
    }

    if (m_error)
    {
        if (Simulator::self()->isRunning())    //多线程访问出现同步占用问题
        {
           Simulator::self()->stopSim();
        }
        if (m_errors.find(m_error) != m_errors.end())
        {
            std::cerr << "Error:" << m_errors[m_error] << std::endl;
        }
        return;
    }
    
    if( m_warning > 0 )
    {
        int type = (m_warning < 100)? 1:2;
        //CircuitWidget::self()->setMsg( m_warnings.value( m_warning), type );
        std::cerr << m_warnings[m_warning] << std::endl;
        m_warning = -10;
    }
    else if( m_warning < 0 )
    {
        if (++m_warning == 0) std::cout << "Running" << std::endl;
    }

     //如果电路的并行线程尚未完成，等待它完成
     {
        std::lock_guard<std::mutex> lock(simStateMutex);
        if (m_CircuitFuture.valid() && m_state != SIM_WAITING) 
        {
            simState_t state = m_state;
            m_state = SIM_WAITING;
            m_CircuitFuture.wait(); // 等待异步任务完成
            m_state = state;
        }
     }


    //模拟器状态为运行中（SIM_RUNNING），则在一个并行线程中运行电路模拟。
    { std::lock_guard<std::mutex> lock(simStateMutex);
        if (m_state == SIM_RUNNING) 
        {
            m_CircuitFuture = std::async(std::launch::async, [this]() {
                this->runCircuit();
                });
        }
    }



    //更新连接器
    if ((m_circTime - m_updtTime) >= 2e8) 
    { 
        //Circuit::self()->updateConnectors();
        for (eNode* node : m_eNodeList) node->updateConnectors();
        m_updtTime = m_circTime;
    }

    m_timer->expires_after(std::chrono::milliseconds(m_timerTick_ms));
    m_timer->async_wait([this](const asio::error_code& error) {
        if(!error) this->timerEvent();
    });
}

void Simulator::runCircuit()
{

    //std::cout << "circuit running" << std::endl;
    int currentCount = 0;

    auto now1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    const int targetSpeed = getSpeed();

    while(currentCount < targetSpeed)
    {
        for(size_t i = 0; i < m_updateList.size(); i++) {
            m_updateList[i]->updateStep();
        }
        solveCircuit();                         // Solve any pending changes  solveCircuit 方法来解决电路中的任何待处理变化
        if(m_state < SIM_RUNNING) return;


        eElement* event = m_firstEvent;             //指向电路事件的指针，并初始化为第一个事件的指针。
        uint64_t endRun = m_circTime + m_analogClock.getStep();      // Run upto next Timer event  计算运行结束的时间
        uint64_t nextTime;                          //存储下一个事件的时间戳。

        //模拟器的事件循环。
        while( event )                              
        {
            //std::cout << "im  here" << std::endl;
            if( event->eventTime > endRun ) break;      // 如果当前事件的时间戳超过了运行结束时间 endRun，则跳出循环

            nextTime = m_circTime;
            while( m_circTime == nextTime )             // 开始处理同一个时间戳下的所有事件
            {
                m_circTime = event->eventTime;          //更新电路时间 m_circTime 为当前事件的时间戳。
                m_firstEvent = event->nextEvent;        //将事件指针更新为下一个事件，以便释放当前事件
                event->nextEvent = NULL;                //清除当前事件的 nextEvent 指针和 eventTime，以便它不再指向任何事件
                event->eventTime = 0;
                event->runEvent();                      //当前事件的 runEvent 方法，执行与该事件关联的回调函数。
                event = m_firstEvent;                   //更新指针为下一个事件
                
                //如果有下一个事件，则更新下一个时间戳，如果没有则返回
                if( event ) nextTime = event->eventTime;
                else break;
            }
            solveCircuit();                              //再次调用 solveCircuit 方法来解决电路中的任何新变化。
            if( m_state < SIM_RUNNING ) break;
            event = m_firstEvent;                        // 更新 event 指针为可能在 solveCircuit 方法中添加的新事件。
        }
        if( m_state > SIM_WAITING ) 
            m_circTime = endRun; //如果模拟器的状态是运行中或更高状态，则将电路时间 m_circTime 更新为运行结束时间 endRun。
        currentCount++;
        auto now2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        auto now = now2 - now1;
        if(currentCount * 1000 >= targetSpeed*(now2 - lastRunTime) || (now2 - lastRunTime > 500))
        {
            lastRunTime = now2;
            break;
        }      
    }
}

void Simulator::solveCircuit()
{
    //循环会持续进行，直到没有更多改变的节点（m_changedNode），没有非线性组件需要处理（m_nonLinear），并且电路已经收敛（m_converged）。
    while( m_changedNode || m_nonLinear || !m_converged ) // Also Proccess changes gererated in voltChanged()
    {
        //有节点的电压改变了（m_changedNode 为真），就调用 solveMatrix() 方法来解决电路的线性方程组。
        if( m_changedNode ) 
            solveMatrix();

        //检查电路是否收敛
        if( m_converged ) m_converged = m_nonLinear==NULL;
        //循环处理所有非线性组件，直到电路收敛
        while( !m_converged )              // Non Linear Components
        {
            m_converged = true;
            //重置 added 标志，调用 voltChanged() 方法来处理电压改变，并移动到下一个已改变的非线性组件。
            while( m_nonLinear )
            {
                m_nonLinear->added = false;
                m_nonLinear->voltChanged();
                m_nonLinear = m_nonLinear->nextChanged;
            }
            if( m_maxNlstp && (m_NLstep++ >= m_maxNlstp) ) { m_warning = 1; return; } // Max iterations reached   表示达到了最大非线性步骤限制，电路未能收敛
            if( m_state < SIM_RUNNING ){ m_converged = false; break; }    // Loop broken without converging  电路未收敛，退出电路运行
            if( m_changedNode ) solveMatrix();  //如果有节点的电压改变了，再次调用 solveMatrix() 方法来解决线性方程组。
        }
        if( !m_converged ) return; // Don't run linear until nonliear converged (Loop broken)  如果电路没有收敛，返回并停止进一步处理。

        m_NLstep = 0;
        //处理所有标记为电压已改变的组件（m_voltChanged）。
        while( m_voltChanged )
        {
            m_voltChanged->added = false;
            m_voltChanged->voltChanged();
            m_voltChanged = m_voltChanged->nextChanged;
        }
        if( m_state < SIM_RUNNING ) break;    // Loop broken without convergin
    }
    if(scopeTime != m_circTime){
        scopeTime = m_circTime;
        {
            std::lock_guard<std::mutex> lock(m_meterMutex);
            for (auto meter : meterList) {  meter->timeStep(scopeTime); }
            updateStatusData(scopeTime);
        }
        updateoutPutData();
    }
}

void Simulator::resetSim()
{
    m_state    = SIM_STOPPED;

    m_error    = 0;
    m_warning  = 0;

    m_lastRefT = 0;
    m_circTime = 1;
    m_updtTime = 0;
    m_NLstep   = 0;


    //InfoWidget::self()->setCircTime( 0 );   --------------------------------------------------------------------------------------//图形界面不需要
    clearEventList();
    m_changedNode = NULL;
    m_voltChanged = NULL;
    m_nonLinear = NULL;
}

void Simulator::createNodes()
{
    std::cout <<"\ncreateNodes...\n";
    //提前清空电路节点
    for( eNode* enode : m_eNodeList )
    {
        enode->clear();
        delete enode;
    }
    m_eNodeList.clear();

    int i = 0;
    std::vector<std::string> pinList;  //用于存储已处理的引脚ID
    std::vector<std::string> pinNames; //存储所有引脚的名称。
    for (const auto& pair : Circuit::self()->m_pinMap) 
    {
        pinNames.push_back(pair.first);
    } //取引脚名称列表

    std::sort(pinNames.begin(), pinNames.end());
    //遍历引脚名称列表
    for( std::string& pinName : pinNames )
    {
        Pin* pin = nullptr;
        auto it = Circuit::self()->m_pinMap.find(pinName);  //返回对应引脚
        if (it != Circuit::self()->m_pinMap.end()) 
        {
            pin = it->second;
        }
        if( !pin ) continue;
        if (std::find(pinList.begin(), pinList.end(), pinName) != pinList.end()) continue;  //pinName 已经存在于 pinList 中，说明该引脚已经被处理过，跳过当前迭代。
        if( !pin->conPin() ) continue;   //如果当前 Pin 对象没有连接到其他引脚
        if( pin->isBus() ) continue;
        if (pinName.find("Node") == 0) continue;

        eNode* node = new eNode( "eNodeSim-" + std::to_string(i));
        i++;
        std::cout << "--------------createNode " << i << node->itemId() << std::endl;;
        pin->registerPinsW( node );
        pin->registerEnode( node );
        //核心，用来将当前处理的引脚以及与之电气上相连的所有引脚注册到新创建的电子节点上。

        //遍历新创建的节点node中的所有电子引脚（ePins），并将它们的ID添加到pinList中，以避免重复处理。
        for( ePin* nodePin : node->getEpins() )
        {
            std::string pinId = nodePin->getId();//qDebug() <<pinId<<"\t\t\t"<<nodePin->getEnode()->itemId();
            if( pinId.find("Node") == 0 ) continue;
            if (std::find(pinList.begin(), pinList.end(), pinId) == pinList.end()) 
            { 
                pinList.push_back(pinId);
            }
        }
    }
    std::cout <<"Created      "<< i << "\teNodes   "<<pinList.size()<<"  Pins\n";
}

void Simulator::startSim(bool paused)
{
    resetSim();   //重设仿真引擎，清空设置
    // setPsPerSec(m_psPerSec);   //设置每秒仿真的步数，用于控制仿真速度
    m_debug = paused;   //指示是否启动调试模式
    {
        std::lock_guard<std::mutex> lock(simStateMutex);
        m_state = SIM_STARTING;
    }

    
    std::cout << "\nStarting Circuit Simulation...\n";

    for (Socket* sock : m_socketList) sock->connectPins( true );  //遍历所有的插座（Socket）对象，并更新它们的引脚状态

    createNodes(); //创建节点

    std::cout << "Initializing " << m_elementList.size() << "\teElements \n";
    for (eElement* el : m_elementList)    // Initialize all Elements  遍历所有电子元器件
    {                                      // This can create new eNodes  可以创建电路节点
        std::cout << "initializing  "<< el->getId() << std::endl;
        el->eventTime = 0;
        el->initialize();   //初始化他们的状态
        el->added = false;
    }

    std::cout << "Initializing " << m_eNodeList.size() << "\teNodes\n";
    //所有的电路节点（eNode），初始化它们并设置节点编号。
    for (int i = 0; i < m_eNodeList.size(); i++)         // Initialize eNodes
    {
        eNode* enode = m_eNodeList.at(i);
        enode->setNodeNumber(i + 1);
        enode->initialize();
        std::cout << "initializing  " << enode->itemId() << std::endl;
    }
    //遍历所有的电路元件，调用它们的 stamp 方法
    for (eElement* el : m_elementList) el->stamp();

    //创建矩阵
    m_matrix->createMatrix(m_eNodeList);

    std::cout << "\nCircuit Matrix looks good";


    std::cout << "\n    Simulation Running... \n";


    // auto now = std::chrono::high_resolution_clock::now();
    // m_startTime = now;
   // 创建定时器并启动异步等待
    m_timer = std::make_unique<asio::steady_timer>(io, m_timerTick_ms);
    m_timer->async_wait([this](const asio::error_code& error) { 
        {
            std::lock_guard<std::mutex> lock(simStateMutex);
            m_state = SIM_RUNNING;
        }
        this->timerEvent(); 
    });
    work = std::make_unique<asio::io_context::work>(io);
    simThread = std::thread([this]() { io.run(); });

}

void Simulator::stopSim()
{
     std::lock_guard<std::mutex> lock(simStateMutex);
    if (m_timer)
    {
        m_timer->cancel();
        m_timer.reset();
    }
    m_state = SIM_STOPPED;
    // 等待异步任务完成
    if (m_CircuitFuture.valid()) 
    {
        m_CircuitFuture.wait();
    }

    std::cout << "\n    Simulation Stopped ";

    for( eNode* node  : m_eNodeList  )  node->setVolt( 0 );
    for( eElement* el : m_elementList ) el->initialize();
    for( Updatable* el : m_updateList ) el->updateStep();

    clearEventList();
    m_changedNode = NULL;
    //if( EditorWindow::self()->debugState() > DBG_STOPPED ) EditorWindow::self()->stop();
}

uint64_t Simulator::realPsPF()
{
    return m_analogClock.getStep();
}

//暂停仿真
void Simulator::pauseSim() // Only pause simulation, don't update UI
{
    std::lock_guard<std::mutex> lock(simStateMutex);
    if(m_state <= SIM_PAUSED) return;
    m_oldState = m_state;
    m_state = SIM_PAUSED;
    // 暂停定时器
    if(m_timer) {
        m_timer->cancel();
    }
    // 等待当前运行的电路模拟完成
    if(m_CircuitFuture.valid()) {
        m_CircuitFuture.wait();
    }
    
}

//恢复仿真
void Simulator::resumeSim()
{
    std::lock_guard<std::mutex> lock(simStateMutex);
    if(m_state != SIM_PAUSED) return;
     m_state = m_oldState;
    // 重新启动定时器
    m_timer->expires_after(std::chrono::milliseconds(m_timerTick_ms));
    m_timer->async_wait([this](const asio::error_code& error) {
        if (error && error != asio::error::operation_aborted) {
            std::cerr << "Timer error: " << error.message() << std::endl;
            return;
        }
        if (!error) this->timerEvent();
    });
    
   
}

void Simulator::clearEventList()
{
    m_firstEvent = NULL;
}

//增加一个事件
void Simulator::addEvent( uint64_t time, eElement* el )
{

    if( m_state < SIM_STARTING ) return;   // 如果模拟器状态小于SIM_STARTING，则不添加事件，直接返回。

    //如果事件el已经有一个事件时间，打印警告并返回。
    if( el->eventTime )
    { std::cout << "Warning: Simulator::addEvent Repeated event"<<el->getId(); return; }

    time += m_circTime;     //将传入的时间加上当前的电路时间（m_circTime），得到事件的绝对时间。
    eElement* last  = NULL;// 初始化一个指向上一个事件的指针。
    eElement* event = m_firstEvent;// 初始化一个指向当前事件的指针，从事件列表的头部开始。

    while( event ){
        if( time <= event->eventTime ) break; // Insert event here
        last  = event;
        event = event->nextEvent;
    }
    el->eventTime = time;
    //遍历事件列表，寻找合适的插入位置。如果找到一个事件其发生时间晚于或等于新事件的时间，就在这个位置插入新事件
    //last是新事件的前一个事件，event是新事件的后一个事件，这里是以时间为依据，event为循环的当前事件，如果event的时间一旦大于time了证明找到了插入位置

    if( last ) last->nextEvent = el;
    else       m_firstEvent = el; // List was empty or insert First

    el->nextEvent = event;
}


//移除一个事件
void Simulator::cancelEvents( eElement* el )
{
    if( el->eventTime == 0 ) return;
    eElement* event = m_firstEvent;
    eElement* last  = NULL;
    eElement* next  = NULL;
    el->eventTime = 0;

    while( event ){
        next = event->nextEvent;
        if( el == event )
        {
            if( last ) last->nextEvent = next;
            else       m_firstEvent = next;
            event->nextEvent = NULL;
        }
        else last = event;
        event = next;
    }   
}

//添加一个节点到节点列表中
void Simulator::addToEnodeList( eNode* nod )
{ 
    if(std::find(m_eNodeList.begin(), m_eNodeList.end(), nod) == m_eNodeList.end()) 
        m_eNodeList.push_back( nod ); 
}

//添加一个元件到元件列表中
void Simulator::addToElementList( eElement* el )
{
    if (std::find(m_elementList.begin(), m_elementList.end(), el) == m_elementList.end())
        m_elementList.push_back(el);
 }

//移除一个元件
void Simulator::remFromElementList( eElement* el )
{ 
    auto it = std::remove(m_elementList.begin(), m_elementList.end(), el);
    m_elementList.erase(it, m_elementList.end());
}

//添加一个状态元件到状态元件列表中
void Simulator::addToStatusElementList( eElement* el )
{
    if (std::find(m_statusElementList.begin(), m_statusElementList.end(), el) == m_statusElementList.end())
        m_statusElementList.push_back(el);
    ElementStatus* data = new ElementStatus(el->getId(), 100000);   //长度定义后续需要优化
    m_statusElementData.push_back(data);
 }
//移除一个状态元件
void Simulator::remFromStatusElementList( eElement* el )
{ 
    auto it = std::remove(m_statusElementList.begin(), m_statusElementList.end(), el);
    m_statusElementList.erase(it, m_statusElementList.end());
}

void Simulator::updateStatusData(uint64_t currentTime)
{
    for(auto statusElement: m_statusElementList){
       for(auto statusElementData:m_statusElementData){
            if(statusElement->getId() == statusElementData->id){
                uint64_t inputTime = currentTime;
                std::string inputMes = statusElement->getElementStatusData();
                statusElementData->datas.push(std::make_pair(inputTime,inputMes));
            }
       }
    }
}

void Simulator::updateoutPutData()
{
    // 1. 准备新数据
    auto newMeterData = std::make_shared<std::vector<MeterData*>>();
    auto newStatusData = std::make_shared<std::vector<ElementStatus*>>();

    // 填充数据
    newMeterData->reserve(meterList.size());
    for (Meter* meter : meterList) {
        for (const auto& [key , value] : meter->getData()) {
            newMeterData->push_back(value);
        }
    }

    newStatusData->reserve(m_statusElementData.size());
    for (auto statusElementData : m_statusElementData) {
        newStatusData->push_back(statusElementData);
    }
    // 2. 原子切换缓冲区
    int newIndex = 1 - m_currentBufferIndex.load(std::memory_order_relaxed);
    m_buffers[newIndex] = {newMeterData, newStatusData};
    m_currentBufferIndex.store(newIndex, std::memory_order_release);
    // 3. 延迟回收旧数据
    std::lock_guard<std::mutex> lock(m_garbageMutex);
    m_garbageCollector.push_back(m_buffers[1 - newIndex]);
    if (m_garbageCollector.size() > 5) m_garbageCollector.clear();
}

OutputData Simulator::getOutputData()
{
    int readIndex = m_currentBufferIndex.load(std::memory_order_acquire);
    return {
        m_buffers[readIndex].meterData ? m_buffers[readIndex].meterData 
                                      : std::make_shared<std::vector<MeterData*>>(),
        m_buffers[readIndex].statusData ? m_buffers[readIndex].statusData
                                       : std::make_shared<std::vector<ElementStatus*>>()
    };
}



void Simulator::addMeter(Meter *meter)
{
    //没有判断是否重复，后续优化
    meterList.push_back(meter);
}

void Simulator::remFromMeter(Meter *meter)
{
    auto it = std::remove(meterList.begin(), meterList.end(), meter);
    meterList.erase(it, meterList.end());
}

std::vector<Meter *>  Simulator::getMeters()
{
    return meterList;
}

//将需要更新的对象添加到更新列表中
void Simulator::addToUpdateList( Updatable* el )
{ 
    if (std::find(m_updateList.begin(), m_updateList.end(), el) == m_updateList.end()) 
    {
        m_updateList.push_back(el);
    }

}

void Simulator::remFromUpdateList( Updatable* el )
{ 
    auto it = std::remove(m_updateList.begin(), m_updateList.end(), el);
    m_updateList.erase(it, m_updateList.end());
}

void Simulator::addToSocketList( Socket* el )
{ 
    if (std::find(m_socketList.begin(), m_socketList.end(), el) == m_socketList.end()) 
    {
        m_socketList.push_back(el);
    }
}

void Simulator::remFromSocketList( Socket* el )
{ 
    auto it = std::remove(m_socketList.begin(), m_socketList.end(), el);
    m_socketList.erase(it, m_socketList.end());

}
//#include "moc_simulator.cpp"
