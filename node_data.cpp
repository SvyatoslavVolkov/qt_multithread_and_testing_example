#include "node_data.h"
#include "node_set.h"
#include <list>
#include <algorithm>

NodeData::NodeData(NodeSet *nodes) : m_nodes(nodes)
{
}

NodeData::~NodeData()
{

}

void NodeData::SetPosition(double x, double y)
{
    m_position.first = x;
    m_position.second = y;
}

std::pair< double, double > NodeData::GetPosition()
{
    return m_position;
}
std::pair< double, double > NodeData::GetNewPosition()
{
    return m_newPosition;
}

bool NodeData::Advance()
{
    if (m_newPosition == m_position)
        return false;

    m_position = m_newPosition;

    return true;
}

std::pair< double, double > NodeData::MapToNode(NodeData *otherNode)
{
    std::pair< double, double > vec(0.0, 0.0);

    if (!otherNode || this == otherNode)
        return vec;

    vec.first = this->GetPosition().first - otherNode->GetPosition().first;
    vec.second = this->GetPosition().second - otherNode->GetPosition().second;

    return vec;
}


void NodeData::CalculateForces()
{
    double xVel = 0.0;
    double yVel = 0.0;

    std::list< NodeData* > *nodeList = m_nodes->GetNodeList();

    for (std::list< NodeData* >::iterator it = nodeList->begin(); it != nodeList->end(); it++)
    {
        NodeData* currentNode = (*it);
        if (this == currentNode)
            continue;

        std::pair<double, double> vec = MapToNode(currentNode);

        double dx = vec.first;
        double dy = vec.second;

        double rr = dx * dx + dy * dy;		// квадрат расстояния между элементами
        double r = sqrt(rr);					// расстояние между элементами
        double f = 1.0 / r - 1.0 / r / r;	// "сила" притяжения

        if (rr > 0)
        {
            // компонента "силы" по соответствующему измерению прямо пропорциональна компоненте расстояния
            // приращения координат за единицу времени:
            xVel -= (dx / 5.0) * f;
            yVel -= (dy / 5.0) * f;
        }
    }

    m_newPosition.first = m_position.first + xVel;
    m_newPosition.second = m_position.second + yVel;

    Borders borders = m_nodes->GetBorders();

    m_newPosition.first = std::min(std::max(m_newPosition.first, borders.left + 10), borders.right - 10);
    m_newPosition.second = std::min(std::max(m_newPosition.second, borders.top + 10), borders.bottom - 10);

}
