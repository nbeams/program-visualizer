/****************************************************************************
** Meta object code from reading C++ file 'UpdateDialog.h'
**
** Created: Thu Jul 8 08:34:32 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/UpdateDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UpdateDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UpdateDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      29,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UpdateDialog[] = {
    "UpdateDialog\0\0okWasPressed()\0"
    "cancelWasPressed()\0"
};

const QMetaObject UpdateDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_UpdateDialog,
      qt_meta_data_UpdateDialog, 0 }
};

const QMetaObject *UpdateDialog::metaObject() const
{
    return &staticMetaObject;
}

void *UpdateDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UpdateDialog))
        return static_cast<void*>(const_cast< UpdateDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int UpdateDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: okWasPressed(); break;
        case 1: cancelWasPressed(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
