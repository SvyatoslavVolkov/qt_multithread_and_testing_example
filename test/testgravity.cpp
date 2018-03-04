#include <QtTest/QtTest>
#include "testgravity.h"
#include "graphwidget.h"
#include "node_view.h"


TestGravity::TestGravity(QObject *parent) : QObject(parent)
{}

QList< NodeView* > TestGravity::GetItems(GraphWidget *widget)
{
    QList<NodeView *> nodes;
    foreach(QGraphicsItem *item, widget->scene()->items())
    {
        if (NodeView *node = qgraphicsitem_cast<NodeView *>(item))
            nodes << node;
    }
    return nodes;
}

void TestGravity::add()
{
    GraphWidget widget;

    int itemsCountBefore = widget.scene()->items().size();

    QPointF mousePoint(0.0, 0.0);

    QTest::mouseRelease(widget.viewport(), Qt::RightButton, Qt::NoModifier, widget.mapFromScene(mousePoint));

    int itemsCountAfter = widget.scene()->items().size();

    QCOMPARE(itemsCountBefore + 1, itemsCountAfter);

}

void TestGravity::remove()
{
    GraphWidget widget;

    QList<NodeView *> nodes = GetItems(&widget);

    if (nodes.empty())
        return;

    int itemsCountBefore = nodes.size();

    widget.RemoveNode(nodes.at(0));

    nodes = GetItems(&widget);

    int itemsCountAfter = nodes.size();

    QCOMPARE(itemsCountBefore, itemsCountAfter + 1);

}