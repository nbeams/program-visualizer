#include "BrowserItem.h"

BrowserItem::BrowserItem()
{
}

BrowserItem::BrowserItem(QTreeWidget *parent, int type, QString nameText) : QTreeWidgetItem(parent, type)
{
    m_nameText = nameText;
    setText(0, m_nameText);
}

BrowserItem::BrowserItem(BrowserItem *parent, int type, QString nameText) : QTreeWidgetItem(parent, type)
{
    m_nameText = nameText;
    setText(0, m_nameText);
    m_parentItem = parent;
}


QString BrowserItem::nameText()
{
    return m_nameText;
}

