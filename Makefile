build_dir = ./bin
src_dir = ./src

CXX = g++
CXXFLAGS = -Wall -Wextra -Wconversion -Werror -pipe -std=c++20 -I $(src_dir)

ifdef DEBUG
	CXXFLAGS += -g -DDEBUG
else
	CXXFLAGS += -O3 -march=native
endif

ifdef PROFILE
	CXXFLAGS += -ggdb
endif

src_files = $(shell find $(src_dir) -name '*.cpp')

OBJS := $(src_files:%=$(build_dir)/%.o)

solver: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(build_dir)/$@ $(OBJS)

$(build_dir)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(build_dir)