/**
Данные набора элементов
*/
#ifndef NODE_SET_H
#define NODE_SET_H

#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>

class NodeData;


/** @brief Границы сцены */
struct Borders
{
    double top;
    double bottom;
    double left;
    double right;
};

class NodeSet
{
public:
    NodeSet();
    ~NodeSet();

    /** @brief Добавление элемента
        @param x - горизонтальная координата
        @param y - вертикальная координата
        @return элемент */
    NodeData* AddNode(double x, double y);

    /** @brief Удаление элемента
        @param node элемент */
    void RemvoeNode(NodeData *node);

    /** @brief Установка границ сцены */
    void SetBorders(double top, double bottom, double left, double right);

    /** @brief Получение границ сцены */
    Borders GetBorders();

    /** @brief Получение списка элементов */
    std::list< NodeData* > *GetNodeList();

    /** @brief Запуск потока вычислений координат */
    void StartCalculatonThread();

    std::mutex              m_mutexReady;
    std::mutex              m_mutexNeed;
    std::condition_variable m_dataReady;    //!< условие "данные готовы"
    std::condition_variable m_needData;     //!< условие "нужны данные"

    std::thread             *m_calcThread;  //!< поток вычислений

    bool                    m_stopFlag;     //!< флаг окончания вычислений координат
    bool                    m_threadStopped; //!< флаг завершения потока вычислений координат

private:

    /** @brief Расчет следующего положения всех элементов набора */
    void CalculateNextPosition();

    /** @brief Цикл расчета следующего положения всех элементов набора (для вызова в отдельном потоке) */
    void CalculationLoop();

    std::list< NodeData* > m_nodeList;  //!< список элементов
    Borders borders;                    //!< границы сцены
};

#endif // NODE_SET_H