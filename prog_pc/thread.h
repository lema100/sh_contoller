#ifndef THREAD_H
#define THREAD_H

#include "../prog_mcu/User/update.h"
#include "modbus/modbus.h"
#include <QThread>
#include <QTimer>
#include <math.h>
#include "main.h"

class thread : public QThread
{
Q_OBJECT

private:
	port_parametrs modbus_port;

	uint16_t connection_status;
	uint32_t _cmd, _cmd_arg;
	QByteArray _bin;

	int mb_err(modbus_t * my_modbus, QString err);

public:
	thread(QObject * parent = nullptr) : QThread(parent)
	{
		moveToThread(this);
	}
	virtual void run();

signals:
	void device_param(device_parametrs par);
	void device_read(device_data data);
	void Print_to_textBrowser(const QString &text);

public slots:
	int m_Timeout();

	void connect_to(port_parametrs par);
	void disconect();

	void cmd(uint32_t cmd, uint32_t cmd_arg);
	void write_parametrs(device_parametrs par);
	void burn_bin(QByteArray bin);
};

#endif // THREAD_H
