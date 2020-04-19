#include "EchoClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
   QApplication a(argc, argv);
   EchoClient w;
   w.show();
   return a.exec();
}
