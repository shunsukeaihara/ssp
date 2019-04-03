CC :=g++
CFLAGS := -I. -Wall
LDFLAGS :=

test: test_butterworth test_resampler

test_butterworth:
	mkdir -p test
	$(CC) $(CFLAGS) $(LDFLAGS) -o test/butterworth_test butterworth_test.cpp
	test/butterworth_test

test_resampler:
	mkdir -p test
	$(CC) $(CFLAGS) $(LDFLAGS) -o test/resampler_test resampler_test.cpp
	test/resampler_test


clean:
	rm -rf *.o
	rm -rf esola
	rm -rf test/*
