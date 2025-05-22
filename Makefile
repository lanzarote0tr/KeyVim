CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC = main.c helper.c inputhandler.c outputhandler.c
OBJ = $(SRC:.c=.o)

TARGET = a.out
TEST_TARGET = test_helper

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# 테스트용 빌드 (조건부 main 사용)
testhelper: helper.c
	$(CC) $(CFLAGS) -DHEADER_TEST helper.c -o $(TEST_TARGET)

testinputhandler: inputhandler.c
	$(CC) $(CFLAGS) -DHEADER_TEST inputhandler.c -o $(TEST_TARGET)

testoutputhandler: outputhandler.c
	$(CC) $(CFLAGS) -DHEADER_TEST outputhandler.c -o $(TEST_TARGET)

# 개별 오브젝트 파일 만들기
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 클린업
clean:
	rm -f *.o $(TARGET) $(TEST_TARGET)

