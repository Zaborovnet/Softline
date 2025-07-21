#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.setWindowTitle("Directory Quota Manager");
  w.show();
  return a.exec();
}
