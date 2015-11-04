#ifndef MemoryMappedDevice_p_h__
#define MemoryMappedDevice_p_h__

#include <QPointer>
#include <QHash>
#include <QMap>
#include "MemoryMappedDevice.h"
class QIODevice;
class MemoryMappedDevicePrivate
{
    DECLARE_PRIVATE_COMMONS(MemoryMappedDevice)
protected:
    QPointer<QIODevice> m_device;
    QHash<qint32, qint64> m_itemsMap;
    QMap<qint64, bool> m_memoryMap;
    MemoryMappedDevice* q_ptr;
};

#endif // MemoryMappedDevice_p_h__
