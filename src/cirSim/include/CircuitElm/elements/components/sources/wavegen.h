/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "clock-base.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class LibraryItem;

class WaveGen : public ClockBase
{
    public:
        WaveGen( std::string type, std::string id );
        ~WaveGen();
        
        enum wave_type {
            Sine = 0,
            Saw,
            Triangle,
            Square,
            Random,
            Wav
        };

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual bool setPropStr( std::string prop, std::string val ) override;

        void updateStep() override{;}
        void initialize() override;
        virtual void stamp() override;
        virtual void runEvent() override;

        int minSteps() { return m_minSteps; }
        void setMinSteps( int steps );

        double duty() { return m_duty; }
        void setDuty( double duty );

        double phaseShift() { return m_phaseShift; }
        void setPhaseShift( double p ) { m_phaseShift = p; }

        std::string waveType() { return m_waveTypeStr; }
        void setWaveType( std::string type );

        double semiAmpli() { return m_voltage/2; }
        void setSemiAmpli( double v );

        double midVolt() { return m_voltBase; }
        void setMidVolt( double v );

        std::string fileName() { return m_fileName; }
        void setFile( std::string fileName );

        bool bipolar() { return m_bipolar; }
        void setBipolar( bool b );

        bool floating() { return m_floating; }
        void setFloating( bool f );

        virtual void setFreq( double freq ) override;

        void setLinkedValue( double v, int i=0 ) override;

    private:
        void genSine();
        void genSaw();
        void genTriangle();
        void genSquare();
        void genRandom();
        void genWav();

        void udtProperties();

        double normalize( double data );
        double degreesToRadians(double degrees);

        // 辅助函数，用于将小端字节序数据转换为当前系统的字节序
        template <typename T>
        T convertFromLittleEndian(const char* buffer);

        bool m_bipolar;          //是否是双极性
        bool m_floating;         //是否处于浮空状态

        wave_type m_waveType;
        std::string m_waveTypeStr;

        double m_duty;           //占空比。
        double m_vOut;           //当前输出电压值(比例值，可以乘以实际电压值换算出时间电压)。
        double m_voltBase;       //直流偏置
        double m_lastVout;       //上一个输出电压值
        double m_halfW;          //半个波长或周期的一半
        double m_time;           //当前时间或时刻
        double m_phaseShift;     //波形的相移量。
        double m_phaseTime;
        
        // int      m_steps;        //波形生成或处理的步骤数。
        uint64_t m_minSteps;
        uint64_t m_eventTime;

        //音频相关
        uint32_t m_index;
        uint16_t m_audioFormat;
        uint16_t m_numChannels;
        uint32_t m_sampleRate;
        uint16_t m_blockSize;
        uint16_t m_bitsPerSample;

        double m_maxValue;
        double m_minValue;
        double m_mult;
        std::vector<double> m_data;
        std::string m_fileName;

        IoPin* m_gndPin;

        std::vector<std::string> m_waves;
//        QPixmap* m_wavePixmap;
};

template<typename T>
inline T WaveGen::convertFromLittleEndian(const char* buffer)
{
    T value;
    std::copy(buffer, buffer + sizeof(T), reinterpret_cast<char*>(&value));

    // 检测系统是否为大端字节序
    const uint16_t endianTest = 0x1;
    const bool isBigEndian = (*reinterpret_cast<const uint8_t*>(&endianTest) == 0);

    if (isBigEndian)
    {
        // 如果系统是大端字节序，需要反转字节
        std::reverse(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + sizeof(T));
    }

    return value;
}
