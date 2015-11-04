#include "MemoryMappedDevice.h"
#include "Private/MemoryMappedDevice_p.h"
#include <QIODevice>
MemoryMappedDevicePrivate::MemoryMappedDevicePrivate(MemoryMappedDevice *q)
	:q_ptr(q)
    ,m_device(nullptr)
{}
MemoryMappedDevice::MemoryMappedDevice()
    : MemoryMappedDevice(new MemoryMappedDevicePrivate(this))
{}
MemoryMappedDevice::MemoryMappedDevice(MemoryMappedDevicePrivate *d)
	:d_ptr(d)
{
	
}

MemoryMappedDevicePrivate::~MemoryMappedDevicePrivate()
{
    if (m_device) {
        if (m_device->isOpen()) {
            m_device->close();
        }
    }
}
MemoryMappedDevice::MemoryMappedDevice(QIODevice* source)
    :MemoryMappedDevice()
{
    setDevice(source);
}

MemoryMappedDevice::~MemoryMappedDevice()
{
    
}

void MemoryMappedDevice::setDevice(QIODevice* source)
{
    Q_D(MemoryMappedDevice);
    if (d->m_device) {
        if (d->m_device->isOpen()) {
            d->m_device->close();
        }
    }
    d->m_itemsMap.clear();
    d->m_memoryMap.clear();
    d->m_memoryMap.insert(0, true);
    d->m_device = source;
    if (d->m_device) {
        if (d->m_device->isSequential()) {
            Q_ASSERT_X(false, "MemoryMappedDevice()", "Device must be seekable");
            d->m_device = nullptr;
        }
        if (!d->m_device->isOpen()) {
            d->m_device->open(QIODevice::ReadWrite);
        }
    }

}

void MemoryMappedDevice::clear()
{
    Q_D(MemoryMappedDevice);
    if (d->m_device) {
        if (d->m_device->isOpen()) {
            d->m_device->close();
        }
        d->m_device->open(QIODevice::Truncate);
        d->m_device->close();
        d->m_device->open(QIODevice::ReadWrite);
    }
    d->m_itemsMap.clear();
    d->m_memoryMap.clear();
    d->m_memoryMap.insert(0, true);
}

bool MemoryMappedDevice::removeValue(qint32 key)
{
    Q_D(MemoryMappedDevice);
    auto itemIter = d->m_itemsMap.find(key);
    if (itemIter == d->m_itemsMap.end())
        return false;
    auto fileIter = d->m_memoryMap.find(itemIter.value());
    Q_ASSERT(fileIter != d->m_memoryMap.end());
    if (fileIter.value())
        return false;
    d->m_itemsMap.erase(itemIter);
    if (fileIter != d->m_memoryMap.begin()) {
        if ((fileIter - 1).value()) {
            d->m_memoryMap.erase(fileIter);
            return true;
        }
    }
    if ((fileIter + 1) != d->m_memoryMap.end()) {
        if ((fileIter + 1).value()) 
            d->m_memoryMap.erase(fileIter + 1);
    }
    fileIter.value() = true;
    return true;
}

QList<qint32> MemoryMappedDevice::keys() const
{
    Q_D(const MemoryMappedDevice);
    return d->m_itemsMap.keys();
}

bool MemoryMappedDevice::contains(qint32 key) const
{
    Q_D(const MemoryMappedDevice);
    return d->m_itemsMap.contains(key);
}

qint32 MemoryMappedDevice::size() const
{
    Q_D(const MemoryMappedDevice);
    return d->m_itemsMap.size();
}

qint64 MemoryMappedDevice::fileSize() const
{
    Q_D(const MemoryMappedDevice);
    return d->m_device->size();
}

bool MemoryMappedDevice::isEmpty() const
{
    Q_D(const MemoryMappedDevice);
    return d->m_itemsMap.isEmpty();
}

bool MemoryMappedDevice::writeBlock(qint32 key, const QByteArray& block)
{
    Q_D(MemoryMappedDevice);
    if (!d->m_device)
        return false;
    if (!d->m_device->isOpen()) {
        if (!d->m_device->open(QIODevice::ReadWrite))
            return false;
    }
    if (!d->m_device->isWritable())
        return false;
    d->m_device->setTextModeEnabled(false);
    auto itemIter = d->m_itemsMap.find(key);
    if (itemIter != d->m_itemsMap.end()) {
        removeValue(key);
    }
    d->m_itemsMap.insert(key, writeInMap(block));
    return true;
}

QByteArray MemoryMappedDevice::readBlock(qint32 key) const
{
    Q_D(const MemoryMappedDevice);
    if (!d->m_device)
        return QByteArray();
    if (!d->m_device->isOpen()) {
        if (!d->m_device->open(QIODevice::ReadWrite))
            return QByteArray();
    }
    if (!d->m_device->isReadable())
        return QByteArray();
    d->m_device->setTextModeEnabled(false);
    auto itemIter = d->m_itemsMap.constFind(key);
    if (itemIter == d->m_itemsMap.constEnd())
        return QByteArray();
    auto fileIter = d->m_memoryMap.constFind(itemIter.value());
    Q_ASSERT(fileIter != d->m_memoryMap.constEnd());
    if(fileIter.value())
        return QByteArray();
    auto nextIter = fileIter + 1;
    d->m_device->seek(fileIter.key());
    if (nextIter == d->m_memoryMap.constEnd())
        return d->m_device->readAll();
    return d->m_device->read(nextIter.key() - fileIter.key());
}

qint64 MemoryMappedDevice::writeInMap(const QByteArray& block)
{
    Q_D(MemoryMappedDevice);
    for (auto i = d->m_memoryMap.begin(); i != d->m_memoryMap.end(); ++i) {
        if (i.value()) {
            // Space is available
            auto j = i + 1;
            if (j != d->m_memoryMap.end()) {
                if (i.key() + static_cast<qint64>(block.size()) > j.key())
                    // Not enough space to save here 
                    continue;
                if (i.key() + static_cast<qint64>(block.size()) < j.key()) {
                    // Item smaller than available space
                    d->m_memoryMap.insert(i.key() + static_cast<qint64>(block.size()), true);
                }
            }
            else {
                d->m_memoryMap.insert(i.key() + block.size(), true);
            }
            i.value() = false;
            d->m_device->seek(i.key());
            d->m_device->write(block);
            return i.key();
        }
    }
    Q_UNREACHABLE();
    return 0;
}



