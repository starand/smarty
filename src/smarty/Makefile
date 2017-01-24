TARGET = smarty

FW_DIR = ../../../cpp_fw
THIRDPARTY_DIR = ../../../cpp/3rdparty
RASPI_DIR = $(FW_DIR)/dev/raspberry
LIBRARY_PATH = $(shell pwd)/$(BIN_DIR)

SRC_DIR = .
COMMON_DIR = ../common
BIN_DIR = ../../bin
CFG_DIR = ./cfg

SRC = \
	$(SRC_DIR)/smarty_app.cpp \
	$(SRC_DIR)/driver_module.cpp \
	$(SRC_DIR)/server_module.cpp \
	$(SRC_DIR)/webserver_module.cpp \
	$(COMMON_DIR)/library_intf.cpp \
	$(COMMON_DIR)/config.cpp \
	$(FW_DIR)/common/logger.cpp \
	$(FW_DIR)/utils/utils.cpp \
	$(FW_DIR)/utils/strutils.cpp \
	$(FW_DIR)/utils/files/library.cpp \
	$(FW_DIR)/utils/files/ini_parser.cpp \
	$(FW_DIR)/utils/files/fileutils.cpp \
	$(FW_DIR)/utils/files/config.cpp \
	$(FW_DIR)/utils/net/xsocket.cpp \
	$(FW_DIR)/utils/threading/threading.cpp \
	$(FW_DIR)/utils/threading/threading_linux.cpp \
	$(THIRDPARTY_DIR)/json/json_reader.cpp \
	$(THIRDPARTY_DIR)/json/json_writer.cpp \
	$(THIRDPARTY_DIR)/json/json_value.cpp

DEPS = $(FW_DIR)/common/asserts.h

OBJS = $(subst .cpp,.o, $(SRC))

INCLUDES = -I./ -I../common -I.. \
	-I$(RASPI_DIR) -I$(RASPI_DIR)/.. \
	-I$(FW_DIR)/types \
	-I$(FW_DIR)/common \
	-I$(FW_DIR)/utils \
	-I$(FW_DIR)/utils/files \
	-I$(FW_DIR)/utils/threading \
	-I$(FW_DIR)/utils/net \
	-I$(THIRDPARTY_DIR)

CXX=g++
CXXFLAGS = $(INCLUDES) -c -g -fPIC -std=gnu++0x


LFLAGS = -ldl -lpthread -lrt


$(TARGET): $(OBJS)
	-@mkdir -p $(BIN_DIR)
	@echo "  [$(TARGET)]"
	@$(CXX) $(LFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET)

.cpp.o: $(DEPS)
	-@mkdir -p $(BIN_DIR)
	@echo "    $< .."
	@$(CXX) $(CXXFLAGS) -o $@ $<

clean :
	-@rm -rf $(OBJS) $(BIN_DIR)/$(TARGET)

rebuild : clean $(TARGET)

