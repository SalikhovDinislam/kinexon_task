OBJ_DIR = obj
BIN_DIR = bin

BIN_GENERATOR = $(BIN_DIR)/data_generator
BIN_RECEIVER  = $(BIN_DIR)/data_receiver
BINS = $(BIN_GENERATOR) $(BIN_RECEIVER)

PROTO_DIR = protobuf
PROTO_FILE = $(PROTO_DIR)/position.proto
PROTO_SRC = $(patsubst %.proto,%.pb.cc,$(PROTO_FILE))
PROTO_HDR = $(patsubst %.proto,%.pb.h,$(PROTO_FILE))
PROTO_OBJ = $(patsubst $(PROTO_DIR)/%.cc,$(OBJ_DIR)/%.o,$(PROTO_SRC))

CPP_FLAGS = -Wall -Wextra -Werror
LFLAGS = -lprotobuf -lzmq -pthread

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

$(BIN_GENERATOR): $(OBJS) | $(BIN_DIR)
	g++ $^ $(LFLAGS) -o $@

$(BIN_RECEIVER): receiver/debug_receiver.cpp include/params.h $(PROTO_OBJ) | $(BIN_DIR)
	g++ $(CPP_FLAGS) -I include -I $(PROTO_DIR) $< $(PROTO_OBJ) $(LFLAGS) -o $@

.PHONY: all clean check
all: $(BINS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(PROTO_SRC) $(PROTO_HDR)

TIMEOUT = 3

check: all
	@timeout $(TIMEOUT) $(BIN_RECEIVER)&
	@timeout $(TIMEOUT) $(BIN_GENERATOR)&
	sleep $(TIMEOUT)

todo:
	@git grep -wn TODO | grep -v 'Makefile.*git' | grep --color TODO
