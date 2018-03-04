#ifndef TEST_GRAVITY_H
#define TEST_GRAVITY_H

#include <QObject>

class NodeView;
class GraphWidget;

class TestGravity : public QObject
{
    Q_OBJECT
public:
    explicit TestGravity(QObject *parent = 0);
private:
    QList< NodeView* > GetItems(GraphWidget *widget);
private slots:
    void add();
    void remove();
};

#endif // TEST_GRAVITY_H