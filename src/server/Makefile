TARGET = server.so

FW_DIR = ../../../cpp_fw
THIRDPARTY_DIR = ../../../cpp/3rdparty
RASPI_DIR = $(FW_DIR)/dev/raspberry

SRC_DIR = .
COMMON_DIR = ../common
BIN_DIR = ../../bin

SRC = \
	$(SRC_DIR)/client/client_thread.cpp \
	$(SRC_DIR)/client/client_heartbeat.cpp \
	$(SRC_DIR)/command/command_device.cpp \
	$(SRC_DIR)/command/command_mode.cpp \
	$(SRC_DIR)/desktop/desktop_handler.cpp \
	$(SRC_DIR)/desktop/desktop_register.cpp \
	$(SRC_DIR)/device/device.cpp \
	$(SRC_DIR)/device/light_object.cpp \
	$(SRC_DIR)/event/device_event.cpp \
	$(SRC_DIR)/event/double_click_event.cpp \
	$(SRC_DIR)/event/event_parser.cpp \
	$(SRC_DIR)/event/event_handler.cpp \
	$(SRC_DIR)/event/mode_event.cpp \
	$(SRC_DIR)/mobile/mobile_handler.cpp \
	$(SRC_DIR)/mobile/mobile_register.cpp \
	$(SRC_DIR)/server/net_server.cpp \
	$(SRC_DIR)/server/smarty_server.cpp \
	$(COMMON_DIR)/smarty_config.cpp \
	$(COMMON_DIR)/enums.cpp \
	$(FW_DIR)/utils/threading/threading.cpp \
	$(FW_DIR)/utils/threading/threading_linux.cpp \
	$(FW_DIR)/utils/net/xsocket.cpp \
	$(FW_DIR)/utils/files/fileutils.cpp \
	$(FW_DIR)/utils/files/ConfigFile.cpp \
	$(FW_DIR)/utils/files/config.cpp \
	$(FW_DIR)/utils/utils.cpp \
	$(FW_DIR)/utils/strutils.cpp \
	$(FW_DIR)/common/logger.cpp \
	$(THIRDPARTY_DIR)/json/json_reader.cpp \
	$(THIRDPARTY_DIR)/json/json_writer.cpp \
	$(THIRDPARTY_DIR)/json/json_value.cpp


DEPS = $(FW_DIR)/common/asserts.h

OBJS = $(subst .cpp,.o, $(SRC))

INCLUDES = -I. -I.. -I../common \
	-I$(RASPI_DIR) -I$(RASPI_DIR)/.. -I$(FW_DIR) \
	-I$(FW_DIR)/types \
        -I$(FW_DIR)/common \
        -I$(FW_DIR)/utils \
        -I$(FW_DIR)/utils/files \
        -I$(FW_DIR)/utils/threading \
        -I$(FW_DIR)/utils/net \
	-I$(FW_DIR)/utils/serialize \
        -I$(THIRDPARTY_DIR)

CXX=g++
CXXFLAGS = $(INCLUDES) \
	-Wall -c -g -fPIC -std=c++11

LFLAGS =  -lpthread -lrt -Wall -shared


$(TARGET): $(OBJS)
	-@mkdir -p $(BIN_DIR)
	@echo "  [$(TARGET)]"
	@g++ $(LFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET)

.cpp.o: $(DEPS)
	@echo "    $< .."
	@$(CXX) $(CXXFLAGS) -o $@ $<


clean :
	-@rm -rf $(OBJS) $(BIN_DIR)/$(TARGET)

rebuild : clean $(TARGET)

