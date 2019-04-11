CC :=g++
CFLAGS := -I. -Wall
LDFLAGS :=

test: test_butterworth sample_resampler sample_esola test_ringbuffer sample_pitchshifter

test_butterworth:
	mkdir -p test
	$(CC) $(CFLAGS) $(LDFLAGS) -o test/butterworth_test butterworth_test.cpp
	test/butterworth_test

sample_resampler:
	mkdir -p test
	$(CC) $(CFLAGS) -O3 $(LDFLAGS) -o test/resampler_sample resampler_sample.cpp

sample_esola:
	mkdir -p test
	$(CC) $(CFLAGS) -O3 $(LDFLAGS) -o test/esola_sample esola_sample.cpp
	# sox sample.wav -t raw - | test/esola_sample |play -r 16000 -b 16 -c 1 -e signed -t raw -

sample_pitchshifter:
	mkdir -p test
	$(CC) $(CFLAGS) -O3 $(LDFLAGS) -o test/pitchshifter_sample pitchshifter_sample.cpp
	# sox sample.wav -t raw - | test/esola_sample |play -r 16000 -b 16 -c 1 -e signed -t raw -

test_ringbuffer:
	mkdir -p test
	$(CC) $(CFLAGS) $(LDFLAGS) -o test/ringbuffer_test ringbuffer_test.cpp
	test/ringbuffer_test

clean:
	rm -rf *.o
	rm -rf esola
	rm -rf test/*
