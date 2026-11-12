/****************************************************************************
** Meta object code from reading C++ file 'ShortsController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/shorts/ShortsController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ShortsController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ShortsController_t {
    QByteArrayData data[17];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ShortsController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ShortsController_t qt_meta_stringdata_ShortsController = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ShortsController"
QT_MOC_LITERAL(1, 17, 14), // "currentChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 9), // "VideoInfo"
QT_MOC_LITERAL(4, 43, 1), // "v"
QT_MOC_LITERAL(5, 45, 5), // "index"
QT_MOC_LITERAL(6, 51, 11), // "listLoading"
QT_MOC_LITERAL(7, 63, 2), // "on"
QT_MOC_LITERAL(8, 66, 9), // "listError"
QT_MOC_LITERAL(9, 76, 7), // "message"
QT_MOC_LITERAL(10, 84, 10), // "onFinished"
QT_MOC_LITERAL(11, 95, 2), // "id"
QT_MOC_LITERAL(12, 98, 16), // "QList<VideoInfo>"
QT_MOC_LITERAL(13, 115, 5), // "items"
QT_MOC_LITERAL(14, 121, 5), // "token"
QT_MOC_LITERAL(15, 127, 8), // "onFailed"
QT_MOC_LITERAL(16, 136, 3) // "msg"

    },
    "ShortsController\0currentChanged\0\0"
    "VideoInfo\0v\0index\0listLoading\0on\0"
    "listError\0message\0onFinished\0id\0"
    "QList<VideoInfo>\0items\0token\0onFailed\0"
    "msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ShortsController[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       6,    1,   44,    2, 0x06 /* Public */,
       8,    1,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    3,   50,    2, 0x08 /* Private */,
      15,    2,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 12, QMetaType::QString,   11,   13,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,   16,

       0        // eod
};

void ShortsController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ShortsController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentChanged((*reinterpret_cast< const VideoInfo(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->listLoading((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->listError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QList<VideoInfo>(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 4: _t->onFailed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< VideoInfo >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<VideoInfo> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ShortsController::*)(const VideoInfo & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ShortsController::currentChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ShortsController::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ShortsController::listLoading)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ShortsController::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ShortsController::listError)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ShortsController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ShortsController.data,
    qt_meta_data_ShortsController,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ShortsController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShortsController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ShortsController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ShortsController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ShortsController::currentChanged(const VideoInfo & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ShortsController::listLoading(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ShortsController::listError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
