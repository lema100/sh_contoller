#ifndef MAIN_H
#define MAIN_H

#include <QWidget>
#include <QDebug>
#include <QMap>
#include <QDateTime>
#include <QThread>

#define PARAM_COUNT				50
#define MAX_ARCH 				256
#define MAX_ARCH_PAR			256

#define DO_MAX 32
#define DI_MAX 20
#define AI_MAX 7

struct port_parametrs
{
	QString com_Port;
	int baud_Rate;
	int stop_bit;
	int address;
};

struct device_parametrs
{
	QVector<uint32_t> list;
};

struct device_data
{
	QString version;
	int baudrate;
	int address;
	float ai[AI_MAX];
	bool di[DI_MAX];
	bool dout[DO_MAX];
	uint8_t dout_mode[DO_MAX];
	uint8_t dout_safe[DO_MAX];
};

enum
{
	ADC_DI11,
	ADC_VCC,
	ADC_DI19,
	ADC_DI20,
	ADC_DI15,
	ADC_DI16,
	ADC_DI12,
};

enum
{
	CMD_SET_OUT = 1,
	CMD_RESET,
	CMD_WRITE_BAUDRATE,
	CMD_WRITE_ADDRESS,
	CMD_WRITE_PARAM,
	CMD_BURN_BIN,
	CMD_CHECK_BIN,
	CMD_SET_OUT0 = 100,
};

enum
{
	MB_IN_MSB = 0,
	MB_IN_LSB,
	MB_OUT_MSB,
	MB_OUT_LSB,
	MB_OUT_RST_MSB,
	MB_OUT_RST_LSB,
	MB_OUT_SET_MSB,
	MB_OUT_SET_LSB,
	MB_AI_CH0 = 10,
	MB_AI_CH1,
	MB_AI_CH2,
	MB_AI_CH3,
	MB_AI_CH4,
	MB_AI_CH5,
	MB_AI_CH6,
	MB_AI_CH7,
	MB_SETT0 = 30,
	MB_SETT_MB_BAUD,
	MB_SETT_MB_SLAVE,
	MB_SETT_OUT0 = 40,
	MB_SETT_MAX = 79,
	MB_VERSION = 80,
	MB_RESET = 99,
	MB_SET_SETT0 = 100,
	MB_SET_SETT_MB_BAUD,
	MB_SET_SETT_MB_SLAVE,
	MB_SET_SETT_OUT0 = 110,
	MB_SET_SETT_MAX = 149,
	MB_UPDATE_CMD = 256,
	MB_UPDATE_ADDR,
	MB_UPDATE_LEN,
	MB_UPDATE_DATA,
};

enum
{
	OUT_TO_MB = 0,
	OUT_TO_LOW,
	OUT_TO_HIGH,
	OUT_TO_IN_0 = 100,
	OUT_TO_INV_IN_0 = 120,
};

#endif // MAIN_H
