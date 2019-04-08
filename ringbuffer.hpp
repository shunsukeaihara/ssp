#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_
#include <string.h>
#include <algorithm>

namespace ssp {

template <class T>
class RingBuffer {
   public:
    RingBuffer(int capasity) : _capasity(capasity) {
        _buffer = new T[capasity];
        std::fill_n(_buffer, _capasity, 0);
        _writeHead = 0;
        _readHead = 0;
        _pos = 0;
    }
    virtual ~RingBuffer() {
        delete _buffer;
    }

    inline T operator[](const int index) const {
        // 読み込み開始位置からの相対アクセス
        // 書き込んでいない領域のケアが必要だけど実用上は気にしなくても良い
        // エラーハンドリングを正しくやりたければオーバーロードをやめてポインタで値を受け取るのが良い
        // あっても数十のインデックス幅でのアクセスなのであんまり気にしなくても良いけど無駄にmodを取っている
        return _buffer[relativeToabsolute(index)];
    }

    inline T &operator[](const int index) {
        return _buffer[relativeToabsolute(index)];
    }

    inline T *at(const int index) {
        return &_buffer[relativeToabsolute(index)];
    }

    inline T get(const int index) const {
        return _buffer[relativeToabsolute(index)];
    }

    inline void set(const int index, T val) {
        _buffer[relativeToabsolute(index)] = val;
    }

    void movePos(int offset) {
        // -を想定していない
        int moved = _pos + offset;
        if (moved < _capasity) {
            _pos = moved;
        } else {
            // capasityを2回目超えてmoveするようなことが起きるとハチャメチャな状態だけどmodとってもおかしくはらない
            _pos = moved % _capasity;
        }
    }

    void moveWritePos(int offset) {
        // -を想定していない
        int moved = _writeHead + offset;
        if (moved < _capasity) {
            _writeHead = moved;
        } else {
            _writeHead = moved % _capasity;
        }
    }

    void write(T *in, int len) {
        int rest = _capasity - _writeHead;
        if (rest > len) {
            memcpy(&_buffer[_writeHead], in, sizeof(T) * len);
            _writeHead += len;
        } else {
            memcpy(&_buffer[_writeHead], in, sizeof(T) * rest);
            int writeRest = len - rest;
            memcpy(_buffer, &in[rest], sizeof(T) * writeRest);
            _writeHead = writeRest;
        }
    }

    int read(T *out, int len) {
        int readSize = _read(out, len);
        if (readSize > 0) {
            _readHead = relativeToabsoluteReadPos(readSize);
        }
        return readSize;
    }

    int readWithSlide(T *out, int len, int offset) {
        int readSize = _read(out, len);
        if (readSize > 0) {
            _readHead = relativeToabsoluteReadPos(offset);
        }
        return readSize;
    }

    bool filled(int len) {
        if (_writeHead >= _readHead) {
            return (_writeHead - _readHead) >= len;
        } else {
            return (_capasity - _readHead + _writeHead) >= len;
        }
    }

    bool filledFromCurrentPos(int len) {
        if (_writeHead >= _pos) {
            return (_writeHead - _pos) >= len;
        } else {
            return (_capasity - _pos + _writeHead) >= len;
        }
    }

    size_t filledCount() {
        if (_writeHead >= _readHead) {
            return _writeHead - _readHead;
        } else {
            return _capasity - _readHead + _writeHead;
        }
    }

    int getCurrentPos() {
        return _pos;
    }
    int getCurrentReadHead() {
        return _readHead;
    }

    size_t getCapasity() {
        return _capasity;
    }

    void reset() {
        std::fill_n(_buffer, _capasity, 0);
        _pos = 0;
        _writeHead = 0;
        _readHead = 0;
    }

   private:
    const int _capasity;
    T *_buffer;
    int _writeHead;
    int _readHead;
    int _pos;

    int relativeToabsoluteReadPos(const int index) const {
        int mod = (index + _readHead) % _capasity;
        if (mod >= 0) {
            return mod;
        } else {
            return _capasity + mod;
        }
    }

    int relativeToabsolute(const int index) const {
        int mod = (index + _pos) % _capasity;
        if (mod >= 0) {
            return mod;
        } else {
            return _capasity + mod;
        }
    }

    int _read(T *out, int len) {
        // 次回の開始indexを返す。読めてなかったら-1を返す
        if (!filled(len)) {
            return -1;
        }
        if (_writeHead > _readHead) {
            // 追い抜いていない
            memcpy(out, &_buffer[_readHead], sizeof(T) * len);
            return len;
        } else {
            // 追い抜いている
            int rest = _capasity - _readHead;
            if (rest > len) {
                memcpy(out, &_buffer[_readHead], sizeof(T) * len);
                return len;
            } else {
                memcpy(out, &_buffer[_readHead], sizeof(T) * rest);
                memcpy(&out[rest], _buffer, sizeof(T) * (len - rest));
                return len;
            }
        }
    }
};
}  // namespace ssp

#endif /* RING_BUFFER_H_ */