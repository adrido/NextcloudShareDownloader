#ifndef PROPFINDPARSER_H
#define PROPFINDPARSER_H

#include <QString>
#include <QXmlStreamReader>

struct PropItem
{
    PropItem() {}
    QString href;
    bool isDir(){return href.back() == '/';}
    const QString getName() {return href.section('/',-1,-1,QString::SectionSkipEmpty|QString::SectionIncludeTrailingSep);}
};

class PropFindParser
{
public:
    PropFindParser(const QByteArray &data);
    QList<PropItem> getList(){return content;}
private:
    QXmlStreamReader xml;
    QList<PropItem> content;
};

#endif // PROPFINDPARSER_H
