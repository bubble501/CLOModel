#include "MemoryMappedDevice.h"
#include <QIODevice>
MemoryMappedDevice::MemoryMappedDevice(QIODevice* source)
    :m_device(source)
{
    m_memoryMap.insert(0, true);
    if (m_device) {
        if (m_device->isSequential()) {
            Q_ASSERT_X(false, "MemoryMappedDevice()", "Device must be seekable");
            m_device = nullptr;
        }
        if (!m_device->isOpen()) {
            m_device->open(QIODevice::ReadWrite);
        }
    }
}

MemoryMappedDevice::~MemoryMappedDevice()
{
    if (m_device->isOpen()) {
        m_device->close();
    }
}

bool MemoryMappedDevice::removeValue(qint32 key)
{
    auto itemIter = m_itemsMap.find(key);
    if (itemIter == m_itemsMap.end())
        return false;
    auto fileIter = m_memoryMap.find(itemIter.value());
    Q_ASSERT(fileIter != m_memoryMap.end());
    if (fileIter.value())
        return false;
    m_itemsMap.erase(itemIter);
    if (fileIter != m_memoryMap.begin()){
        if ((fileIter - 1).value()) {
            m_memoryMap.erase(fileIter);
            return true;
        }
    }
    if ((fileIter + 1) != m_memoryMap.end()){
        if ((fileIter + 1).value()) 
            m_memoryMap.erase(fileIter+1);
    }
    fileIter.value() = true;
    return true;
}

QList<qint32> MemoryMappedDevice::itemKeys() const
{
    return m_itemsMap.keys();
}

bool MemoryMappedDevice::writeBlock(qint32 key, const QByteArray& block)
{
    if (!m_device)
        return false;
    if(!m_device->isOpen()){
        if (!m_device->open(QIODevice::ReadWrite))
            return false;
    }
    if (!m_device->isWritable())
        return false;
    m_device->setTextModeEnabled(false);
    auto itemIter = m_itemsMap.find(key);
    if (itemIter != m_itemsMap.end()) {
        removeValue(key);
    }
    m_itemsMap.insert(key, writeInMap(block));
    return true;
}

QByteArray MemoryMappedDevice::readBlock(qint32 key) const
{
    if (!m_device)
        return QByteArray();
    if (!m_device->isOpen()) {
        if (!m_device->open(QIODevice::ReadWrite))
            return QByteArray();
    }
    if (!m_device->isReadable())
        return QByteArray();
    m_device->setTextModeEnabled(false);
    auto itemIter = m_itemsMap.constFind(key);
    if (itemIter == m_itemsMap.constEnd())
        return QByteArray();
    auto fileIter = m_memoryMap.constFind(itemIter.value());
    Q_ASSERT(fileIter != m_memoryMap.constEnd());
    if(fileIter.value())
        return QByteArray();
    auto nextIter = fileIter + 1;
    m_device->seek(fileIter.key());
    if (nextIter == m_memoryMap.constEnd())
        return m_device->readAll();
    return m_device->read(nextIter.key() - fileIter.key());
}

qint64 MemoryMappedDevice::writeInMap(const QByteArray& block)
{
    for (auto i = m_memoryMap.begin(); i != m_memoryMap.end(); ++i) {
        if (i.value()) {
            // Space is available
            auto j = i + 1;
            if (j != m_memoryMap.end()) {
                if (i.key() + static_cast<qint64>(block.size()) > j.key())
                    // Not enough space to save here 
                    continue;
                if (i.key() + static_cast<qint64>(block.size()) < j.key()) {
                    // Item smaller than available space
                    m_memoryMap.insert(i.key() + static_cast<qint64>(block.size()), true);
                }
            }
            else {
                m_memoryMap.insert(i.key() + block.size(), true);
            }
            i.value() = false;
            m_device->seek(i.key());
            m_device->write(block);
            return i.key();
        }
    }
}



