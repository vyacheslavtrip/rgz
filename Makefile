CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fPIC

SRC_DIR = src
INC_DIR = include
LIB_DIR = lib
BUILD_DIR = build
BIN = prog

# Источники
MAIN_SRC = $(SRC_DIR)/main/main.cpp
GENKEY_SRC = $(SRC_DIR)/generate_key/generate_key.cpp

XTEA_SRCS   = $(SRC_DIR)/xtea/xtea.cpp $(SRC_DIR)/xtea/utils.cpp $(SRC_DIR)/xtea/plugin.cpp
CHACHA_SRCS = $(SRC_DIR)/chacha20/chacha20.cpp $(SRC_DIR)/chacha20/utils.cpp $(SRC_DIR)/chacha20/plugin.cpp
RABBIT_SRCS = $(SRC_DIR)/rabbit/rabbit.cpp $(SRC_DIR)/rabbit/plugin.cpp $(SRC_DIR)/rabbit/utils.cpp

# Макрос для превращения пути src/.../*.cpp → build/.../*.o
define src_to_obj
$(BUILD_DIR)/$(subst $(SRC_DIR)/,,$(1:.cpp=.o))
endef

# Объекты
MAIN_OBJ    = $(call src_to_obj,$(MAIN_SRC))
GENKEY_OBJ  = $(call src_to_obj,$(GENKEY_SRC))
XTEA_OBJS   = $(foreach src,$(XTEA_SRCS),$(call src_to_obj,$(src)))
CHACHA_OBJS = $(foreach src,$(CHACHA_SRCS),$(call src_to_obj,$(src)))
RABBIT_OBJS = $(foreach src,$(RABBIT_SRCS),$(call src_to_obj,$(src)))

# Инклуды
INC = -I$(INC_DIR) \
      -I$(INC_DIR)/xtea \
      -I$(INC_DIR)/generate_key \
      -I$(INC_DIR)/chacha20 \
      -I$(INC_DIR)/rabbit \

.PHONY: all clean

all: $(LIB_DIR)/libxtea.so \
     $(LIB_DIR)/libchacha20.so \
     $(LIB_DIR)/librabbit.so \
     $(BIN)

# Сборка бинарника
$(BIN): $(MAIN_OBJ) $(GENKEY_OBJ)
	$(CXX) $(CXXFLAGS) $^ -ldl -o $@

# Динамические библиотеки
$(LIB_DIR)/libxtea.so: $(XTEA_OBJS)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^

$(LIB_DIR)/libchacha20.so: $(CHACHA_OBJS)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^

$(LIB_DIR)/librabbit.so: $(RABBIT_OBJS)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^

# Сборка объектов
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN) \
		$(LIB_DIR)/libxtea.so \
		$(LIB_DIR)/libchacha20.so \
		$(LIB_DIR)/librabbit.so