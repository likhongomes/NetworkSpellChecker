CC = g++


TARGET:=spell

$(TARGET):spellChecker.cc open_listenfd.cc		
	@$(CC) $(CFLAGS) $^ -o $@



.PHONY: all
all:$(TARGET)
	@./$(TARGET)

test: example.c
	@$(CC) $(CFLAGS) $^ -o $@ -D TESTING
	@./test

clean:
	rm example







