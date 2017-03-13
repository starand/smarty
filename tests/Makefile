TARGET = unittest

FW_DIR = ../../cpp_fw
RASPI_DIR = $(FW_DIR)/dev/raspberry

THIRD_PARTY_DIR = ../../3rdparty
GTEST_DIR = $(THIRD_PARTY_DIR)/googletest/googletest
JSON_DIR = $(THIRD_PARTY_DIR)/json

SRC_DIR = ../src
BIN_DIR = ../bin
LIB_DIR = ./libs

SRC = main.cpp \
	driver/device_controller_test.cpp \
	server/client_info_test.cpp \
	server/device_test.cpp \
	$(SRC_DIR)/common/config.cpp \
	$(SRC_DIR)/driver/device_controller.cpp \
	$(SRC_DIR)/server/device/device.cpp \
	$(FW_DIR)/common/logger.cpp \
	$(FW_DIR)/utils/net/xsocket.cpp \
	$(FW_DIR)/utils/threading/threading.cpp \
	$(FW_DIR)/utils/threading/threading_linux.cpp \
	$(FW_DIR)/utils/files/config.cpp \
	$(FW_DIR)/utils/files/fileutils.cpp \
	$(FW_DIR)/utils/files/ini_parser.cpp \
	$(FW_DIR)/utils/strutils.cpp \
	$(FW_DIR)/utils/utils.cpp \
	$(JSON_DIR)/json_reader.cpp \
	$(JSON_DIR)/json_value.cpp \
	$(JSON_DIR)/json_writer.cpp

DEPS = $(FW_DIR)/common/asserts.h

OBJS = $(subst .cpp,.o, $(SRC))

INCLUDES = -I. \
	-I$(GTEST_DIR)/include \
	-I$(SRC_DIR) \
	-I$(RASPI_DIR) \
	-I$(RASPI_DIR)/.. \
	-I$(FW_DIR) \
	-I$(FW_DIR)/types \
	-I$(FW_DIR)/common \
	-I$(FW_DIR)/utils \
	-I$(FW_DIR)/utils/threading \
	-I$(FW_DIR)/utils/net \
	-I$(FW_DIR)/utils/files \
	-I$(THIRD_PARTY_DIR)

CXX=g++
CXXFLAGS = $(INCLUDES) \
	-Wall -fPIC -c -g -std=c++11

LFLAGS = -lpthread -L$(LIB_DIR) -lgtest


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

