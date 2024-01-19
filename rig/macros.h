#ifndef RIG_MACROS_H
#define RIG_MACROS_H

#define QSTRING_FREQ(f) (QString::number((f), 'f', 5))
#define Hz2MHz(f) ((double)((f)/1e6))
#define mW2W(f) ((double)((f)/1000.0))
#ifndef MHz
#define MHz(f)  ((double)((f)*(double)1000000))
#endif

#endif // RIG_MACROS_H
