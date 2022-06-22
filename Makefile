#TODO: create the directory
OBJ_DIR = obj

PROTO_DIR = protobuf
PROTO_FILE = $(PROTO_DIR)/position.proto
PROTO_SRC = $(patsubst %.proto,%.pb.cc,$(PROTO_FILE))
PROTO_HDR = $(patsubst %.proto,%.pb.h,$(PROTO_FILE))
PROTO_OBJ = $(OBJ_DIR)/proto.o

%.pb.cc %.pb.h: %.proto
	protoc --cpp_out=. $<

$(PROTO_OBJ): $(PROTO_SRC) $(PROTO_HDR)
	g++ -I . -c $< -o $@

.PHONY: all clean
all: $(PROTO_OBJ)

clean:
	rm -f $(PROTO_OBJ) $(PROTO_SRC) $(PROTO_HDR)
