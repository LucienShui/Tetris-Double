#include "666.h"
#include <QApplication>
int main(int argc, char *argv[])
 {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Tetris!");
    w.setWindowIcon(QIcon(":/icon/game.png"));
    w.show();
    return a.exec();
}
