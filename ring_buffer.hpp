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

    void movePos(size_t offset){
        size_t moved = _pos + offset;
        if (moved < _capasity){
            _pos = moved;
        }else{
            _pos = (moved - _capasity) % _capasity;
        }
    }

    int read(T *out, size_t len){
        // 正しく読めたら0が返る
        if (!filled(len)){
            return -1;
        }
        if (_writeHead > _readHead){
            // 追い抜いていない
            memcpy(&_buffer[_readHead], out, sizeof(T) * len);
            _readHead += len;
            return 0;
        }else{
            // 追い抜いている
            size_t rest = _capasity - _readHead;
            if (rest > len){
                memcpy(&_buffer[_readHead], out, sizeof(T) * len);
                _readHead += len;
                return 0;
            }else{
                memcpy(&_buffer[_readHead], out, sizeof(T) * rest);
                memcpy(_buffer, &out[rest], sizeof(T) * (len - rest));
                _readHead = rest;
                return 0;
            }
        }
    }

    bool filled(size_t len){
        if (_writeHead > _readHead){
            return (_writeHead - _readHead) > len;
        }else{
            return (_capasity - _readHead + _writeHead) > len;
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
};
}

#endif /* RING_BUFFER_H_ */