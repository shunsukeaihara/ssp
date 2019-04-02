#ifndef COMMON_H_
#define COMMON_H_
namespace ssp{

template <typename T> inline int isEpochMark(const T in){
    int *p = (int*)(in);
    return (*p) & (1<<0);
}

template <typename T> inline void setEpochMark(T &in){
    int *p = (int*)(in);
    (*p) |= (1<<0);
}

template <typename T> inline void unsetEpochMark(T &in){
    int *p = (int*)(in);
    (*p) &= ~(1<<0);
}

}
#endif