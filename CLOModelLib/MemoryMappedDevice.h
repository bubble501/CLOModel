#ifndef MemoryMappedDevice_h__
#define MemoryMappedDevice_h__

#include <QHash>
#include <QMap>
#include <tuple>
#include <QByteArray>
#include <QDataStream>
#include <QPointer>
#include "clomodellib_global.h"
class QIODevice;
class CLOMODELLIB_EXPORT MemoryMappedDevice
{
public:
    MemoryMappedDevice(QIODevice* source);
    virtual ~MemoryMappedDevice();
    template <class T> bool setValue(qint32 key, const T& val){
        QByteArray block;
        {
            QDataStream writerStream(&block, QIODevice::WriteOnly);
            writerStream << val;
        }
        return writeBlock(key, block);
    }
    template <class T> std::tuple<bool,T> value(qint32 key) const {
        static_assert(std::is_default_constructible<T>::value, "T must provide a default constructor");
        T result;
        QByteArray block = readBlock(key);
        if (block.isEmpty())
            return std::make_tuple(false, result);
        QDataStream readerStream(block);
        readerStream >> result;
        return std::make_tuple(true,result);
    }
    bool removeValue(qint32 key);
    QList<qint32> itemKeys() const;
protected:
    QPointer<QIODevice> m_device;
    QHash<qint32, qint64> m_itemsMap;
    QMap<qint64, bool> m_memoryMap;
    bool writeBlock(qint32 key, const QByteArray& block);
    QByteArray readBlock(qint32 key) const;
    qint64 writeInMap(const QByteArray& block);

};
#endif // MemoryMappedDevice_h__

