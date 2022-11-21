#ifndef ORANGE_OS_KQUEUE_H_
#define ORANGE_OS_KQUEUE_H_
#include "types.h"
#include "klib.h"
#define KQUEUE_INIT(buf, tol) {0, 0, tol, buf}

typedef struct {
    size_t front, rear; /* Index of head and rear in buf */
    size_t tol;        /* Total capacity of buf (total size / esize)*/
    void *buf;
} kqueue;

static inline void kqueue_clear(kqueue *kq) {
    kq->front = kq->rear = 0;
}

static inline void kqueue_push(kqueue *kq, void *ele, size_t esize) {
    memcpy(kq->buf + kq->rear * esize, ele, esize);
    ++kq->rear;
    if (kq->rear == kq->tol)
        kq->rear = 0;
}

static inline void kqueue_pop(kqueue *kq) {
    kq->front = kq->front == kq->tol - 1 ? 0 : kq->front + 1;
}

static inline int kqueue_empty(kqueue *kq) {
    return kq->front == kq->rear;
}

static inline int kqueue_full(kqueue *kq) {
    return kq->rear + 1 == kq->front || (kq->front == 0 && kq->rear + 1 == kq->tol);
}

static inline void *kqueue_front(kqueue *kq, size_t esize) {
    return kq->buf + kq->front * esize;
}

#endif // ORANGE_OS_KQUEUE_H_