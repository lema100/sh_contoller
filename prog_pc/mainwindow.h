#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMainWindow>
#include <QtCharts>
#include <QProgressBar>
#include "main.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	const QVector<QString> ai_name = {"VCC", "AI10", "AI11", "AI14", "AI15", "AI18", "AI19"};
	QMap<int, QString> mode;

	Ui::MainWindow *ui;
	QTimer *timer;

	QVector<QPushButton *> di;
	QVector<QProgressBar *> ai;
	QVector<QPushButton *> dout;
	QVector<QComboBox *> dout_mode;
	QVector<QComboBox *> dout_safe;
	QVector<QLabel *> ai_v;

	QVector<QString> string_comport;
	port_parametrs port_param;

signals:
	void connect_to(port_parametrs par);
	void disconect();

	void cmd(uint32_t cmd, uint32_t cmd_arg);
	void write_parametrs(device_parametrs par);
	void burn_bin(QByteArray bin);

public slots:
	void set_out(bool);
	void set_out_mode(int);
	void device_param(device_parametrs par);
	void device_read(device_data data);
	void Print_to_textBrowser(const QString &text);

	void on_comboBox_activated(int index);
	void on_lineEdit_slave_par_textChanged(const QString &text);
	void on_comboBox_baudrate_par_currentTextChanged(const QString &text);
	void on_pushButton_reset_clicked(bool checked);
	void on_pushButton_check_update_clicked(bool checked);
	void on_pushButton_burn_update_clicked(bool checked);

	void timeout(void);
};

#endif // MAINWINDOW_H
