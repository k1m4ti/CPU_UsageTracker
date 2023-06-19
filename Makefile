#files extensions
.SUFFIXES: .c .o .x .h

#flags
GCC_FLAGS = -Wall -Wextra -std=c99 -pedantic -O -pthread
CLANG_FLAGS = -Weverything -std=c99 -O -pthread -Wno-disabled-macro-expansion -Wno-missing-noreturn

FLAGS = $(CLANG_FLAGS)

#compiler and linker (type clang or gcc)
CO = clang
LD = $(CO)

#files names
FUNCTIONS = functions
READER = reader
PRINTER = printer
ANALYZER = analyzer
WATCHDOG = watchdog
MAIN = main
LOGGER = logger

TEST_MAIN = main_test
TEST_FUNCTIONS = functions_test
TEST_READER = reader_test
TEST_ANALYZER = analyzer_test
TEST_PRINTER = printer_test
TEST_WATCHDOG = watchdog_test
TEST_LOGGER = logger_test

#executive programs names
EXEC1 = prog.x
EXEC2 = test.x


# object files
FUNCTIONS_O = $(FUNCTIONS).o
MAIN_O = $(MAIN).o
READER_O = $(READER).o
ANALYZER_O = $(ANALYZER).o
PRINTER_O = $(PRINTER).o
WATCHDOG_O = $(WATCHDOG).o
LOGGER_O = $(LOGGER).o

TEST_FUNCTIONS_O = $(TEST_FUNCTIONS).o
TEST_MAIN_O = $(TEST_MAIN).o
TEST_READER_O = $(TEST_READER).o
TEST_ANALYZER_O = $(TEST_ANALYZER).o
TEST_PRINTER_O = $(TEST_PRINTER).o
TEST_WATCHDOG_O = $(TEST_WATCHDOG).o
TEST_LOGGER_O = $(TEST_LOGGER).o

#header file
HEAD = $(FUNCTIONS).h

#libraries names
LIB_NAME = $(FUNCTIONS)
TEST_LIB_NAME = $(TEST_FUNCTIONS)

#dir for libraries
LIB_DIR = ./lib

#static libraries
LIB_STAT = lib$(LIB_NAME).a
TEST_LIB_STAT = lib$(TEST_LIB_NAME).a

#compiled files for libraries
LIB_OBJS = $(FUNCTIONS_O) $(READER_O) $(ANALYZER_O) $(PRINTER_O) $(WATCHDOG_O) $(LOGGER_O)
TEST_LIB_OBJS = $(TEST_FUNCTIONS_O) $(TEST_READER_O) $(TEST_ANALYZER_O) $(TEST_PRINTER_O) $(TEST_WATCHDOG_O) $(TEST_LOGGER_O)

#static library archiver for creating libraries
AR = ar

ARFLAGS = rsv


#pattern rule
%.o: %.c %.h
	$(CO) $(FLAGS) -c $<
%.o: %.c
	$(CO) $(FLAGS) -c $<


#creating libraries
$(LIB_STAT): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $?
	mkdir -p $(LIB_DIR)
	mv $(LIB_STAT) $(LIB_DIR)

$(TEST_LIB_STAT): $(TEST_LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $?
	mkdir -p $(LIB_DIR)
	mv $(TEST_LIB_STAT) $(LIB_DIR)	


#executive programs
$(EXEC1): $(MAIN_O) $(LIB_STAT)
	$(LD) -o $@ $(FLAGS) $(MAIN_O) -L$(LIB_DIR) -l$(LIB_NAME)

$(EXEC2): $(TEST_MAIN_O) $(TEST_LIB_STAT)
	$(LD) -o $@ $(FLAGS) $(TEST_MAIN_O) -L$(LIB_DIR) -l$(TEST_LIB_NAME)	

.PHONY: run	test

# running program
run: $(EXEC1)
	./$(EXEC1)

test: $(EXEC2)
	valgrind ./$(EXEC2)

.PHONY: clean tar

#cleaning directory
clean:                                                     
	rm -f *.o  *~ *.a *.so *.x *h.gch core core* a.out; rm -rf ${LIB_DIR}
