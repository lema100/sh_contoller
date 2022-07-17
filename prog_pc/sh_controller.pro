#-------------------------------------------------
#
# Project created by QtCreator 2020-08-26T23:49:55
#
#-------------------------------------------------

QT += core gui charts serialport widgets

TARGET = sh_controller_config
TEMPLATE = app

SOURCES += \
		main.cpp \
		thread.cpp \
		mainwindow.cpp \
		modbus/modbus.c \
		modbus/modbus-data.c \
		modbus/modbus-rtu.c \
		modbus/modbus-tcp.c \
		../prog_mcu/User/crc16.c \
		../prog_mcu/User/update.c \

HEADERS += \
		main.h \
		stm32f1xx_hal.h \
		thread.h \
		mainwindow.h \
		modbus/modbus.h \
		modbus/modbus-version.h \
		modbus/modbus-private.h \
		modbus/modbus-rtu.h \
		modbus/modbus-rtu-private.h \
		modbus/modbus-tcp.h \
		modbus/modbus-tcp-private.h \
		modbus/config.h \
		../prog_mcu/User/crc16.h \
		../prog_mcu/User/update.h \

FORMS += \
		mainwindow.ui

LIBS += -lws2_32

RC_ICONS += sh_controller.ico
