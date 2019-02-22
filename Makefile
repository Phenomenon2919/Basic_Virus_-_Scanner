PROG_NAME = hello_world.c
OBJ_NAME = hello
SCAN_NAME = scanner.c
SCAN_OBJ_NAME = scanner
DUMP_VIRUS_PROG = dump_virus.c
DUMP_VIRUS_OBJ = dump_virus

default: $(PROG_NAME)
	gcc $(PROG_NAME) -o $(OBJ_NAME)

all: $(PROG_NAME)
	gcc $(PROG_NAME) -o $(OBJ_NAME)

scanner: $(SCAN_NAME)
	gcc $(SCAN_NAME) -o $(SCAN_OBJ_NAME) -lssl -lcrypto -lpthread

dump_virus: $(DUMP_VIRUS_PROG)
	gcc $(DUMP_VIRUS_PROG) -o $(DUMP_VIRUS_OBJ)