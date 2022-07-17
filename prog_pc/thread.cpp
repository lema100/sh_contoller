#include "thread.h"

void thread::run(){
	QTimer timer;
	connection_status=0;

	connect(&timer, SIGNAL(timeout()), this, SLOT(m_Timeout()));
	timer.start(50);
	qDebug() << "Transmiter start, thread_id = " << QThread::currentThreadId();

	exec();
}

int thread::mb_err(modbus_t * my_modbus, QString err)
{
	_cmd = 0;
	qDebug() << err;
	modbus_close(my_modbus);
	modbus_free(my_modbus);
	emit Print_to_textBrowser(err);
	return -1;
}

const QString update_state_to_str(update_state_t status)
{
	const QMap<update_state_t, QString> update_state_str =
	{
		{UPDATE_CRC_STRUCT_ERROR, "CRC HEADER ERROR"},
		{UPDATE_LEN_ERROR, "LENGTH APPLICATION ERROR"},
		{UPDATE_END_ADDR_ERROR, "END ADDRESS ERROR"},
		{UPDATE_CRC_ERROR, "CRC APPLICATION ERROR"},
		{UPDATE_CRC_MISMATCH, "APPLICATION CAN UPDATE"},
		{UPDATE_APP_IS_ACTUAL, "APPLICATION IS ACTUAL"},
		{UPDATE_APP_IS_UPDATED, "APPLICATION IS UPDATED"},
	};

	if (!update_state_str.keys().contains(status))
		return "UNKNOWN STATE";

	return update_state_str[status];
}

int thread::m_Timeout(){
	uint16_t tab_reg[UINT8_MAX];

	if (connection_status)
	{
		qDebug() << "Transmiter, thread_id = " << QThread::currentThreadId();

		modbus_t* my_modbus = modbus_new_rtu((char *)modbus_port.com_Port.toLatin1().data(), modbus_port.baud_Rate, 'N', 8, modbus_port.stop_bit);
		modbus_set_slave(my_modbus, modbus_port.address);

		if (modbus_connect(my_modbus) == -1)
			return mb_err(my_modbus, "Error conection");

		if ((modbus_read_registers(my_modbus, 0, 120, tab_reg)) == -1)
		{
			return mb_err(my_modbus, "Error read data");
		}
		else
		{
			QList<uint16_t> list;
			device_data data;

			for (uint16_t i = 0; i < 120; i++)
				list.append(tab_reg[i]);
			qDebug() << list;

			data.address = tab_reg[MB_SETT_MB_SLAVE];
			data.baudrate = tab_reg[MB_SETT_MB_BAUD] * 10;
			data.version = QString::fromLatin1((char *) &tab_reg[MB_VERSION]);

			uint32_t tmp = ((uint32_t)tab_reg[MB_IN_MSB] << 16) | tab_reg[MB_IN_LSB];
			for (int i = 0; i < DI_MAX; i++)
				data.di[i] = tmp & (1 << i);
			tmp = ((uint32_t)tab_reg[MB_OUT_MSB] << 16) | tab_reg[MB_OUT_LSB];
			for (int i = 0; i < DO_MAX; i++)
				data.dout[i] = tmp & (1 << i);

			for (int i = 0; i < DO_MAX; i++)
			{
				data.dout_safe[i] = tab_reg[MB_SETT_OUT0 + i] >> 8;
				data.dout_mode[i] = tab_reg[MB_SETT_OUT0 + i] & 0xFF;
			}

			int pos[] = {ADC_VCC, ADC_DI11, ADC_DI12, ADC_DI15, ADC_DI16, ADC_DI19, ADC_DI20};
			for (int i = 0; i < AI_MAX; i++)
				data.ai[i] = tab_reg[MB_AI_CH0 + pos[i]] / 1000.0;

			if (_cmd >= CMD_SET_OUT0 && _cmd < (CMD_SET_OUT0 + DO_MAX))
			{
				tab_reg[0] = _cmd_arg;
				if ((modbus_write_registers(my_modbus, _cmd - CMD_SET_OUT0 + MB_SET_SETT_OUT0, 1, tab_reg)) == -1)
					return mb_err(my_modbus, "Error set output mode cmd");
				emit Print_to_textBrowser("Set output mode OK");
				_cmd = 0;
			}
			else
			{
				switch (_cmd)
				{
				case CMD_SET_OUT:
				{
					tab_reg[0] = _cmd_arg >> 16;
					tab_reg[1] = (uint16_t)_cmd_arg;
					if ((modbus_write_registers(my_modbus, MB_OUT_MSB, 2, tab_reg)) == -1)
						return mb_err(my_modbus, "Error set output cmd");
					emit Print_to_textBrowser("Set output OK");
					break;
				}
				case CMD_RESET:
				{
					tab_reg[0] = 1;
					if ((modbus_write_registers(my_modbus, MB_RESET, 1, tab_reg)) == -1)
						return mb_err(my_modbus, "Error reset cmd");
					emit Print_to_textBrowser("Reset OK");
					break;
				}
				case CMD_WRITE_BAUDRATE:
				{
					tab_reg[0] = _cmd_arg / 10;
					if ((modbus_write_registers(my_modbus, MB_SET_SETT_MB_BAUD, 1, tab_reg)) == -1)
						return mb_err(my_modbus, "Error write baudrate cmd");
					emit Print_to_textBrowser("Write baudrate OK");
					break;
				}
				case CMD_WRITE_ADDRESS:
				{
					tab_reg[0] = _cmd_arg;
					if ((modbus_write_registers(my_modbus, MB_SET_SETT_MB_SLAVE, 1, tab_reg)) == -1)
						return mb_err(my_modbus, "Error write address cmd");
					emit Print_to_textBrowser("Write address OK");
					break;
				}
				case CMD_BURN_BIN:
				{
					update_ctx_t su;
					su.ver = 0;
					su.ver_ext = 1;
					su.start_add = UPDATE_START;
					su.end_add = UPDATE_START + _bin.size();
					su.crc = CRC16(_bin.data(), 0, _bin.size());
					su.crc_this = CRC16((char *)&su, 0, sizeof(update_ctx_t) - 4);
					_bin.prepend((char *)&su, sizeof(update_ctx_t));

					for (uint32_t i = UPDATE_START; i < UPDATE_END; i += FLASH_PAGE_SIZE)
					{
						uint32_t retry = 3;
						tab_reg[0] = UPDATE_CMD_ERASE;
						tab_reg[1] = (i - FLASH_BASE) / 4;
						if ((modbus_write_registers(my_modbus, MB_UPDATE_CMD, 2, tab_reg)) == -1)
							return mb_err(my_modbus, QString("Update erase 0x%1 ERROR0").arg(i, 8, 16, QLatin1Char('0')));
						while (retry && ((tab_reg[0] & 0x8000) == 0))
						{
							if ((modbus_read_registers(my_modbus, MB_UPDATE_CMD, 1, tab_reg)) == -1)
								return mb_err(my_modbus, QString("Update erase 0x%1 ERROR1").arg(i, 8, 16, QLatin1Char('0')));
							retry--;
						}
						if (retry == 0)
							return mb_err(my_modbus, QString("Update erase 0x%1 ERROR2").arg(i, 8, 16, QLatin1Char('0')));
						emit Print_to_textBrowser(QString("Update erase 0x%1 OK").arg(i, 8, 16, QLatin1Char('0')));
					}
					for (uint32_t i = 0; i < _bin.size(); i += 200)
					{
						uint32_t retry = 3;
						tab_reg[0] = UPDATE_CMD_WRITE;
						tab_reg[1] = (UPDATE_START + i - FLASH_BASE) / 4;
						tab_reg[2] = _bin.size() - i > 200 ? 200 : _bin.size() - i;
						memcpy(&tab_reg[3], _bin.data() + i, tab_reg[2]);
						if ((modbus_write_registers(my_modbus, MB_UPDATE_CMD, 105, tab_reg)) == -1)
							return mb_err(my_modbus, QString("Write update 0x%1 ERROR0").arg(i, 8, 16, QLatin1Char('0')));
						while (retry && ((tab_reg[0] & 0x8000) == 0))
						{
							if ((modbus_read_registers(my_modbus, MB_UPDATE_CMD, 1, tab_reg)) == -1)
								return mb_err(my_modbus, QString("Write update 0x%1 ERROR1").arg(i, 8, 16, QLatin1Char('0')));
							retry--;
						}
						if (retry == 0)
							return mb_err(my_modbus, QString("Write update 0x%1 ERROR2").arg(i, 8, 16, QLatin1Char('0')));
						emit Print_to_textBrowser(QString("Write update 0x%1 OK").arg(i + UPDATE_START, 8, 16, QLatin1Char('0')));
					}
					break;
				}
				case CMD_CHECK_BIN:
				{
					uint32_t retry = 3;
					tab_reg[0] = UPDATE_CMD_CHECK;
					tab_reg[1] = (UPDATE_START - FLASH_BASE) / 4;
					if ((modbus_write_registers(my_modbus, MB_UPDATE_CMD, 2, tab_reg)) == -1)
						return mb_err(my_modbus, QString("Check update ERROR"));
					while (retry && ((tab_reg[0] & 0x8000) == 0))
					{
						if ((modbus_read_registers(my_modbus, MB_UPDATE_CMD, 1, tab_reg)) == -1)
							return mb_err(my_modbus, QString("Check update ERROR"));
						retry--;
					}
					if (retry == 0)
						return mb_err(my_modbus, QString("Check update ERROR"));
					emit Print_to_textBrowser(QString("Check update %1").arg(update_state_to_str((update_state_t)(tab_reg[0] - 0x8000))));
					break;
				}
				default:
					emit device_read(data);
					emit Print_to_textBrowser("Read OK");
					break;
				}
				_cmd = 0;
			}
		}
		modbus_close(my_modbus);
		modbus_free(my_modbus);
	}
	return 1;
}


void thread::connect_to(port_parametrs par)
{
	modbus_port = par;
	connection_status = 1;
}

void thread::disconect()
{
	connection_status = 0;
}

void thread::cmd(uint32_t cmd, uint32_t cmd_arg)
{
	_cmd = cmd;
	_cmd_arg = cmd_arg;
}

void thread::write_parametrs(device_parametrs par)
{
	_cmd = CMD_WRITE_PARAM;
}

void thread::burn_bin(QByteArray bin)
{
	_bin = bin;
	_cmd = CMD_BURN_BIN;
}
