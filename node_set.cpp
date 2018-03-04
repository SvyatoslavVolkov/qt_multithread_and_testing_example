/**
Данные набора элементов
*/
#include "node_set.h"
#include "node_data.h"

NodeSet::NodeSet()
    : m_stopFlag(false)
    , m_threadStopped(false)
    , m_calcThread(0)
{ }

NodeSet::~NodeSet()
{
    while (!m_threadStopped)
    {
        std::lock_guard< std::mutex > lkNeed(m_mutexNeed);   ///////////////// THREAD
        std::unique_lock< std::mutex > lkReady(m_mutexReady);   ///////////////// THREAD
        m_dataReady.wait(lkReady, []{ return true; });   ///////////////// THREAD

        m_stopFlag = true;


        lkReady.unlock();   ///////////////// THREAD
        m_needData.notify_all();   ///////////////// THREAD
    }

    if (!m_nodeList.empty())
    {
        for (std::list< NodeData* >::iterator it = m_nodeList.begin(); it != m_nodeList.end(); it++)
            delete (*it);
    }
}

NodeData* NodeSet::AddNode(double x, double y)
{
    NodeData* node = new NodeData(this);

    m_nodeList.push_back(node);

    node->SetPosition(x, y);

    return node;
}

void NodeSet::RemvoeNode(NodeData *node)
{
    m_nodeList.remove(node);
    delete node;
}

std::list< NodeData* > * NodeSet::GetNodeList()
{
    return &m_nodeList;
}

void NodeSet::SetBorders(double top, double bottom, double left, double right)
{
    borders.top = top;
    borders.bottom = bottom;
    borders.left = left;
    borders.right = right;
}

Borders NodeSet::GetBorders()
{
    return borders;
}

void NodeSet::CalculateNextPosition()
{
    if (m_nodeList.empty())
        return;

    std::list< NodeData* >::iterator it;

    for (it = m_nodeList.begin(); it != m_nodeList.end(); it++)
        (*it)->CalculateForces();

    for (it = m_nodeList.begin(); it != m_nodeList.end(); it++)
        (*it)->Advance();
}

void NodeSet::CalculationLoop()
{
    std::unique_lock< std::mutex > lockNeedStart(m_mutexNeed);
    m_needData.wait(lockNeedStart, []{ return true; });
    lockNeedStart.unlock();
    
    while (!m_stopFlag)
    {
        std::unique_lock< std::mutex > lockNeed(m_mutexNeed);

        m_needData.wait(lockNeed);

        if (m_stopFlag)
            break;

        m_mutexReady.lock();

        CalculateNextPosition();

        m_mutexReady.unlock();

        lockNeed.unlock();

        m_dataReady.notify_all();
    }
    m_threadStopped = true;
}

void NodeSet::StartCalculatonThread()
{
    if (!m_calcThread)
    {
        m_calcThread = new std::thread(&NodeSet::CalculationLoop, this);
        m_calcThread->detach();
    }
}