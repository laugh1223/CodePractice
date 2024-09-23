#设置编译工具路径
CROSS_COMPILER = ""
#设置编译器
CC  = ${CROSS_COMPILER}/
CXX = ${CROSS_COMPILER}/
#设置链接器
LD  = ${CROSS_COMPILER}/
#设置头文件路径
INCLUDE = -I
CXXFLAGS += $(INCLUDE)
#设置库路径
LIBPATH = -L 
#设置需要使用的库名
LIBS = -l
LDFLAGES = $(LIBPATH) $(LIBS)
#设置目标和依赖
SRC    := $(wildcard *.c *.cpp)
TARGET  = main
OBJECTS = $(patsubst %.c,%.o,$(SRC))

$(TARGET):$(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

.cpp .o :
	$(CC) $(CFLAGS) -c $< -o $@
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

#清除操作
.PHONY:clean
clean:
	-rm -rf $(TARGET)
	-rm -rf $(OBJECTS)
