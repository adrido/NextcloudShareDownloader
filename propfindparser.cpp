#include "propfindparser.h"

#include <QUrl>
#include <QDebug>

PropFindParser::PropFindParser(const QByteArray &data)
{
    xml.addData(data);
    PropItem item;
    xml.error();
    while (!xml.atEnd()) {
          xml.readNext();
          //qDebug()<< xml.name();
          if(xml.name() == "href"){
              item.href = QUrl::fromPercentEncoding(xml.readElementText().toUtf8());
          }
          else if(xml.name()=="response" && xml.isEndElement()){
              content.append(item);
          }
    }
    if (xml.hasError()) {
          qDebug()<< xml.errorString();
    }
}
