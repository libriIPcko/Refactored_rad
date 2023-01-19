#include <QCoreApplication>
#include <radar_v2.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    radar_v2 rad;
    rad.init(rad.cfgFile_path);

    return a.exec();
}
