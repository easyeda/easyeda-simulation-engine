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

class CircMatrix
{
    typedef std::vector<double>      d_vector_t;  
    typedef std::vector<double*>     dp_vector_t;
    struct DenseMat {
        int n=0;
        std::vector<double> data;
        DenseMat() = default;
        DenseMat(int n_): n(n_), data(n_>0 ? (size_t)n_*n_ : 0, 0.0) {}
        void resize(int n_){ n=n_; data.assign((size_t)n*n, 0.0); }
        inline double &operator()(int r,int c){ return data[(size_t)r*n + c]; }
        inline const double &operator()(int r,int c) const{ return data[(size_t)r*n + c]; }
        inline double *rowPtr(int r){ return data.data() + (size_t)r*n; }
        inline const double *rowPtr(int r) const { return data.data() + (size_t)r*n; } 
    };
    typedef std::vector<dp_vector_t> dp_matrix_t;

public:
    CircMatrix();
    ~CircMatrix();

    static CircMatrix* self() { return m_pSelf; }

    void createMatrix( std::vector<eNode*> &eNodeList );
    bool solveMatrix();

    inline void stampDiagonal( int group, int n, double value )
    {
        m_admitChanged[group] = true;
        m_circMatrix[n-1][n-1] = value;     
    }

    inline void stampMatrix( int row, int col, double value )
    {
        m_circMatrix[row-1][col-1] = value;     
    }

    inline void stampCoef( int group, int row, double value )
    {
        m_currChanged[group] = true;
        m_coefVect[row-1] = value;
    }

private:
    static CircMatrix* m_pSelf;

    void analyze();
    void addConnections( int enodNum, std::vector<int>* nodeGroup, std::vector<int>* allNodes );

    // optimized implementations
    void factorMatrix( int n, int group );   
    bool luSolve( int n, int group );        

    int m_numEnodes;
    std::vector<eNode*>* m_eNodeList;

    std::vector<dp_matrix_t> m_aList;  
    std::vector<DenseMat>  m_aFaList;  
    std::vector<dp_vector_t> m_bList;   

    std::vector<std::vector<int>> m_permutations; 

    std::vector<bool>    m_admitChanged;
    std::vector<bool>    m_currChanged;
    std::vector<eNode*>*       m_eNodeActive;
    std::vector<std::vector<eNode*>> m_eNodeActList;

    std::vector<std::vector<double>> m_circMatrix; 
    std::vector<double> m_coefVect;
};


