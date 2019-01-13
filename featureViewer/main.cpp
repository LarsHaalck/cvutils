#include <QApplication>

#include "mainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    cvutils::MainWindow window;
    window.show();

    return app.exec();
}
