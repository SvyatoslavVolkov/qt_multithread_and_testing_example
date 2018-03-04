/*
    √рафическое представление элемента (за основу вз¤т класс Node из проекта Elastic Nodes примеров Qt)
    */
#include "node_view.h"
#include "graphwidget.h"
#include "node_data.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <math.h>

NodeView::NodeView(GraphWidget *graphWidget)
    : graph(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

NodeView::~NodeView()
{
//    delete nodeData;
}

void NodeView::SetData(NodeData *nodeData)
{
    this->nodeData = nodeData;

    qreal x = (qreal)nodeData->GetPosition().first;
    qreal y = (qreal)nodeData->GetPosition().second;

    setPos(x, y);
}

NodeData* NodeView::GetData()
{
    return nodeData;
}

void NodeView::UpdateData()
{
    if (!scene() || scene()->mouseGrabberItem() == this) // если текущий шарик удерживаетс¤ мышью
    {
        newPos = pos();
        nodeData->SetPosition((double)newPos.x(), (double)newPos.y());
        return;
    }

    std::pair< double, double > pos = nodeData->GetPosition();

    newPos = QPointF((qreal)pos.first, (qreal)pos.second);
}

bool NodeView::advance()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}

QRectF NodeView::boundingRect() const
{
    qreal adjust = 2;
    return QRectF(-10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath NodeView::shape() const
{
    QPainterPath path;

    path.addEllipse(-10, -10, 20, 20);

    return path;
}

void NodeView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
    }
    else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
}

QVariant NodeView::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
    case ItemPositionHasChanged:
    {
        QPointF newPos = value.toPointF();
        nodeData->SetPosition((double)newPos.x(), (double)newPos.y());
        graph->ItemMoved();
        break;
    }
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void NodeView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    graph->MousePressedOnNode(true);

    update();

    QGraphicsItem::mousePressEvent(event);
}

void NodeView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        graph->RemoveNode(this);

        return;
    }

    update();

    QGraphicsItem::mouseReleaseEvent(event);
}