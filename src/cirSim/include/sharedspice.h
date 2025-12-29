/* header file for shared ngspice */
/* Copyright 2021 Holger Vogt */
/* Modified BSD license */

/*
Interface between a calling program (caller) and ngspice.dll (ngspice.so)

**
ngSpice_Init(SendChar*, SendStat*, ControlledExit*,
             SendData*, SendInitData*, BGThreadRunning*, void*)

调用方加载ngsipce.dll库后，必须通过ngSpice_init()来初始化模拟器
调用方中定义的多个回调函数的地址指针将发送到ngspice.dll
After caller has loaded ngspice.dll, the simulator has to be initialized
by calling ngSpice_Init(). Address pointers of several callback functions
defined in the caller are sent to ngspice.dll.

Callback funtion typedefs
SendChar       typedef of callback function for reading printf, fprintf, fputs   用于读取 printf、fprintf、fput 的回调函数的重定义
SendStat       typedef of callback function for reading status string and percent value  用于读取状态字符串和百分比值的回调函数的重定义
ControlledExit typedef of callback function for tranferring a signal upon
               ngspice controlled_exit to caller. May be used by caller
               to detach ngspice.dll.
               用于在 ngspice controlled_exit 上将信号传输给调用方。呼叫者可以使用它来分离ngspice.dll。
SendData       typedef of callback function for sending an array of structs containing
               data values of all vectors in the current plot (simulation output)
               用于发送包含当前图中所有向量数据值的结构数组（仿真输出）
SendInitData   typedef of callback function for sending an array of structs containing info on
               all vectors in the current plot (immediately before simulation starts)
               用于发送包含当前绘图中所有向量信息的结构数组（在仿真开始之前）
BGThreadRunning typedef of callback function for sending a boolean signal (true if thread
                is running)
                用于发送仿真状态

The void pointer may contain the object address of the calling
function ('self' or 'this' pointer), so that the answer may be directed
to a calling object. Callback functions are defined in the global section.
void指针可以包含调用函数的对象地址（'self'或'this'指针），以便应答可以定向到调用对象。回调函数在全局部分中定义。

**
ngSpice_Command(char*)
Send a valid command (see the control or interactive commands) from caller
to ngspice.dll. Will be executed immediately (as if in interactive mode).
Some commands are rejected (e.g. 'plot', because there is no graphics interface).
Command 'quit' will remove internal data, and then send a notice to caller via
ngexit().
从调用方发送有效命令（请参阅控制命令或交互式命令）到ngspice.dll。
将立即执行（就像在交互模式下一样）。
某些命令被拒绝（例如“plot”，因为没有图形界面）。
命令 'quit' 将删除内部数据，然后通过 ngexit（） 向调用方发送通知。

**
ngGet_Vec_Info(char*)
receives the name of a vector (may be in the form 'vectorname' or
<plotname>.vectorname) and returns a pointer to a vector_info struct.
The caller may then directly assess the vector data (but probably should
not modify them).
接收向量的名称（可能采用“vectorname”或 <plotname>.vectorname 的形式），并返回指向vector_info结构的指针。
然后，调用者可以直接评估矢量数据（但可能应该不修改它们）。

***************** If XSPICE is enabled *************************************
**
ngCM_Input_Path(const char*)
Set the input path for files loaded by code models
like d_state, file_source, d_source.
Useful when netlist is sent by ngSpice_Circ and therefore
Infile_Path cannot be retrieved automatically.
If NULL is sent, return the current Infile_Path.

**
ngGet_Evt_NodeInfo(char*)
receives the name of a event node vector (may be in the form 'vectorname' or
<plotname>.vectorname) and returns a pointer to a evt_node_info struct.
The caller may then directly assess the vector data.

**
char** ngSpice_AllEvtNodes(void);
returns to the caller a pointer to an array of all event node names.
****************************************************************************

**
ngSpice_Circ(char**)
sends an array of null-terminated char* to ngspice.dll. Each char* contains a
single line of a circuit (each line like in an input file **.sp). The last
entry to char** has to be NULL. Upon receiving the arry, ngspice.dll will
immediately parse the input and set up the circuit structure (as if received
the circuit from a file by the 'source' command.

**
char* ngSpice_CurPlot();
returns to the caller a pointer to the name of the current plot

**
char** ngSpice_AllPlots()
returns to the caller a pointer to an array of all plots (by their typename)

**
char** ngSpice_AllVecs(char*);
returns to the caller a pointer to an array of vector names in the plot
named by the string in the argument.

**
int ngSpice_LockRealloc(void)
int ngSpice_UnlockRealloc(void)
Locking and unlocking the realloc of output vectors during simulation. May be set
during reading output vectors in the primary thread, while the simulation in the
background thread is moving on.

**
Additional basics:
No memory mallocing and freeing across the interface:
Memory allocated in ngspice.dll has to be freed in ngspice.dll.
Memory allocated in the calling program has to be freed only there.

ngspice.dll should never call exit() directly, but handle either the 'quit'
request to the caller or an request for exiting upon error,
done by callback function ngexit().

All boolean signals (NG_BOOL) are of type _Bool, if ngspice is compiled. They
are of type bool if sharedspice.h is used externally.
*/

#ifndef NGSPICE_PACKAGE_VERSION
#define NGSPICE_PACKAGE_VERSION "42"
#endif
/* we have NG_BOOL instead of BOOL */
#ifndef HAS_NG_BOOL
#define HAS_NG_BOOL 1
#endif

#ifndef NGSPICE_DLL_H
#define NGSPICE_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__CYGWIN__)
  #ifdef SHARED_MODULE
    #define IMPEXP __declspec(dllexport)
  #else
    #define IMPEXP __declspec(dllimport)
  #endif
#else
  /* use with gcc flag -fvisibility=hidden */
  #if __GNUC__ >= 4
    #define IMPEXP __attribute__ ((visibility ("default")))
    #define IMPEXPLOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define IMPEXP
    #define IMPEXP_LOCAL
  #endif
#endif

/* required only if header is used by the caller,
   is already defined in ngspice.dll */
/*仅当调用方使用标头时才需要，已在ngspice.dll中定义*/
#ifndef ngspice_NGSPICE_H
/* Complex numbers. */
struct ngcomplex {
    double cx_real;
    double cx_imag;
} ;

typedef struct ngcomplex ngcomplex_t;
#endif

/* NG_BOOL is the boolean variable at the ngspice interface.
   When ompiling ngspice shared module, typedef to _BOOL, which is boolean in C,
   when used externally, keep it to be of type bool,
   as has been available in the past. */
/*NG_BOOL 是 ngspice 接口上的布尔变量。
  当将 ngspice 共享模块 typedef 压缩为 _BOOL（在 C 语言中是布尔值）时，
  当外部使用时，请将其保持为 bool 类型，就像过去一样。*/
#ifndef SHARED_MODULE
typedef bool NG_BOOL;
#else
typedef _Bool NG_BOOL;
#endif

/* vector info obtained from any vector in ngspice.dll.
   Allows direct access to the ngspice internal vector structure,
   as defined in include/ngspice/devc.h . */
/*从 ngspice.dll 中的任何向量获取的向量信息，允许直接访问 ngspice 内部向量结构，如 include/ngspice/devc.h 中所定义。*/

//包含了一个特定向量（例如仿真中的一个节点电压或电流）的信息。它包含向量的名称、类型、标志、实际数据（实数或复数），以及向量的长度。
typedef struct vector_info {
    char *v_name;		/* Same as so_vname. */
    int v_type;			/* Same as so_vtype. */
    short v_flags;		/* Flags (a combination of VF_*). */
    double *v_realdata;		/* Real data. */
    ngcomplex_t *v_compdata;	/* Complex data. */
    int v_length;		/* Length of the vector. */
} vector_info, *pvector_info;




//包含了一个特定向量的实际数据值。这个结构体用于传递单个数据点的值，包括它的名称、实部和虚部值，以及标记该向量是否是比例向量（例如时间或频率的向量）和是否包含复数数据。
typedef struct vecvalues {
    char* name;        /* name of a specific vector */
    double creal;      /* actual data value */
    double cimag;      /* actual data value */
    NG_BOOL is_scale;     /* if 'name' is the scale vector */
    NG_BOOL is_complex;   /* if the data are complex numbers */
} vecvalues, *pvecvalues;



//包含了一组向量的值。它包含了向量的数量、当前数据集的索引（即接受的数据点的数量），以及指向 vecvalues 结构体数组的指针，这个数组包含了所有向量的值。
typedef struct vecvaluesall {
    int veccount;      /* number of vectors in plot */
    int vecindex;      /* index of actual set of vectors. i.e. the number of accepted data points */
    pvecvalues *vecsa; /* values of actual set of vectors, indexed from 0 to veccount - 1 */
} vecvaluesall, *pvecvaluesall;




/* info for a specific vector */
//关于单个向量的信息，这些信息在 ngSpice 仿真中用于管理和访问仿真数据。每个向量代表一个仿真变量，例如节点电压或支路电流。
typedef struct vecinfo
{
    int number;     /* number of vector, as postion in the linked list of vectors, starts with 0  向量在向量链表中的位置编号，从 0 开始。*/
    char *vecname;  /* name of the actual vector 实际向量的名称。 */
    NG_BOOL is_real;   /* TRUE if the actual vector has real data  一个布尔值，如果为 TRUE，则表示实际向量包含实数数据。*/
    void *pdvec;    /* a void pointer to struct dvec *d, the actual vector 指向 dvec 结构体的指针，dvec 结构体包含了实际向量的数据。*/
    void *pdvecscale; /* a void pointer to struct dvec *ds, the scale vector 指向 dvec 结构体的指针，dvec 结构体包含了比例向量（如时间或频率向量）的数据。 */
} vecinfo, *pvecinfo;



/* info for the current plot */
//用来描述 ngSpice 仿真中一个绘图（plot）的整体信息的数据结构。
typedef struct vecinfoall
{
    /* the plot */
    char *name;
    char *title;
    char *date;
    char *type;
    int veccount;

    /* the data as an array of vecinfo with length equal to the number of vectors in the plot */
    pvecinfo *vecs;

} vecinfoall, *pvecinfoall;

/* to be used by ngGet_Evt_NodeInfo, returns all data of a specific node after simulation */
#ifdef XSPICE
/* a single data point */
typedef struct evt_data
{
    int           dcop;        /* t.b.d. */
    double        step;        /* simulation time */
    char          *node_value; /* one of 0s, 1s, Us, 0r, 1r, Ur, 0z, 1z, Uz, 0u, 1u, Uu */
} evt_data, *pevt_data;

/* a list of all data points of the node selected by the char* argument to ngGet_Evt_NodeInfo */
typedef struct evt_shared_data
{
    pevt_data *evt_dect; /* array of data */
    int num_steps;       /* length of the array */
} evt_shared_data, *pevt_shared_data;
#endif


/* callback functions
addresses received from caller with ngSpice_Init() function
*/






/* sending output from stdout, stderr to caller
总结来说，ngSpice 提供这些 typedef 是为了：
定义回调函数的确切签名，以便用户知道如何编写与库兼容的函数。
确保类型安全，使得编译器能够在编译时检查函数指针的类型是否正确，防止运行时错误。
提供清晰的文档，说明库在运行时期望什么样的输入和输出。
你的任务是根据这些签名编写实际的回调函数，并将它们的指针传递给 ngSpice 的初始化函数。这样，你的函数就会在适当的时候被 ngSpice 调用。
*/
typedef int (SendChar)(char*, int, void*);
/*
   char* string to be sent to caller output
   int   identification number of calling ngspice shared lib
   void* return pointer received from caller, e.g. pointer to object having sent the request
*/






/* sending simulation status to caller */
/*向调用方发送模拟状态*/
typedef int (SendStat)(char*, int, void*);
/*
   char* simulation status and value (in percent) to be sent to caller
   int   identification number of calling ngspice shared lib
   void* return pointer received from caller

   char* 类型的字符串，包含仿真的状态信息或进度值。
   int 类型的标识符，用于区分可能存在的多个 ngSpice 实例。
   void* 类型的用户数据指针，它指向你在初始化时传递给 ngSpice 的任何额外数据。
*/





/* asking for controlled exit 
    要求控制退出*/
typedef int (ControlledExit)(int, NG_BOOL, NG_BOOL, int, void*);
/*
   int   exit status
   NG_BOOL  if true: immediate unloading dll, if false: just set flag, unload is done when function has returned
   NG_BOOL  if true: exit upon 'quit', if false: exit due to ngspice.dll error
   int   identification number of calling ngspice shared lib
   void* return pointer received from caller

   int exitStatus:
    表示退出状态。通常，这个值会指示程序退出时的状态，例如是否成功或遇到了错误。

    NG_BOOL immediateUnload:
    这是一个布尔值，指示是否应该立即卸载 ngSpice 动态链接库 (DLL)。如果为 true，则在回调函数执行完毕后立即卸载 DLL；如果为 false，则只设置一个标志，稍后再卸载 DLL。

    NG_BOOL exitDueToQuit:
    这是一个布尔值，指示退出是由于用户发出的 quit 命令，还是由于 ngSpice DLL 内部错误。如果为 true，则表示退出是因为用户请求；如果为 false，则表示退出是由于错误。

    int ident:
    一个整数，用于标识调用 ngSpice 共享库的实例。如果你的程序中有多个 ngSpice 实例，这个标识符可以帮助你区分它们。

    void* userData:
    用户数据指针，它指向你在初始化时传递给 ngSpice 的任何额外数据。你可以使用这个指针来访问程序中的其他资源或状态信息。
*/






/* send back actual vector data */
/*发回实际矢量数据*/
typedef int (SendData)(pvecvaluesall, int, int, void*);
/*
   vecvaluesall* pointer to array of structs containing actual values from all vectors
   int           number of structs (one per vector)
   int           identification number of calling ngspice shared lib
   void*         return pointer received from caller
   pvecvaluesall:
   这是一个指向 vecvaluesall 结构体数组的指针，每个结构体包含来自所有向量的实际值。vecvaluesall 结构体通常定义了仿真中每个向量的名称和对应的数据值。
   int numVecs:
   这是一个整数，表示 vecvaluesall 结构体数组中结构体的数量，也就是向量的数量
   int ident:
   这是一个整数，用于标识调用 ngSpice 共享库的实例。如果你的程序中有多个 ngSpice 实例，这个标识符可以帮助你区分它们。
   void* userData:
   这是一个 void* 类型的用户数据指针，它指向你在初始化时传递给 ngSpice 的任何额外数据。你可以使用这个指针来访问程序中的其他资源或状态信息。
   用来接收来自 ngSpice 仿真的实际向量数据的。当 ngSpice 仿真产生数据时，它会调用这个回调函数，并传递仿真结果。
*/




/* send back initailization vector data */
/*发回初始化向量数据*/
typedef int (SendInitData)(pvecinfoall, int, void*);
/*
   vecinfoall* pointer to array of structs containing data from all vectors right after initialization
   int         identification number of calling ngspice shared lib
   void*       return pointer received from caller
   这是一个指向 vecinfoall 结构体的指针，该结构体包含了绘图的信息和指向向量信息的指针数组。
   int ident:
   这是一个整数，用于标识调用 ngSpice 共享库的实例。如果你的程序中有多个 ngSpice 实例，这个标识符可以帮助你区分它们。
   void* userData:
    这是一个 void* 类型的用户数据指针，它指向你在初始化时传递给 ngSpice 的任何额外数据。你可以使用这个指针来访问程序中的其他资源或状态信息。
   用于接收来自 ngSpice 仿真的初始向量数据。这个回调函数在仿真初始化阶段被调用，它提供了关于当前绘图（plot）的信息，包括绘图的名称、标题、日期、类型以及包含的向量数量。
   此外，它还提供了一个指向 vecinfo 结构体数组的指针，这个数组包含了绘图中每个向量的详细信息。
*/


/* indicate if background thread is running */
typedef int (BGThreadRunning)(NG_BOOL, int, void*);
/*
   NG_BOOL        true if background thread is running
   int         identification number of calling ngspice shared lib
   void*       return pointer received from caller

   NG_BOOL running:
   一个布尔值，指示后台线程是否正在运行。如果值为 true，则表示后台线程当前正在运行；如果为 false，则表示后台线程已停止
   int ident:
   一个整数，用于标识调用 ngSpice 共享库的实例。如果你的程序中有多个 ngSpice 实例，这个标识符可以帮助你区分它们。
   void* userData:
   这是一个 void* 类型的用户数据指针，它指向你在初始化时传递给 ngSpice 的任何额外数据。你可以使用这个指针来访问程序中的其他资源或状态信息。
   用来通知用户程序 ngSpice 的后台线程运行状态的。
*/

/* callback functions
   addresses received from caller with ngSpice_Init_Sync() function
*/

/* ask for VSRC EXTERNAL value */
typedef int (GetVSRCData)(double*, double, char*, int, void*);
/*
   double*     return voltage value
   double      actual time
   char*       node name
   int         identification number of calling ngspice shared lib
   void*       return pointer received from caller
*/

/* ask for ISRC EXTERNAL value */
typedef int (GetISRCData)(double*, double, char*, int, void*);
/*
   double*     return current value
   double      actual time
   char*       node name
   int         identification number of calling ngspice shared lib
   void*       return pointer received from caller
*/

/* ask for new delta time depending on synchronization requirements */
typedef int (GetSyncData)(double, double*, double, int, int, int, void*);
/*
   double      actual time (ckt->CKTtime)
   double*     delta time (ckt->CKTdelta)
   double      old delta time (olddelta)
   int         redostep (as set by ngspice)
   int         identification number of calling ngspice shared lib
   int         location of call for synchronization in dctran.c
   void*       return pointer received from caller
*/

#ifdef XSPICE
/* callback functions
addresses received from caller with ngSpice_Init_Evt() function
*/

/* Upon time step finished, called per node */
typedef int (SendEvtData)(int, double, double, char *, void *, int, int, int, void*);
/*
   int         node index
   double      step, actual simulation time
   double      dvalue, a real value for specified structure component for plotting purposes
   char        *svalue, a string value for specified structure component for printing
   void        *pvalue, a binary data structure
   int         plen, size of the *pvalue structure
   int         mode, the mode (op, dc, tran) we are in
   int         ident, identification number of calling ngspice shared lib
   void*       return pointer received from caller
*/

/* Upon initialization, called once per event node
   To build up a dictionary of nodes */
typedef int (SendInitEvtData)(int, int, char*, char*, int, void*);
/*
   int         node index
   int         maximum node index, number of nodes
   char*       node name
   char*       udn-name, node type
   int         identification number of calling ngspice shared lib
   void*       return pointer received from caller
*/
#endif

/* ngspice initialization,
printfcn: pointer to callback function for reading printf, fprintf
statfcn: pointer to callback function for the status string and percent value
ControlledExit: pointer to callback function for setting a 'quit' signal in caller
SendData: pointer to callback function for returning data values of all current output vectors
SendInitData: pointer to callback function for returning information of all output vectors just initialized
BGThreadRunning: pointer to callback function indicating if workrt thread is running
userData: pointer to user-defined data, will not be modified, but
          handed over back to caller during Callback, e.g. address of calling object


 ngspice 初始化，
 printfcn：指向回调函数的指针，用于读取 printf，fprintfstatfcn：指向状态字符串和百分比值的回调函数的指针
 ControlledExit：指向回调函数的指针，用于在调用方中设置“退出”信号 SendData：指向回调函数的指针，用于返回所有当前输出向量的数据值
 SendInitData：指向回调函数的指针，用于返回刚刚初始化的所有输出向量的信息
 BGThreadRunning：指向回调函数的指针，指示 workrt 线程是否正在运行 userData：指向用户定义数据的指针，不会被修改，而是在回调期间交还给调用方，例如调用对象的地址
*/
IMPEXP
int  ngSpice_Init(SendChar* printfcn, SendStat* statfcn, ControlledExit* ngexit,
                  SendData* sdata, SendInitData* sinitdata, BGThreadRunning* bgtrun, void* userData);

/* initialization of synchronizing functions
vsrcdat: pointer to callback function for retrieving a voltage source value from caller
isrcdat: pointer to callback function for retrieving a current source value from caller
syncdat: pointer to callback function for synchronization
ident: pointer to integer unique to this shared library (defaults to 0)
userData: pointer to user-defined data, will not be modified, but
          handed over back to caller during Callback, e.g. address of calling object.
          If NULL is sent here, userdata info from ngSpice_Init() will be kept, otherwise
          userdata will be overridden by new value from here.
同步函数的初始化
vsrcdat：指向回调的指针 用于从调用方检索电压源值的函数 
isrcdat：指向回调函数的指针，用于从调用方检索当前源值 
syncdat：指向用于同步的回调函数的指针 
ident：指向此共享库唯一整数的指针（默认为 0） 
userData：指向用户定义数据的指针，不会被修改，而是在回调期间交还给调用方， 例如，调用对象的地址。如果在此处发送 NULL，则将保留 ngSpice_Init（） 中的用户数据信息，否则 userdata 将被此处的新值覆盖。

*/
IMPEXP
int  ngSpice_Init_Sync(GetVSRCData *vsrcdat, GetISRCData *isrcdat, GetSyncData *syncdat, int *ident, void *userData);

/* Caller may send ngspice commands to ngspice.dll.
Commands are executed immediately */
IMPEXP
int  ngSpice_Command(char* command);

/* get info about a vector */
IMPEXP
pvector_info ngGet_Vec_Info(char* vecname);

#ifdef XSPICE
/* Set the input path for files loaded by code models.
   If NULL is sent, return the current Infile_Path. */
IMPEXP
char* ngCM_Input_Path(const char* path);

/* get info about the event node vector */
IMPEXP
pevt_shared_data ngGet_Evt_NodeInfo(char* nodename);

/* get a list of all event nodes */
IMPEXP
char** ngSpice_AllEvtNodes(void);

/* initialization of XSPICE callback functions 
sevtdata: data for a specific event node at time 'step'
sinitevtdata: single line entry of event node dictionary (list)
userData: pointer to user-defined data, will not be modified, but
handed over back to caller during Callback, e.g. address of calling object */
IMPEXP
int  ngSpice_Init_Evt(SendEvtData* sevtdata, SendInitEvtData* sinitevtdata, void* userData);
#endif


/* send a circuit to ngspice.dll
   The circuit description is a dynamic array
   of char*. Each char* corresponds to a single circuit
   line. The last-but-one entry of the array has to be a .end card,
   followed by the last entry NULL. */
IMPEXP
int ngSpice_Circ(char** circarray);


/* return to the caller a pointer to the name of the current plot */
IMPEXP
char* ngSpice_CurPlot(void);


/* return to the caller a pointer to an array of all plots created
so far by ngspice.dll */
IMPEXP
char** ngSpice_AllPlots(void);


/* return to the caller a pointer to an array of vector names in the plot
named by plotname */
IMPEXP
char** ngSpice_AllVecs(char* plotname);

/* returns TRUE if ngspice is running in a second (background) thread */
IMPEXP
NG_BOOL ngSpice_running(void);

/* set a breakpoint in ngspice */
IMPEXP
NG_BOOL ngSpice_SetBkpt(double time);


#ifdef __cplusplus
}
#endif

#endif
