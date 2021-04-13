#ifndef SIGNAL_H
#define SIGNAL_H
#include "common.h"

typedef struct Signal  Signal;
typedef struct Emitter Emitter;

Signal* signal_new();
void    signal_destroy(Signal* self);
void    signal_connect(Signal* self, Callback callback);
void    signal_disconnect(Signal* self, void (*fn)());
void    signal_emit(Signal* self, const void* data);

Emitter* emitter_new(u32 num_singals);
void     emitter_delete(Emitter* emitter);
void     emitter_connect(Emitter* self, int sig, Callback callback);
void     emitter_disconnect(Emitter* self, int sig, void (*fn)());
void     emitter_emit(Emitter* self, int sig, const void* data);

#endif // SIGNAL_H
