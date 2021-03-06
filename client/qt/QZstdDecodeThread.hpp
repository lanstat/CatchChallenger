#ifndef QZstdDecodeThread_h
#define QZstdDecodeThread_h

#ifndef NOTHREADS
#include <QThread>
#else
#include <QObject>
#endif
#include "../tarcompressed/ZstdDecode.hpp"

/// \brief to decode the xz via a thread
class QZstdDecodeThread :
        #ifndef NOTHREADS
        public QThread
        #else
        public QObject
        #endif
        , public ZstdDecode
{
        Q_OBJECT
        public:
                QZstdDecodeThread();
                ~QZstdDecodeThread();
        #ifndef NOTHREADS
        protected:
        #else
        public:
        #endif
                void run();
        signals:
                void decodedIsFinish();
};

#endif // QZstdDecodeThread_h
