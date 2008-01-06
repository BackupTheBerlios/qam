TEMPLATE	= app
QT			+= network
CONFIG		+= static
SOURCES		= ../src/main.cpp ../src/monitor.cpp ../src/viewmonitor.cpp ../src/viewsetmonitor.cpp ../src/viewconfigure.cpp ../src/options.cpp ../src/footer.cpp ../src/resizer.cpp
HEADERS		= ../src/monitor.h ../src/viewmonitor.h ../src/viewsetmonitor.h ../src/viewconfigure.h ../src/connectionentry.h ../src/options.h ../src/footer.h ../src/resizer.h
TARGET		= qam
RESOURCES   = ../qam-resource.qrc
