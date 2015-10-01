#ifndef CheckedTempDir_h__
#define CheckedTempDir_h__

#include <QTemporaryDir>
#include <QDir>
#include <memory>
class CheckedTempDir
{
protected:
    std::unique_ptr<QTemporaryDir> m_baseDir;
public:
    CheckedTempDir(const CheckedTempDir&) = delete;
    CheckedTempDir& operator=(const CheckedTempDir&) = delete;
    CheckedTempDir()
        :m_baseDir(new QTemporaryDir())
    {}
    virtual ~CheckedTempDir(){}
    CheckedTempDir(const QString & templatePath){
        const QDir tmpDir(templatePath);
        if (!tmpDir.exists())
            tmpDir.mkpath(templatePath);
        m_baseDir.reset(new QTemporaryDir(templatePath));
    }
    bool autoRemove() const { return m_baseDir->autoRemove(); }
    bool isValid() const { return m_baseDir->isValid(); }
    QString path() const { return m_baseDir->path(); }
    bool remove() { return m_baseDir->remove(); }
    void setAutoRemove(bool b) { m_baseDir->setAutoRemove(b); }
};
#endif // CheckedTempDir_h__