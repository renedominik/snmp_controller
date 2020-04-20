OBJ = \
../tara/source/string/io_string_functions.o \
../tara/source/string/string_functions.o \
main/snmp_system_controller.o

### USE EITHER OPT OR DEBUG !!
OPT = # -O3 
CDEBUG = #-g -pg

CC = g++

NAME = snmp_system_controller.exe

CPPFLAGS = -Wall  -O3 -fmessage-length=0 -Wno-deprecated -I /usr/local/

CFLAGS = $(CDEBUG) $(OPT) $(INCLUDES) 

LDFLAGS = $(CDEBUG) 

%.o: %.cc %.cpp 
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS) 

clean:
	rm -f $(NAME)
	rm -f $(OBJ)

