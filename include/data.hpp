#ifndef DATA_HPP
#define DATA_HPP

#include <QByteArray>

class Data
{
protected:
    virtual QByteArray readFromSource(const QByteArray& theSource, const unsigned int theStartPos, const unsigned int theSize) const final;
    virtual bool writeSourceToFile(const QByteArray& theSource, const QString& thePath) const final;

public:
    virtual QByteArray rawData() const = 0;
    virtual bool writeToFile(const QString& thePath) const final;
    virtual unsigned int size() const = 0;

    virtual ~Data() {}
};

#endif // DATA_HPP
