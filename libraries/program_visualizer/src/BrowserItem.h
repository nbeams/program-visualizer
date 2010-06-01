#ifndef BROWSERITEM_H
#define BROWSERITEM_H

#include <QTreeWidgetItem>

class BrowserItem : public QTreeWidgetItem
{

public:
    BrowserItem();
    BrowserItem(QTreeWidget *parent = 0, int type = 0, QString nameText = 0);
    BrowserItem(BrowserItem *parent = 0, int type = 0, QString nameText = 0);

    QString nameText();

private:

    QString m_nameText;
    BrowserItem *m_parentItem;


};

#endif // BROWSERITEM_H
