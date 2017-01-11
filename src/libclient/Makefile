TARGET = libclient.so

FW_DIR = ../../../../../framework
PROJECTS = ../../projects

SRC_DIR = .
COMMON_DIR = ../common
BIN_DIR = ../../bin

SRC = $(SRC_DIR)/libclient_interface.cpp \
	$(SRC_DIR)/smarty_connector.cpp \
	$(SRC_DIR)/notification_listener.cpp \
	$(SRC_DIR)/command_sender.cpp \
	$(FW_DIR)/utils/net/xsocket.cpp \
	$(FW_DIR)/utils/utils.cpp \
	$(FW_DIR)/utils/strutils.cpp \
	$(FW_DIR)/utils/files/fileutils.cpp \
	$(FW_DIR)/utils/files/ini_parser.cpp \
	$(FW_DIR)/utils/threading/threading.cpp \
	$(FW_DIR)/utils/threading/threading_linux.cpp \
	$(FW_DIR)/common/logger.cpp

DEPS = $(FW_DIR)/common/asserts.h

OBJS = $(subst .cpp,.o, $(SRC))

INCLUDES = -I. -I.. -I../common \
	-I$(FW_DIR)/types -I$(FW_DIR)/common -I$(FW_DIR)/utils -I$(FW_DIR)/utils/threading -I$(FW_DIR)/utils/net \
	-I$(FW_DIR)/utils/serialize \
	-I$(FW_DIR)/utils/files

CXX=g++
CXXFLAGS = $(INCLUDES) \
	-c -g -Wall -fPIC -std=c++11
	
LDFLAGS = -shared -lpthread -lrt


$(TARGET): $(OBJS)
	-@mkdir -p $(BIN_DIR)
	@echo "  [$(TARGET)]"
	@$(CXX) $(LDFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET)

.cpp.o: $(DEPS)
	@echo "    $< .."
	@$(CXX) $(CXXFLAGS) -o $@ $<

run : $(TARGET)
	$(BIN_DIR)/$(TARGTE)


clean :
	-@rm -rf $(OBJS) $(BIN_DIR)/$(TARGET)
	-@rm -rf $(PROJECTS)/*.suo $(PROJECTS)/*.sdf $(PROJECTS)/projects/*.user $(PROJECTS)/client/Debug

rebuild : clean $(TARGET)
	
