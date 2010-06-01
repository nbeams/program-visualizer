/****************************************************************************
** Meta object code from reading C++ file 'VisualizerRect.h'
**
** Created: Wed Apr 14 22:50:18 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/VisualizerRect.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VisualizerRect.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VisualizerRect[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_VisualizerRect[] = {
    "VisualizerRect\0\0rectClicked()\0"
};

const QMetaObject VisualizerRect::staticMetaObject = {
    { &QGraphicsTextItem::staticMetaObject, qt_meta_stringdata_VisualizerRect,
      qt_meta_data_VisualizerRect, 0 }
};

const QMetaObject *VisualizerRect::metaObject() const
{
    return &staticMetaObject;
}

void *VisualizerRect::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VisualizerRect))
        return static_cast<void*>(const_cast< VisualizerRect*>(this));
    return QGraphicsTextItem::qt_metacast(_clname);
}

int VisualizerRect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsTextItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rectClicked(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void VisualizerRect::rectClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
