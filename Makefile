build_dir = ./build
src_dir = ./src

CXX = g++
CXXFLAGS = -Wall -Wno-unused-result -Werror=return-type -Werror=main -pipe -D_FORTIFY_SOURCE=2 -fno-delete-null-pointer-checks -std=c++20 -I $(src_dir)

ifdef DEBUG
	CXXFLAGS += -g -DDEBUG
else
	CXXFLAGS += -O0
endif

src_files = $(shell find $(src_dir) -name '*.cpp')

OBJS := $(src_files:%=$(build_dir)/%.o)

standard: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(build_dir)/$@ $(OBJS)

$(build_dir)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(build_dir)