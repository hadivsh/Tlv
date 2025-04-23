# نام فایل اجرایی
TARGET = TestTlv

# فایل‌های سورس
SRCS = TestTlv.c Tlv.c

# فایل‌های هدر
HDRS = Tlv.h

# فایل‌های آبجکت
OBJS = $(SRCS:.c=.o)

# کامپایلر
CC = gcc

# فلگ‌های کامپایلر
CFLAGS = -Wall -Wextra -Werror

# قانون پیش‌فرض برای کامپایل و اجرا
all: $(TARGET) run

# قانون کامپایل فایل اجرایی
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# قانون کامپایل فایل‌های آبجکت
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# قانون اجرا
run: $(TARGET)
	./$(TARGET)

# قانون تمیز کردن (پاک کردن فایل‌های اجرایی و آبجکت)
clean:
	rm -f $(OBJS) $(TARGET)