#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <algorithm>

namespace ssp{

template<class T>
class RingBuffer{
public:
    RingBuffer(size_t capasity): _capasity(capasity){
        _buffer = new T[capasity]();
        _writeHead = 0;
        _readHead = 0;
        _pos = 0;
    }
    virtual ~RingBuffer(){
        delete _buffer;
    }


    inline T operator[](const int index) const{
        //読み込み開始位置からの相対アクセス
        // 書き込んでいない領域のケアが必要だけど実用上は気にしなくても良い
        // エラーハンドリングを正しくやりたければオーバーロードをやめてポインタで値を受け取るのが良い
        // あっても数十のインデックス幅でのアクセスなのであんまり気にしなくても良いけど無駄にmodを取っている
        return _buffer[relativeToabsolute(index)];
    }

    inline T& operator[](const int index) {
        return _buffer[relativeToabsolute(index)];
    }

    void movePos(size_t offset){
        size_t moved = _pos + offset;
        if (moved < _capasity){
            _pos = moved;
        }else{
            // capasityを2回目超えてmoveするようなことが起きるとハチャメチャな状態だけどmodとってもおかしくはらない
            _pos = (moved - _capasity) % _capasity;
        }
    }

    void moveWritePos(size_t offset){
        size_t moved = _writePos + offset;
        if (moved < _capasity){
            _writePos = moved;
        }else{
            // capasityを2回目超えてmoveするようなことが起きるとハチャメチャな状態だけどmodとってもおかしくはらない
            _writePos = (moved - _capasity) % _capasity;
        }
    }

    void write(T *in, size_t len){
        size_t rest = _capasity - _writeHead;
        if (rest > len){
            memcpy(_buffer, in, sizeof(T)*len);
            _writeHead += len;
        }else{
            memcpy(&_buffer[_writeHead], *in, sizeof(T) * rest);
            size_t writeRest = len - rest;
            memcpy(_buffer, &in[writeRest], sizeof(T) * writeRest);
            _writeHead = writeRest;
        }
    }

    int read(T *out, size_t len){
        int readSize = _read(out, len);
        if (readSize > 0){
            _readPos = relativeToabsolute(readSize);
        }
        return readSize;
    }

    int readWithSlide(T *out, size_t len, int offset){
        int readSize = _read(out, len);
        if (readSize > 0){
            _readPos = relativeToabsolute(offset);
        }
        return readSize;
    }

    bool filled(size_t len){
        if (_writeHead > _readHead){
            return (_writeHead - _readHead) > len;
        }else{
            return (_capasity - _readHead + _writeHead) > len;
        }
    }

    bool filledFromCurrentPos(size_t len){
        if (_writeHead > _pos){
            return (_writeHead - _pos) > len;
        }else{
            return (_capasity - _pos + _writeHead) > len;
        }
    }

    void reset(){
        std::fill_n(_buffer, _capasity, 0);
        _pos = 0;
        _writeHead = 0;
        _readHead = 0;
    }

private:
    const size_t _capasity;
    T *_buffer;
    int _writeHead;
    int _readHead;
    int _pos;

    inline int relativeToabsolute(const int index){
        int pos = index + _pos;
        if (pos >= 0){
            return pos % _capasity;
        }else{
            int mod = pos % _capasity;
            if (mod != 0){
                return _capasity - mod;
            }else{
                return 0;
            }
        }
    }

    int _read(T *out, size_t len){
        // 次回の開始indexを返す。読めてなかったら-1を返す
        if (!filled(len)){
            return -1;
        }
        if (_writeHead > _readHead){
            // 追い抜いていない
            memcpy(out, &_buffer[_readHead], sizeof(T) * len);
            return len;
        }else{
            // 追い抜いている
            size_t rest = _capasity - _readHead;
            if (rest > len){
                memcpy(out, &_buffer[_readHead], sizeof(T) * len);
                return len;
            }else{
                memcpy(out, &_buffer[_readHead], sizeof(T) * rest);
                memcpy(&out[rest], _buffer, sizeof(T) * (len - rest));
                _readHead = rest;
                return len;
            }
        }
    }
};
}

#endif /* RING_BUFFER_H_ */