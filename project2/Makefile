CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
CC_FLAGS := -std=c++11
CC_FLAGS +=	-Wall -Wextra
CC_FLAGS += -O3
LD_FLAGS := -Lsrc/crypto++
LD_LIBS := -lcryptopp

bin/ObjectSecurity: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^ $(LD_LIBS)

obj/%.o: src/%.cpp
	g++ $(CC_FLAGS) -c -o $@ $<
	
clean:
	rm -rf obj/*.o obj/*.d bin/ObjectSecurity
	
CC_FLAGS += -MMD
-include $(OBJFILES:.o=.d)
