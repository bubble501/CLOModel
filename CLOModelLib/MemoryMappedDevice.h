#ifndef MemoryMappedDevice_h__
#define MemoryMappedDevice_h__
#include <tuple>
#include <QByteArray>
#include <QDataStream>
#include "CommonFunctions.h"
class QIODevice;
class MemoryMappedDevicePrivate;
class CLOMODELLIB_EXPORT MemoryMappedDevice
{
    DECLARE_PUBLIC_COMMONS(MemoryMappedDevice)
public:
    MemoryMappedDevice(QIODevice* source);
    void setDevice(QIODevice* source = nullptr);
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
    void clear();
    bool removeValue(qint32 key);
    QList<qint32> keys() const;
    bool contains(qint32 key) const;
    qint32 size() const;
    qint64 fileSize() const;
    bool isEmpty() const;
protected:
    MemoryMappedDevicePrivate* d_ptr;
    bool writeBlock(qint32 key, const QByteArray& block);
    QByteArray readBlock(qint32 key) const;
    qint64 writeInMap(const QByteArray& block);

};
#endif // MemoryMappedDevice_h__

