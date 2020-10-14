.POSIX:

include config.mk

# Sources
CPP_FILES = $(shell find src -name "*.cpp")
HPP_FILES = $(shell find {src,include} -name "*.hpp")

# Objects
CPP_OBJ = $(CPP_FILES:.cpp=.o)

OBJ = $(CPP_OBJ)

# Dependency files
DEP = $(OBJ:.o=.d)

all: $(NAME)

# Compilation
%.o: %.cpp
	$(CXX) $(CPP_FLAGS) -MT $@ -MF $*.d -MMD -o $@ -c $<

# Linking
$(NAME): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

# Dependencies
-include $(DEP)

debug: CPP_FLAGS += -DDEBUG -g
debug: $(NAME)

clean:
	rm -f $(NAME) $(NAME).asm $(OBJ) $(DEP)

run: $(NAME)
	./$(NAME)

format:
	clang-format -i --style=$(CLANG_FORMAT_STYLE) $(CPP_FILES) $(HPP_FILES)

install:
	cp -r include/poacher $(PREFIX)

.PHONY: all clean echo
