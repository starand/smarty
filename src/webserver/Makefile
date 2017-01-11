TARGET = webserver.so

FW_DIR = ../../../cpp_fw
THIRDPARTY_DIR = ../../../cpp/3rdparty
RASPI_DIR = $(FW_DIR)/dev/raspberry
PROJECTS = ../../projects

SRC_DIR = .
COMMON_DIR = ../common
BIN_DIR = ../../bin

SRC = $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/web_server.cpp \
	$(SRC_DIR)/webclient_handler.cpp \
	$(SRC_DIR)/thread_pool.cpp \
	$(SRC_DIR)/request_parser.cpp \
	$(SRC_DIR)/html_builder.cpp \
	$(SRC_DIR)/request_executor.cpp \
	$(COMMON_DIR)/smarty_config.cpp \
	$(FW_DIR)/utils/threading/threading.cpp \
	$(FW_DIR)/utils/threading/threading_linux.cpp \
	$(FW_DIR)/utils/net/xsocket.cpp \
	$(FW_DIR)/utils/files/config.cpp \
	$(FW_DIR)/common/logger.cpp \
	$(FW_DIR)/utils/utils.cpp \
	$(FW_DIR)/utils/strutils.cpp \
	$(THIRDPARTY_DIR)/json/json_reader.cpp \
	$(THIRDPARTY_DIR)/json/json_writer.cpp \
	$(THIRDPARTY_DIR)/json/json_value.cpp

DEPS = $(FW_DIR)/common/asserts.h

OBJS = $(subst .cpp,.o, $(SRC))

INCLUDES = -I. -I.. -I../common \
	-I$(RASPI_DIR) -I$(RASPI_DIR)/.. \
	-I$(FW_DIR)/types -I$(FW_DIR)/common -I$(FW_DIR)/utils -I$(FW_DIR)/utils/files -I$(FW_DIR)/utils/threading -I$(FW_DIR)/utils/net \
	-I$(FW_DIR)/utils/serialize -I$(THIRDPARTY_DIR)

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
	-@rm -rf $(PROJECTS)/*.suo $(PROJECTS)/*.sdf $(PROJECTS)/projects/*.user $(PROJECTS)/webserver/Debug

rebuild : clean $(TARGET)

