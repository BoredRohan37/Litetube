/****************************************************************************
** Meta object code from reading C++ file 'PlaybackSession.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/playback/PlaybackSession.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlaybackSession.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlaybackSession_t {
    QByteArrayData data[28];
    char stringdata0[241];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlaybackSession_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlaybackSession_t qt_meta_stringdata_PlaybackSession = {
    {
QT_MOC_LITERAL(0, 0, 15), // "PlaybackSession"
QT_MOC_LITERAL(1, 16, 7), // "loading"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 9), // "VideoInfo"
QT_MOC_LITERAL(4, 35, 1), // "v"
QT_MOC_LITERAL(5, 37, 12), // "resolvedMeta"
QT_MOC_LITERAL(6, 50, 14), // "ResolvedStream"
QT_MOC_LITERAL(7, 65, 1), // "s"
QT_MOC_LITERAL(8, 67, 7), // "started"
QT_MOC_LITERAL(9, 75, 6), // "failed"
QT_MOC_LITERAL(10, 82, 7), // "message"
QT_MOC_LITERAL(11, 90, 5), // "ended"
QT_MOC_LITERAL(12, 96, 8), // "position"
QT_MOC_LITERAL(13, 105, 3), // "pos"
QT_MOC_LITERAL(14, 109, 3), // "dur"
QT_MOC_LITERAL(15, 113, 9), // "buffering"
QT_MOC_LITERAL(16, 123, 2), // "on"
QT_MOC_LITERAL(17, 126, 13), // "tracksChanged"
QT_MOC_LITERAL(18, 140, 6), // "tracks"
QT_MOC_LITERAL(19, 147, 16), // "qualitiesChanged"
QT_MOC_LITERAL(20, 164, 11), // "inputAction"
QT_MOC_LITERAL(21, 176, 6), // "action"
QT_MOC_LITERAL(22, 183, 10), // "onResolved"
QT_MOC_LITERAL(23, 194, 2), // "id"
QT_MOC_LITERAL(24, 197, 15), // "onResolveFailed"
QT_MOC_LITERAL(25, 213, 3), // "msg"
QT_MOC_LITERAL(26, 217, 10), // "onMpvError"
QT_MOC_LITERAL(27, 228, 12) // "onFileLoaded"

    },
    "PlaybackSession\0loading\0\0VideoInfo\0v\0"
    "resolvedMeta\0ResolvedStream\0s\0started\0"
    "failed\0message\0ended\0position\0pos\0dur\0"
    "buffering\0on\0tracksChanged\0tracks\0"
    "qualitiesChanged\0inputAction\0action\0"
    "onResolved\0id\0onResolveFailed\0msg\0"
    "onMpvError\0onFileLoaded"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlaybackSession[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       5,    1,   87,    2, 0x06 /* Public */,
       8,    0,   90,    2, 0x06 /* Public */,
       9,    1,   91,    2, 0x06 /* Public */,
      11,    0,   94,    2, 0x06 /* Public */,
      12,    2,   95,    2, 0x06 /* Public */,
      15,    1,  100,    2, 0x06 /* Public */,
      17,    1,  103,    2, 0x06 /* Public */,
      19,    0,  106,    2, 0x06 /* Public */,
      20,    1,  107,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      22,    2,  110,    2, 0x08 /* Private */,
      24,    2,  115,    2, 0x08 /* Private */,
      26,    1,  120,    2, 0x08 /* Private */,
      27,    0,  123,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   13,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::QVariantList,   18,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   21,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 6,   23,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   23,   25,
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void,

       0        // eod
};

void PlaybackSession::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlaybackSession *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loading((*reinterpret_cast< const VideoInfo(*)>(_a[1]))); break;
        case 1: _t->resolvedMeta((*reinterpret_cast< const ResolvedStream(*)>(_a[1]))); break;
        case 2: _t->started(); break;
        case 3: _t->failed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->ended(); break;
        case 5: _t->position((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 6: _t->buffering((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->tracksChanged((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 8: _t->qualitiesChanged(); break;
        case 9: _t->inputAction((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->onResolved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const ResolvedStream(*)>(_a[2]))); break;
        case 11: _t->onResolveFailed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 12: _t->onMpvError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->onFileLoaded(); break;
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
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PlaybackSession::*)(const VideoInfo & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::loading)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)(const ResolvedStream & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::resolvedMeta)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::started)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::failed)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::ended)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)(double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::position)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::buffering)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)(const QVariantList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::tracksChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::qualitiesChanged)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (PlaybackSession::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackSession::inputAction)) {
                *result = 9;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PlaybackSession::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PlaybackSession.data,
    qt_meta_data_PlaybackSession,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PlaybackSession::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlaybackSession::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlaybackSession.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PlaybackSession::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void PlaybackSession::loading(const VideoInfo & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PlaybackSession::resolvedMeta(const ResolvedStream & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PlaybackSession::started()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void PlaybackSession::failed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PlaybackSession::ended()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void PlaybackSession::position(double _t1, double _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void PlaybackSession::buffering(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void PlaybackSession::tracksChanged(const QVariantList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void PlaybackSession::qualitiesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void PlaybackSession::inputAction(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
