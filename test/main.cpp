#include <QApplication>
#include <QtTest/QtTest>
#include "testgravity.h"

int main(int argc, char *argv[])
{
    freopen("testing.log", "w", stdout);

    QApplication a(argc, argv);

    QTest::qExec(new TestGravity, argc, argv);

    return 0;
}

//QTEST_MAIN(TestGravity)