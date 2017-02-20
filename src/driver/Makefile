TARGET = driver.so

FW_DIR = ../../../cpp_fw
RASPI_DIR = $(FW_DIR)/dev/raspberry

SRC_DIR = .
BIN_DIR = ../../bin

SRC = $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/device_controller.cpp \
	$(SRC_DIR)/driver.cpp \
	$(RASPI_DIR)/gpio.cpp \
	$(RASPI_DIR)/c_gpio.cpp \
	$(RASPI_DIR)/bcm2835.cpp \
	$(RASPI_DIR)/spi.cpp \
	$(FW_DIR)/utils/utils.cpp \
	$(FW_DIR)/utils/strutils.cpp \
	$(FW_DIR)/utils/threading/threading.cpp \
	$(FW_DIR)/utils/threading/threading_linux.cpp \
	$(FW_DIR)/utils/net/xsocket.cpp \
	$(FW_DIR)/utils/files/fileutils.cpp \
	$(FW_DIR)/utils/files/ini_parser.cpp \
	$(FW_DIR)/common/logger.cpp

DEPS = $(FW_DIR)/common/asserts.h

OBJS = $(subst .cpp,.o, $(SRC))

INCLUDES = -I. \
	-I.. \
	-I../common \
	-I$(RASPI_DIR) \
	-I$(RASPI_DIR)/.. \
	-I$(FW_DIR)/types \
	-I$(FW_DIR)/common \
	-I$(FW_DIR)/utils \
	-I$(FW_DIR)/utils/threading \
	-I$(FW_DIR)/utils/net \
	-I$(FW_DIR)/utils/files

CXX=g++
CXXFLAGS = $(INCLUDES) \
	-Wall -fPIC -c -g -std=c++11

LFLAGS = -shared -lpthread


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

