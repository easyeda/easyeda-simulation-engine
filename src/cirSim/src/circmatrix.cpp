/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "circmatrix.h"
#include "simulator.h"

CircMatrix* CircMatrix::m_pSelf = 0l;

CircMatrix::CircMatrix()
{
    m_pSelf = this;
    m_numEnodes = 0;
}


CircMatrix::~CircMatrix(){}

void CircMatrix::createMatrix( std::vector<eNode*> &eNodeList )
{
    m_eNodeList = &eNodeList;
    m_numEnodes = (int)eNodeList.size();

    m_circMatrix.clear();
    m_coefVect.clear();

    m_circMatrix.resize( m_numEnodes , std::vector<double>( m_numEnodes , 0.0 ) );
    m_coefVect.resize( m_numEnodes , 0.0 );

    std::cout <<"\nInitializing Matrix: "<< m_numEnodes << " eNodes\n";
    analyze();
}

void CircMatrix::addConnections( int enodNum, std::vector<int>* nodeGroup, std::vector<int>* allNodes )
{
    nodeGroup->push_back( enodNum );
    auto it = std::find(allNodes->begin(), allNodes->end(), enodNum);
    if (it != allNodes->end()) allNodes->erase(it);
    eNode* enod = (*m_eNodeList)[enodNum - 1];
    enod->setSingle( false );

    std::vector<int> cons = enod->getConnections();
    for( int nodeNum : cons )
    {
        if( nodeNum == 0 ) continue;
        if(std::find(nodeGroup->begin(), nodeGroup->end(), nodeNum) == nodeGroup->end()) addConnections( nodeNum, nodeGroup, allNodes );
    }
}

void CircMatrix::analyze()
{
    std::cout << "entering analyze" << std::endl;

    std::vector<int> allNodes;
    for( int i=0; i<m_numEnodes; i++ ) allNodes.push_back( i+1 );

    m_aList.clear();
    m_aFaList.clear();
    m_bList.clear();
    m_eNodeActList.clear();
    m_permutations.clear();
    int group = 0;
    int singleNode = 0;

    while( !allNodes.empty() )
    {
        std::vector<int> nodeGroup;
        addConnections( allNodes.front(), &nodeGroup, &allNodes );

        int numEnodes = (int)nodeGroup.size();
        if( numEnodes==1 )
        {
            eNode* enod = m_eNodeList->at( nodeGroup[0]-1 );
            enod->setSingle( true );
            singleNode++;
        }
        else
        {
            dp_matrix_t a;
            DenseMat ap; // temp container not used directly for pointers
            dp_vector_t b;
            std::vector<eNode*> eNodeActive;

            a.resize( numEnodes , dp_vector_t( numEnodes , nullptr ) );
            b.resize( numEnodes , nullptr );

            int ny=0;
            for( int y=0; y<m_numEnodes; ++y )
            {
                if(std::find(nodeGroup.begin(), nodeGroup.end(), y + 1) == nodeGroup.end()) 
                    continue;
                int nx=0;
                for( int x=0; x<m_numEnodes; ++x )
                {
                    if(std::find(nodeGroup.begin(), nodeGroup.end(), x + 1) == nodeGroup.end()) 
                        continue;
                    a[nx][ny] = &(m_circMatrix[x][y]);
                    nx++;
                }
                b[ny] = &(m_coefVect[y]);
                eNode* node = m_eNodeList->at(y);
                node->setNodeGroup( group );
                eNodeActive.push_back( node );
                ny++;
            }

            m_aList.push_back( std::move(a) );
            m_aFaList.emplace_back( numEnodes ); // reserve a flattened matrix of size n*n
            m_bList.push_back( std::move(b) );
            m_eNodeActList.push_back( std::move(eNodeActive) );
            m_permutations.emplace_back();
            group++;
        }
    }

    m_admitChanged.resize( group, true );
    m_currChanged.resize(  group, true );

    std::cout <<"CircMatrix::solveMatrix "<<group<<" Circuits\n";
    std::cout <<"CircMatrix::solveMatrix "<<singleNode<<" Single Nodes\n";
}

bool CircMatrix::solveMatrix()
{
    bool ok = true;
    for( int i=0; i<m_bList.size(); ++i )
    {
        if( !m_admitChanged[i] && !m_currChanged[i] ) continue;

        m_eNodeActive = &(m_eNodeActList[i]);
        int n = (int)m_eNodeActive->size();

        if( m_admitChanged[i] ) factorMatrix( n, i );
        if( !luSolve( n, i ) ) ok = false;

        m_currChanged[i]  = false;
        m_admitChanged[i] = false;
    }
    return ok;
}

void CircMatrix::factorMatrix( int n, int group ) // Factor matrix into Lower/Upper triangular
{
    dp_matrix_t &apPtrs = m_aList[group];
    DenseMat &LU = m_aFaList[group];
    std::vector<int> &perm = m_permutations[group];
    perm.resize(n);

    for(int row=0; row<n; ++row){
        double* dst = LU.rowPtr(row);
        for(int col=0; col<n; ++col){
            dst[col] = *(apPtrs[row][col]);
        }
        perm[row] = row;
    }

    for(int k=0; k<n; ++k){
        // Find pivot
        int piv = k;
        double maxv = fabs(LU(k,k));
        for(int i=k+1;i<n;++i){
            double v = fabs(LU(i,k));
            if(v > maxv){ maxv = v; piv = i; }
        }
        if(maxv == 0.0) {
            // singular; continue to next column (keep zeros)
            continue;
        }
        if(piv != k){
            // swap rows k and piv in LU
            for(int j=0;j<n;++j) std::swap(LU(k,j), LU(piv,j));
            std::swap(perm[k], perm[piv]);
        }
        double diag = LU(k,k);
        double *rowk = LU.rowPtr(k);
        for(int i=k+1;i<n;++i){
            double *rowi = LU.rowPtr(i);
            double mult = rowi[k] / diag;
            rowi[k] = mult; 
            for(int j=k+1;j<n;++j) rowi[j] -= mult * rowk[j];
        }
    }
}

bool CircMatrix::luSolve( int n, int group ) // Solves the system to get voltages for each node
{
    const DenseMat &LU = m_aFaList[group];
    const dp_vector_t &bp = m_bList[group];
    const std::vector<int> &perm = m_permutations[group];

    std::vector<double> b(n);
    for(int i=0;i<n;++i) b[i] = *(bp[i]);

    std::vector<double> bperm(n);
    for(int i=0;i<n;++i) bperm[i] = b[perm[i]];

    // Forward substitution Ly = bperm (L has unit diagonal, L stored in lower part of LU)
    for(int i=0;i<n;++i){
        double sum = bperm[i];
        const double *rowi = LU.rowPtr(i);
        for(int j=0;j<i;++j) sum -= rowi[j] * bperm[j];
        bperm[i] = sum;
    }

    bool isOk = true;


    // Back substitution Ux = y
    std::vector<double> x(n);
    for(int i=n-1;i>=0;--i){
        const double *rowi = LU.rowPtr(i);
        double sum = bperm[i];
        for(int j=i+1;j<n;++j) sum -= rowi[j] * x[j];
        double div = rowi[i];
        if(div == 0.0) {
            isOk = false;
        } else{
            x[i] = sum / div;
        }
    }

    // Write back voltages to nodes
    for(int i=0;i<n;++i){
        // std:: cout << 'the node of ' << i << 'volt is' << x[i] << std::endl;
        m_eNodeActive->at(i)->setVolt( x[i] );
    }
    return true;
}
