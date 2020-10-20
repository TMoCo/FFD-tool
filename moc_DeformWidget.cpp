/****************************************************************************
** Meta object code from reading C++ file 'DeformWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "DeformWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DeformWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DeformWidget_t {
    QByteArrayData data[9];
    char stringdata0[93];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DeformWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DeformWidget_t qt_meta_stringdata_DeformWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "DeformWidget"
QT_MOC_LITERAL(1, 13, 8), // "loadMesh"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 8), // "fileName"
QT_MOC_LITERAL(4, 32, 9), // "buildGrid"
QT_MOC_LITERAL(5, 42, 14), // "changeGridSize"
QT_MOC_LITERAL(6, 57, 5), // "value"
QT_MOC_LITERAL(7, 63, 14), // "changeGridType"
QT_MOC_LITERAL(8, 78, 14) // "setAttenuation"

    },
    "DeformWidget\0loadMesh\0\0fileName\0"
    "buildGrid\0changeGridSize\0value\0"
    "changeGridType\0setAttenuation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DeformWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       4,    0,   42,    2, 0x0a /* Public */,
       5,    1,   43,    2, 0x0a /* Public */,
       7,    1,   46,    2, 0x0a /* Public */,
       8,    1,   49,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,

       0        // eod
};

void DeformWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DeformWidget *_t = static_cast<DeformWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadMesh((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->buildGrid(); break;
        case 2: _t->changeGridSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->changeGridType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setAttenuation((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject DeformWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_DeformWidget.data,
      qt_meta_data_DeformWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DeformWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeformWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DeformWidget.stringdata0))
        return static_cast<void*>(this);
    return QGLWidget::qt_metacast(_clname);
}

int DeformWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
