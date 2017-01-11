TARGET = kclient


PROJECTS = ../../projects
FW_DIR = ../../../../../framework
JSON = ../../../../../3rdparty/json
SRC_DIR = .
COMMON_DIR = ../common
BIN_DIR = ../../bin

SRC = $(SRC_DIR)/main.cpp \
	$(COMMON_DIR)/client_lib.cpp \
	$(COMMON_DIR)/library_intf.cpp \
	$(COMMON_DIR)/config_parser.cpp \
	$(FW_DIR)/utils/files/library.cpp \
	$(FW_DIR)/utils/files/ini_parser.cpp \
	$(FW_DIR)/utils/files/fileutils.cpp \
	$(FW_DIR)/common/logger.cpp \
	$(FW_DIR)/utils/threading/threading.cpp \
	$(FW_DIR)/utils/threading/threading_linux.cpp \
	$(FW_DIR)/utils/net/xsocket.cpp \
	$(FW_DIR)/utils/strutils.cpp \
	$(FW_DIR)/utils/utils.cpp \
	$(JSON)/json_value.cpp \
	$(JSON)/json_reader.cpp \
	$(JSON)/json_writer.cpp
	
DEPS = $(FW_DIR)/common/asserts.h

OBJS = $(subst .cpp,.o, $(SRC))

INCLUDES = -I. -I.. -I../common \
	-I$(FW_DIR)/types -I$(FW_DIR)/common -I$(FW_DIR)/utils -I$(FW_DIR)/utils/threading -I$(FW_DIR)/utils/net \
	-I$(FW_DIR)/utils/serialize -I$(FW_DIR)/utils/files \
	-I$(JSON)/..

CXX=g++
CXXFLAGS = $(INCLUDES) \
	-c -g -Wall -std=c++11
	
LDFLAGS = -ldl -lpthread -lrt


$(TARGET): $(OBJS)
	-@mkdir -p $(BIN_DIR)
	@echo "  [$(TARGET)]"
	@$(CXX) $(LDFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET)

.cpp.o: $(DEPS)
	@echo "    $< .."
	@$(CXX) $(CXXFLAGS) -o $@ $<

clean :
	-@rm -rf $(OBJS) $(BIN_DIR)/$(TARGET)
	-@rm -rf $(PROJECTS)/*.suo $(PROJECTS)/*.sdf $(PROJECTS)/projects/*.user $(PROJECTS)/client/Debug

rebuild: clean $(TARGET)
	-@echo REBUILD

run : $(TARGET)
	-@./run.sh
