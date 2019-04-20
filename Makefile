CC :=g++
CFLAGS := -I. -Wall
LDFLAGS :=

test: test_butterworth test_ringbuffer test_delay sample_pitchshifter sample_zfr sample_resampler sample_timestretch

test_butterworth:
	mkdir -p tests/bin/
	$(CC) $(CFLAGS) $(LDFLAGS) -o tests/bin/butterworth_test tests/butterworth_test.cpp
	tests/bin/butterworth_test

test_ringbuffer:
	mkdir -p tests/bin/
	$(CC) $(CFLAGS) $(LDFLAGS) -o tests/bin/ringbuffer_test tests/ringbuffer_test.cpp
	tests/bin/ringbuffer_test

test_delay:
	mkdir -p tests/bin/
	$(CC) $(CFLAGS) $(LDFLAGS) -o tests/bin/delay_test tests/delay_test.cpp
	tests/bin/delay_test

sample_zfr:
	mkdir -p samples/bin/
	$(CC) $(CFLAGS) $(LDFLAGS) -o samples/bin/zfr_sample samples/zfr_sample.cpp

sample_resampler:
	mkdir -p samples/bin/
	$(CC) $(CFLAGS) -O3 $(LDFLAGS) -o samples/bin/resampler_sample samples/resampler_sample.cpp

sample_timestretch:
	mkdir -p samples/bin/
	$(CC) $(CFLAGS) -O3 $(LDFLAGS) -o samples/bin/timestretch_sample samples/timestretch_sample.cpp
	# sox sample.wav -t raw - | samples/bin/timestretch_sample |play -r 16000 -b 16 -c 1 -e signed -t raw -

sample_pitchshifter:
	mkdir -p samples/bin/
	$(CC) $(CFLAGS) -O3 $(LDFLAGS) -o samples/bin/pitchshifter_sample samples/pitchshifter_sample.cpp
	# sox sample.wav -t raw - | samples/bin/esola_sample |play -r 16000 -b 16 -c 1 -e signed -t raw -


clean:
	rm -rf tests/bin/*
	rm -rf samples/bin/*
