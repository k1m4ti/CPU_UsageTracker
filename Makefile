#files extensions
.SUFFIXES: .c .h .o .x

#compiler and linker (type clang or gcc)
CO = clang
LD = $(CO)
FLAGS = $(CLANG_FLAGS)

#flags
GCC_FLAGS = -Wall -Wextra -std=c99 -pedantic -O -pthread
CLANG_FLAGS = -Weverything -std=c99 -O -Wno-disabled-macro-expansion -Wno-missing-noreturn


#files names
MAIN_TEST = main_test
FUNCTIONS_TEST = functions_test
READER_TEST = reader_test
ANALYZER_TEST = analyzer_test
PRINTER_TEST = printer_test
WATCHDOG_TEST = watchdog_test
LOGGER_TEST = logger_test

MAIN = main
FUNCTIONS = functions
READER = reader
ANALYZER = analyzer
PRINTER = printer
WATCHDOG = watchdog
LOGGER = logger

HEADER_FILE = functions.h

#executive programs names
EXEC_TEST = test.x
EXEC = prog.x


#object files
MAIN_TEST_O = $(MAIN_TEST).o
FUNCTIONS_TEST_O = $(FUNCTIONS_TEST).o
READER_TEST_O = $(READER_TEST).o
ANALYZER_TEST_O = $(ANALYZER_TEST).o
PRINTER_TEST_O = $(PRINTER_TEST).o
WATCHDOG_TEST_O = $(WATCHDOG_TEST).o
LOGGER_TEST_O = $(LOGGER_TEST).o

MAIN_O = $(MAIN).o
FUNCTIONS_O = $(FUNCTIONS).o
READER_O = $(READER).o
ANALYZER_O = $(ANALYZER).o
PRINTER_O = $(PRINTER).o
WATCHDOG_O = $(WATCHDOG).o
LOGGER_O = $(LOGGER).o


#dir for libraries
LIB_DIR = ./lib

#libraries names
TEST_LIB_NAME = $(FUNCTIONS_TEST)
LIB_NAME = $(FUNCTIONS)

#static libraries
STAT_LIB_TEST = lib$(TEST_LIB_NAME).a
STAT_LIB = lib$(LIB_NAME).a

#compiled files for libraries
TEST_LIB_OBJS = $(FUNCTIONS_TEST_O) $(READER_TEST_O) $(ANALYZER_TEST_O) $(PRINTER_TEST_O) $(WATCHDOG_TEST_O) $(LOGGER_TEST_O)
LIB_OBJS = $(FUNCTIONS_O) $(READER_O) $(ANALYZER_O) $(PRINTER_O) $(WATCHDOG_O) $(LOGGER_O)

#static library archiver for creating libraries
AR = ar
AR_FLAGS = rsv


#pattern rule
%.o: %.c %.h
	$(CO) $(FLAGS) -c $<
%.o: %.c
	$(CO) $(FLAGS) -c $<


#creating libraries
$(STAT_LIB_TEST): $(TEST_LIB_OBJS)
	$(AR) $(AR_FLAGS) $@ $?
	mkdir -p $(LIB_DIR)
	mv $(STAT_LIB_TEST) $(LIB_DIR)

$(STAT_LIB): $(LIB_OBJS)
	$(AR) $(AR_FLAGS) $@ $?
	mkdir -p $(LIB_DIR)
	mv $(STAT_LIB) $(LIB_DIR)


#executive programs
$(EXEC_TEST): $(MAIN_TEST_O) $(STAT_LIB_TEST)
	$(LD) -o $@ $(FLAGS) $(MAIN_TEST_O) -L$(LIB_DIR) -l$(TEST_LIB_NAME)

$(EXEC): $(MAIN_O) $(STAT_LIB)
	$(LD) -o $@ $(FLAGS) $(MAIN_O) -L$(LIB_DIR) -l$(LIB_NAME)


.PHONY: test run

#running the program
test: $(EXEC_TEST)
	valgrind ./$(EXEC_TEST)

run: $(EXEC)
	./$(EXEC)


.PHONY: clean

#cleaning the directory
clean:
	rm -f *.o  *~ *.a *.so *.x *h.gch core core* a.out; rm -rf ${LIB_DIR}
