#include "EchoServer.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
   QApplication a(argc, argv);
   EchoServer w;
   w.show();
   return a.exec();
}
