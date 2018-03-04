/*
    Виджет отображения (за основу взят класс GraphWidget из проекта Elastic Nodes примеров Qt)
    */
#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView> 
#include <QThread>
#include <vector>

class NodeView;


class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = 0);
    ~GraphWidget();

    /** @brief действия после перемещения элемента */
    void ItemMoved();

    /** @brief Установка/сброс флага нажания кнопки мыши над элементом */
    void MousePressedOnNode(bool mousePressedOnNode);

    /** @brief Получение флага нажания кнопки мыши над элементом */
    bool IsMousePressedOnNode();

    /** @brief Добавление элемента
        @param x - горизонтальная координата
        @param y - вертикальная координата
        @return элемент */
    NodeView* AddNode(qreal x, qreal y);

    /** @brief Удаление элемента
        @param node элемент */
    void RemoveNode(NodeView *node);

    /** @brief Добавление заданого числе элементов со случайными координатами
        @param count число элементов */
    void AddNodes(unsigned int count);

    /** @brief Запуск таймера */
    void StartTimer();

    /** @brief Остановка таймера */
    void StopTimer();

protected:
    void timerEvent(QTimerEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);

private:

    class		PrivateData;
    PrivateData	*m_pData;       //!< данные класса
};

#endif // GRAPHWIDGET_H