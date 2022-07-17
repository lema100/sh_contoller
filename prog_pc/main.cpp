#include "mainwindow.h"
#include "thread.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QStringList paths = QCoreApplication::libraryPaths();
	paths.append(".");
	paths.append("platforms");
	QCoreApplication::setLibraryPaths(paths);

	QApplication a(argc, argv);
	MainWindow w;
	thread *th1 = new thread();

	qRegisterMetaType<port_parametrs>("port_parametrs");
	qRegisterMetaType<device_parametrs>("device_parametrs");
	qRegisterMetaType<device_data>("device_data");
	qRegisterMetaType<uint16_t>("uint16_t");
	qRegisterMetaType<uint32_t>("uint32_t");
	qRegisterMetaType<QByteArray>("QByteArray");
	qRegisterMetaType<QMap<QDateTime, int>>("QMap<QDateTime, int>");

	QObject::connect(&w, SIGNAL(connect_to(port_parametrs)), th1, SLOT(connect_to(port_parametrs)));
	QObject::connect(&w, SIGNAL(disconect()), th1, SLOT(disconect()));
	QObject::connect(&w, SIGNAL(cmd(uint32_t, uint32_t)), th1, SLOT(cmd(uint32_t, uint32_t)));
	QObject::connect(&w, SIGNAL(write_parametrs(device_parametrs)), th1, SLOT(write_parametrs(device_parametrs)));
	QObject::connect(&w, SIGNAL(burn_bin(QByteArray)), th1, SLOT(burn_bin(QByteArray)));

	QObject::connect(th1, SIGNAL(device_read(device_data)), &w, SLOT(device_read(device_data)));
	QObject::connect(th1, SIGNAL(device_param(device_parametrs)), &w, SLOT(device_param(device_parametrs)));
	QObject::connect(th1, SIGNAL(Print_to_textBrowser(QString)), &w, SLOT(Print_to_textBrowser(QString)));

	w.show();

	th1->start();

	return a.exec();
}
