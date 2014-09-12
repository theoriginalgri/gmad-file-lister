#ifndef GMADREADER_H
#define GMADREADER_H

#include <QList>
#include <QString>

class QIODevice;

struct GMadContentFileInfo {
    quint32 number;
    QString name;
    quint64 size;
    quint32 crc;
};

class GMadReader
{
public:
    GMadReader();
    ~GMadReader();

    bool parse(QIODevice *device);

    QString errorString() const;

    qint8 formatVersion() const;
    quint64 steamId() const;
    quint64 timestamp() const;

    quint32 addonVersion() const;

    QString name() const;
    QString description() const;
    QString author() const;

    QList<GMadContentFileInfo> files() const;

private:
    class Private;
    Private* d;
};

#endif // GMADREADER_H
