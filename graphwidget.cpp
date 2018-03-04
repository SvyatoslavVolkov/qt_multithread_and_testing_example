/*
Виджет отображения (за основу взят класс GraphWidget из проекта Elastic Nodes примеров Qt)
*/
#include "node_set.h"
#include "graphwidget.h"
#include "node_view.h"
#include <QRectF> 
#include <mutex>
#include <thread>
#include <math.h>


const qreal MAX_X(200.0);
const qreal MAX_Y(200.0);

class GraphWidget::PrivateData
{
public:
    PrivateData() : timerId(0)
        , mousePressedOnNode(false)
        , scene(new QGraphicsScene())
        , nodeSet(new NodeSet())
    {}

    ~PrivateData()
    {
        delete nodeSet;
        delete scene;
    }
    QGraphicsScene *scene;              //!< сцена
    int             timerId;            //!< идентификатор таймера
    bool            mousePressedOnNode; //!< флаг нажания кнопки мыши над элементом
    NodeSet         *nodeSet;           //!< набор данных
};

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), m_pData(new PrivateData())
{
    qreal maxX2 = 2 * MAX_X;
    qreal maxY2 = 2 * MAX_Y;
    m_pData->scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    m_pData->scene->setSceneRect(-MAX_X, -MAX_Y, maxX2, maxY2);
    QRectF sceneRect = m_pData->scene->sceneRect();
    m_pData->nodeSet->SetBorders((double)sceneRect.top(), (double)sceneRect.bottom(), (double)sceneRect.left(), (double)sceneRect.right());
    setScene(m_pData->scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(maxX2, maxY2);

    AddNodes(3);

    m_pData->nodeSet->StartCalculatonThread();
    //m_pData->nodeSet->dataCondition.notify_all();
    m_pData->nodeSet->m_needData.notify_all();

}

GraphWidget::~GraphWidget()
{
    StopTimer();
    delete m_pData;
}

void GraphWidget::ItemMoved()
{
    if (!m_pData->timerId)
        StartTimer();
}

void GraphWidget::MousePressedOnNode(bool mousePressedOnNode)
{
    m_pData->mousePressedOnNode = mousePressedOnNode;
}


bool GraphWidget::IsMousePressedOnNode()
{
    return m_pData->mousePressedOnNode;
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<NodeView *> nodes;
    foreach(QGraphicsItem *item, scene()->items())
    {
        if (NodeView *node = qgraphicsitem_cast<NodeView *>(item))
            nodes << node;
    }

    std::lock_guard< std::mutex > lkNeed(m_pData->nodeSet->m_mutexNeed);   ///////////////// THREAD

    std::unique_lock< std::mutex > lkReady(m_pData->nodeSet->m_mutexReady);   ///////////////// THREAD
    m_pData->nodeSet->m_dataReady.wait(lkReady, []{ return true; });   ///////////////// THREAD


    foreach(NodeView *node, nodes)
        node->UpdateData();

    lkReady.unlock();   ///////////////// THREAD
    m_pData->nodeSet->m_needData.notify_all();   ///////////////// THREAD


    bool itemsMoved = false;
    foreach(NodeView *node, nodes)
    {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved)
    {
        StopTimer();
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!IsMousePressedOnNode())
    {
        if (event->button() == Qt::RightButton)
        {
            QPointF scenePos = mapToScene(event->pos());
            AddNode(scenePos.x(), scenePos.y());
        }
    }
    else
    {
        MousePressedOnNode(false);
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
}

NodeView* GraphWidget::AddNode(qreal x, qreal y)
{
    std::lock_guard< std::mutex > lkNeed(m_pData->nodeSet->m_mutexNeed);   ///////////////// THREAD

    std::unique_lock< std::mutex > lkReady(m_pData->nodeSet->m_mutexReady);   ///////////////// THREAD
    m_pData->nodeSet->m_dataReady.wait(lkReady, []{ return true; });   ///////////////// THREAD

    NodeView *node = new NodeView(this);
    NodeData *nodeData = m_pData->nodeSet->AddNode((double)x, (double)y);

    m_pData->scene->addItem(node);

    node->SetData(nodeData);

    lkReady.unlock();   ///////////////// THREAD
    m_pData->nodeSet->m_needData.notify_all();   ///////////////// THREAD

    return node;
}

void GraphWidget::RemoveNode(NodeView *node)
{
    std::lock_guard< std::mutex > lkNeed(m_pData->nodeSet->m_mutexNeed);   ///////////////// THREAD

    std::unique_lock< std::mutex > lkReady(m_pData->nodeSet->m_mutexReady);   ///////////////// THREAD
    m_pData->nodeSet->m_dataReady.wait(lkReady, []{ return true; });   ///////////////// THREAD

    m_pData->nodeSet->RemvoeNode(node->GetData());

    lkReady.unlock();   ///////////////// THREAD
    m_pData->nodeSet->m_needData.notify_all();   ///////////////// THREAD


    delete node;
}

void GraphWidget::AddNodes(unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        qreal x = static_cast<qreal>(qrand()) / RAND_MAX * 2.0*MAX_X - MAX_X;
        qreal y = static_cast<qreal>(qrand()) / RAND_MAX * 2.0*MAX_Y - MAX_Y;

        AddNode(x, y);
    }
}

void GraphWidget::StartTimer()
{
    m_pData->timerId = startTimer(1000 / 25);
}

void GraphWidget::StopTimer()
{
    killTimer(m_pData->timerId);
    m_pData->timerId = 0;
}