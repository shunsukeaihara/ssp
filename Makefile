CC :=g++
CFLAGS := -I. -Wall
LDFLAGS :=

test: test_butterworth test_resampler test_esola test_ringbuffer

test_butterworth:
	mkdir -p test
	$(CC) $(CFLAGS) $(LDFLAGS) -o test/butterworth_test butterworth_test.cpp
	test/butterworth_test

test_resampler:
	mkdir -p test
	$(CC) $(CFLAGS) $(LDFLAGS) -o test/resampler_test resampler_test.cpp
	test/resampler_test

test_esola:
	mkdir -p test
	$(CC) $(CFLAGS) -g -O0 $(LDFLAGS) -o test/esola_test esola_test.cpp
	# test/esola_test

test_ringbuffer:
	mkdir -p test
	$(CC) $(CFLAGS) $(LDFLAGS) -o test/ringbuffer_test ringbuffer_test.cpp
	test/ringbuffer_test


clean:
	rm -rf *.o
	rm -rf esola
	rm -rf test/*
