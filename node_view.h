/*
    Графическое представление элемента (за основу взят класс Node из проекта Elastic Nodes примеров Qt)
    */
#ifndef NODE_VIEW_H
#define NODE_VIEW_H


#include <QGraphicsItem>
#include <QList>

class GraphWidget;
class QGraphicsSceneMouseEvent;
class NodeData;

class NodeView : public QGraphicsItem
{
public:
    NodeView(GraphWidget *graphWidget);
    ~NodeView();

    /** @brief Связь графического элемента с объектом данных
        @param nodeData - объект данных */
    void SetData(NodeData *nodeData);

    /** @brief Получение объекта данных */
    NodeData* GetData();

    /** @brief Установка нового положения графического элемента (newPos) из объекта данных */
    void UpdateData();

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    /** @brief перемещение графического элемента */
    bool advance();


    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    QPointF     newPos;     //!< новое положение графического элемента
    GraphWidget *graph;     //!< виджет отображения    
    NodeData    *nodeData;  //!< данные
};

#endif // NODE_VIEW_H