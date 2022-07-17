#include "mainwindow.h"
#include "ui_mainwindow.h"

#define WIDTH_COM			55
#define HEIGHT_COM			30
#define HEIGHT_AI			150
#define WIDTH_DROP_COMBO_DO	100
#define HEIGHT_COMBO_DO		20

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setWindowTitle(windowTitle() + "   " + __DATE__);

	on_comboBox_activated(0);

	di.resize(DI_MAX);
	for (int i = 0; i < DI_MAX; i++)
	{
		di[i] = new QPushButton(QString("DI%1").arg(i));
		di[i]->setMinimumSize(WIDTH_COM, HEIGHT_COM);
		di[i]->setMaximumSize(WIDTH_COM, HEIGHT_COM);
		di[i]->setCheckable(true);
		di[i]->setEnabled(false);
		di[i]->setStyleSheet("QPushButton:checked {background-color: lightgreen; } QPushButton {background-color: lightpink; }");
		di[i]->setChecked(i % 2);
		if (i % 2)
			ui->horizontalLayout_DI2->addWidget(di[i]);
		else
			ui->horizontalLayout_DI1->addWidget(di[i]);
	}
	ui->horizontalLayout_DI1->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
	ui->horizontalLayout_DI2->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

	ai.resize(AI_MAX);
	ai_v.resize(AI_MAX);
	for (int i = 0; i < AI_MAX; i++)
	{
		ai_v[i] = new QLabel();
		ai_v[i]->setText(QString("%2\n%1V").arg((double)i * 2999 / 1000, 0, 'f', 3).arg(ai_name[i]));
		ai_v[i]->setMinimumSize(WIDTH_COM, HEIGHT_COM);
		ai_v[i]->setMaximumSize(WIDTH_COM, HEIGHT_COM);
		ai_v[i]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->horizontalLayout_AI_v->addWidget(ai_v[i]);

		ai[i] = new QProgressBar();
		ai[i]->setMinimumSize(WIDTH_COM, HEIGHT_AI);
		ai[i]->setMaximumSize(WIDTH_COM, HEIGHT_AI);
		ai[i]->setOrientation(Qt::Vertical);
		ai[i]->setAlignment(Qt::AlignCenter);
		ai[i]->setMaximum(25000);
		ai[i]->setMinimum(0);
		ai[i]->setValue(i * 3000);
		ui->horizontalLayout_AI->addWidget(ai[i]);
	}
	ui->horizontalLayout_AI->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
	ui->horizontalLayout_AI_v->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

	dout.resize(DO_MAX);
	dout_mode.resize(DO_MAX);
	dout_safe.resize(DO_MAX);

	mode[0] = "MB";
	mode[1] = "LOW";
	mode[2] = "HIGH";
	for(int i = 0; i < DI_MAX; i++)
	{
		mode[OUT_TO_IN_0 + i] = QString("DI%1").arg(i);
		mode[OUT_TO_INV_IN_0 + i] = QString("DI%1 INVERT").arg(i);
	}

	for (int i = 0; i < DO_MAX; i++)
	{
		dout[i] = new QPushButton(QString("DO%1").arg(i));
		dout[i]->setMinimumSize(WIDTH_COM, HEIGHT_COM);
		dout[i]->setMaximumSize(WIDTH_COM, HEIGHT_COM);
		dout[i]->setCheckable(true);
		dout[i]->setStyleSheet("QPushButton:checked {background-color: lightgreen; } QPushButton {background-color: lightpink; }");
		QObject::connect(dout[i], SIGNAL(clicked(bool)), this, SLOT(set_out(bool)));

		dout_safe[i] = new QComboBox(this);
		dout_mode[i] = new QComboBox(this);
		dout_safe[i]->setMinimumSize(WIDTH_COM, HEIGHT_COMBO_DO);
		dout_safe[i]->setMaximumSize(WIDTH_COM, HEIGHT_COMBO_DO);
		dout_mode[i]->setMinimumSize(WIDTH_COM, HEIGHT_COMBO_DO);
		dout_mode[i]->setMaximumSize(WIDTH_COM, HEIGHT_COMBO_DO);
		dout_safe[i]->setToolTip("Offline mode");
		dout_mode[i]->setToolTip("Online mode");
		QObject::connect(dout_safe[i], SIGNAL(currentIndexChanged(int)), this, SLOT(set_out_mode(int)));
		QObject::connect(dout_mode[i], SIGNAL(currentIndexChanged(int)), this, SLOT(set_out_mode(int)));

		for (const auto & j : mode.values())
		{
			dout_mode[i]->addItem(j);
			dout_safe[i]->addItem(j);
		}

		dout_mode[i]->view()->setMinimumWidth(WIDTH_DROP_COMBO_DO);
		dout_safe[i]->view()->setMinimumWidth(WIDTH_DROP_COMBO_DO);

		if (i % 2)
		{
			ui->horizontalLayout_DO2->addWidget(dout[i]);
			ui->horizontalLayout_DO2_safe->addWidget(dout_safe[i]);
			ui->horizontalLayout_DO2_mode->addWidget(dout_mode[i]);
		}
		else
		{
			ui->horizontalLayout_DO1->addWidget(dout[i]);
			ui->horizontalLayout_DO1_safe->addWidget(dout_safe[i]);
			ui->horizontalLayout_DO1_mode->addWidget(dout_mode[i]);
		}

	}

	ui->horizontalLayout_DO1->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
	ui->horizontalLayout_DO2->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

	timer = new QTimer;
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	timer->start(500);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::set_out_mode(int)
{
	int mode_index = dout_mode.indexOf((QComboBox * const)sender());
	int safe_index = dout_safe.indexOf((QComboBox * const)sender());
	int index = mode_index != -1 ? mode_index : safe_index;

	if (index != -1)
	{
		uint32_t arg = mode.key(dout_safe[index]->currentText());
		arg <<= 8;
		arg |= mode.key(dout_mode[index]->currentText());

		emit cmd(CMD_SET_OUT0 + index, arg);
	}

	qDebug() << "index mode" << mode_index;
	qDebug() << "index safe" << safe_index;
}

void MainWindow::set_out(bool)
{
	uint32_t arg = 0;

	for (int i = 0; i < DO_MAX; i++)
		arg |= dout[i]->isChecked() ? (1 << i) : 0;
	emit cmd(CMD_SET_OUT, arg);
}

void MainWindow::on_pushButton_reset_clicked(bool checked)
{
	(void) checked;
	emit cmd(CMD_RESET, 0);
}

void MainWindow::on_pushButton_burn_update_clicked(bool checked)
{
	(void) checked;

	auto file_name = QFileDialog::getOpenFileName(this, "Open File", "", "Binary (*.bin)");
	if (!file_name.isEmpty())
	{
		QByteArray data;

		auto file = std::make_shared<QFile>(file_name);
		file->open(QIODevice::ReadOnly);
		data = file->readAll();
		file->close();
		emit burn_bin(data);
	}
}

void MainWindow::on_pushButton_check_update_clicked(bool checked)
{
	(void) checked;
	emit cmd(CMD_CHECK_BIN, 0);
}

void MainWindow::on_lineEdit_slave_par_textChanged(const QString &text)
{
	emit cmd(CMD_WRITE_ADDRESS, text.toUInt());
}

void MainWindow::on_comboBox_baudrate_par_currentTextChanged(const QString &text)
{
	emit cmd(CMD_WRITE_BAUDRATE, text.toUInt());
}

void MainWindow::timeout(void)
{
	qDebug() << "Timeout";
}

void MainWindow::device_param(device_parametrs par)
{
}

void MainWindow::device_read(device_data data)
{
	ui->label_version->setText("Version: " + data.version);
	if (!ui->lineEdit_slave_par->hasFocus())
		ui->lineEdit_slave_par->setText(QString::number(data.address));
	if (!ui->comboBox_baudrate_par->hasFocus())
		ui->comboBox_baudrate_par->setCurrentText(QString::number(data.baudrate));

	for (int i = 0; i < DI_MAX; i++)
		di[i]->setChecked(data.di[i]);
	for (int i = 0; i < DO_MAX; i++)
		dout[i]->setChecked(data.dout[i]);
	for (int i = 0; i < AI_MAX; i++)
	{
		ai[i]->setValue(data.ai[i] * 1000);
		ai_v[i]->setText(QString("%2\n%1V").arg(data.ai[i], 0, 'f', 3).arg(ai_name[i]));
	}
	for (int i = 0; i < DO_MAX; i++)
	{
		if (!dout_safe[i]->hasFocus())
			dout_safe[i]->setCurrentText(mode[data.dout_safe[i]]);
		if (!dout_mode[i]->hasFocus())
			dout_mode[i]->setCurrentText(mode[data.dout_mode[i]]);
	}
}

void MainWindow::Print_to_textBrowser(const QString & text)
{
	static uint32_t i;
	i++;
	ui->textBrowser->append(QString("%1: %2").arg(i).arg(text));
}

void MainWindow::on_comboBox_activated(int index)
{
	qDebug() << "Combobox clicked" << index;

	if (string_comport.size() != 0)
	{
		port_param.com_Port = string_comport[ui->comboBox->currentIndex()];
		port_param.baud_Rate = ui->comboBox_baudrate->currentText().toInt();
		port_param.stop_bit = 1;
		port_param.address = ui->lineEdit_slave->text().toInt();

		emit connect_to(port_param);
	}

	ui->comboBox->clear();
	string_comport.clear();
	for (const auto & info : QSerialPortInfo::availablePorts())
	{
		QString s = info.portName() + ": " + info.description();
		ui->comboBox->addItem(s);
		string_comport.append(info.systemLocation());
	}
}
