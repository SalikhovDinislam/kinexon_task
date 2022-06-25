OBJ_DIR = obj
BIN_DIR = bin

BIN = $(BIN_DIR)/data_generator

PROTO_DIR = protobuf
PROTO_FILE = $(PROTO_DIR)/position.proto
PROTO_SRC = $(patsubst %.proto,%.pb.cc,$(PROTO_FILE))
PROTO_HDR = $(patsubst %.proto,%.pb.h,$(PROTO_FILE))
PROTO_OBJ = $(patsubst $(PROTO_DIR)/%.cc,$(OBJ_DIR)/%.o,$(PROTO_SRC))

CPP_FLAGS = -Wall -Wextra -Werror

%.pb.cc %.pb.h: %.proto
	protoc --cpp_out=. $<

$(PROTO_OBJ): $(PROTO_SRC) $(PROTO_HDR) | $(OBJ_DIR)
	g++ $(CPP_FLAGS) -I . -c $< -o $@

SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC)) $(PROTO_OBJ)

$(OBJ_DIR) $(BIN_DIR) :
	mkdir -p $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(PROTO_HDR) $(wildcard include/*) | $(OBJ_DIR)
	g++ $(CPP_FLAGS) -I include -I $(PROTO_DIR) -c $< -o $@

$(BIN) : $(OBJS) | $(BIN_DIR)
	g++ $^ -lprotobuf -pthread -o $@

.PHONY: all clean check
all: $(BIN)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(PROTO_SRC) $(PROTO_HDR)

check: all
	$(BIN) && echo OK
