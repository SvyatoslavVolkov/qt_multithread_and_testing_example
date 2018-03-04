/**
Данные элемента
*/
#ifndef NODE_DATA_H
#define NODE_DATA_H
#include <list>

class NodeSet;

class NodeData
{
public:
    NodeData(NodeSet *nodes);
    ~NodeData();

    /** @brief Расчет сил притяжения, действующих на элемент */
    void CalculateForces();

    /** @brief Координаты текущего элемента в системе координат с центром в элементе otherNode */
    std::pair< double, double > MapToNode(NodeData *otherNode);

    /** @brief Установка координат элемента */
    void SetPosition(double x, double y);

    /** @brief Получение координат элемента */
    std::pair< double, double > GetPosition();

    /** @brief Получение координат элемента с учетом перемещения под действием притяжения */
    std::pair< double, double > GetNewPosition();

    /** @brief Перемещение элемента под действием притяжения */
    bool Advance();

private:
    std::pair< double, double > m_position;     //!< текущие координаты элемента
    std::pair< double, double > m_newPosition;  //!< координаты элемента с учетом перемещения под действием притяжения
    NodeSet                     *m_nodes;       //!< графический элемент
};


#endif // NODE_DATA_H