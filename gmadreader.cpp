#include "gmadreader.h"

#include <QIODevice>

struct gmad_header {
    qint32 magic;
    char formatVersion;
    quint64 steamid;
    quint64 timestamp;
};

class GMadReader::Private
{
public:
    // Properties
    QString errorString;

    gmad_header header;

    QString name;
    QString description;
    QString author;

    quint32 addonVersion;

    QList<GMadContentFileInfo> files;

    // Functions
    QString readString(QIODevice *device);
};

QString GMadReader::Private::readString(QIODevice *device)
{
    QByteArray output;

    char data;

    while (true) {
        device->read(&data, sizeof(data));

        if (data == 0)
            break;
        else
            output.append(data);
    }

    return output;
}

GMadReader::GMadReader()
: d(new Private)
{
}

GMadReader::~GMadReader()
{
    delete d;
}

bool GMadReader::parse(QIODevice *device)
{
    d->files.clear();

    // Read the header
    device->read((char*)&d->header, sizeof(d->header));

    if (d->header.magic != 0x44414d47) {
        d->errorString = QObject::tr("Wrong magic number. Not a garrys mod addon");
        return false;
    }

    // Read additional information
    d->name = d->readString(device);
    d->description = d->readString(device);
    d->author = d->readString(device);

    device->read((char*)&d->addonVersion, sizeof(d->addonVersion));

    while (true) {
        GMadContentFileInfo fileInfo;
        device->read((char*)&fileInfo.number, sizeof(fileInfo.number));

        if (fileInfo.number == 0)
            break;

        fileInfo.name = d->readString(device);
        device->read((char*)&fileInfo.size, sizeof(fileInfo.size));
        device->read((char*)&fileInfo.crc, sizeof(fileInfo.crc));

        d->files.append(fileInfo);
    }

    return true;
}

QString GMadReader::errorString() const
{
    return d->errorString;
}

qint8 GMadReader::formatVersion() const
{
    return d->header.formatVersion;
}

quint64 GMadReader::steamId() const
{
    return d->header.steamid;
}

quint64 GMadReader::timestamp() const
{
    return d->header.timestamp;
}

QString GMadReader::name() const
{
    return d->name;
}

QString GMadReader::description() const
{
    return d->description;
}

QString GMadReader::author() const
{
    return d->author;
}

QList<GMadContentFileInfo> GMadReader::files() const
{
    return d->files;
}
