#include <assert.h>
#include <common.hpp>
#include <ringbuffer.hpp>

using namespace ssp;

int main() {
    RingBuffer<float> buffer = RingBuffer<float>(10);
    float in[3] = {0.0, 1.0, -1.0};
    float out[3] = {};
    float ref[3] = {1.0, 2.0, 3.0};
    buffer.write(in, 3);
    setEpochMark(buffer.at(0));
    assert(isEpochMark(buffer[0]));
    unsetEpochMark(buffer.at(0));
    assert(!isEpochMark(buffer[0]));
    assert(buffer.filled(3));

    assert(buffer[0] == in[0]);
    buffer.movePos(1);
    assert(buffer[0] == in[1]);
    assert(!buffer.filledFromCurrentPos(3));
    buffer.reset();
    for (int x = 0; x < 1000; x++) {
        buffer.write(in, 3);
        buffer.read(out, 3);
        for (int i = 0; i < 3; i++) {
            assert(in[i] == out[i]);
        }
        std::fill_n(out, 3, 0);
        assert(!buffer.filled(1));
    }
    buffer.reset();
    for (int i = 0; i < 3; i++) {
        buffer[i] = ref[i];
    }
    buffer.moveWritePos(3);
    assert(buffer.filled(3));

    buffer.read(out, 3);
    for (int i = 0; i < 3; i++) {
        assert(ref[i] == out[i]);
    }
    buffer.reset();
    for (int i = 0; i < 3; i++) {
        buffer[i] = ref[i];
        assert(buffer[i] == ref[i]);
    }
    buffer.reset();
    buffer.write(in, 3);
    buffer.write(ref, 3);
    buffer.movePos(3);
    for (int i = 0; i < 3; i++) {
        assert(buffer[i] == ref[i]);
    }
    RingBuffer<short> buffer2 = RingBuffer<short>(10);
    buffer2[0] = 1;
    buffer2[1] = 2;
    buffer2[2] = 3;
    buffer2.movePos(3);
    buffer2[0] = 4;
    buffer2[1] = 5;
    buffer2[2] = 6;
    for (int i = -3; i < 3; i++) {
        assert(buffer2[i] == (i + 4));
    }
    RingBuffer<short> buffer3 = RingBuffer<short>(10);
    for (int x = 0; x < 100; x++) {
        for (int i = 0; i < 10; i++) {
            buffer3[i] = i;
        }
        buffer3.movePos(10);
        for (int i = 0; i < 10; i++) {
            assert(buffer3[i - 10] == i);
        }
    }
}