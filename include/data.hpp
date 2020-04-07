#ifndef DATA_HPP
#define DATA_HPP

#include <QByteArray>
#include <QDir>

class Data
{
public:
    Data() { saveDir.setPath(savePath); }

    virtual QByteArray rawData() const = 0;
    virtual bool writeToFile(const QString& thePath) const final;
    virtual unsigned int size() const = 0;
    virtual bool save() const final;

    virtual ~Data() {}

protected:
    virtual QByteArray readFromSource(const QByteArray& theSource, const unsigned int theStartPos, const unsigned int theSize) const final;
    virtual bool writeToSource(const QByteArray& theSource, QByteArray& theDestination, const int theStartPos) final;
    virtual bool writeSourceToFile(const QByteArray& theSource, const QString& thePath) const final;
    virtual bool saveSource(const QByteArray& theSource) const final;

private:
    static const QString savePath;

    QDir saveDir;
};

#endif // DATA_HPP
