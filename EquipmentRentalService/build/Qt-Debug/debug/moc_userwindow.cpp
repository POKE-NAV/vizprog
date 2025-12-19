/****************************************************************************
** Meta object code from reading C++ file 'userwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../userwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'userwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10UserWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto UserWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10UserWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "UserWindow",
        "onEquipmentSearchTextChanged",
        "",
        "text",
        "onEquipmentStatusFilterChanged",
        "index",
        "onEquipmentTypeFilterChanged",
        "onAddNewEquipmentClicked",
        "onEditEquipmentClicked",
        "equipmentId",
        "onDeleteEquipmentClicked",
        "onRentEquipmentClicked",
        "status",
        "onReserveEquipmentClicked",
        "loadProcessingRequests",
        "onRentalsSearchTextChanged",
        "onRentalsStatusFilterChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onEquipmentSearchTextChanged'
        QtMocHelpers::SlotData<void(const QString &)>(1, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'onEquipmentStatusFilterChanged'
        QtMocHelpers::SlotData<void(int)>(4, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Slot 'onEquipmentTypeFilterChanged'
        QtMocHelpers::SlotData<void(int)>(6, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Slot 'onAddNewEquipmentClicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessProtected, QMetaType::Void),
        // Slot 'onEditEquipmentClicked'
        QtMocHelpers::SlotData<void(int)>(8, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Slot 'onDeleteEquipmentClicked'
        QtMocHelpers::SlotData<void(int)>(10, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Slot 'onRentEquipmentClicked'
        QtMocHelpers::SlotData<void(int, const QString &)>(11, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 9 }, { QMetaType::QString, 12 },
        }}),
        // Slot 'onReserveEquipmentClicked'
        QtMocHelpers::SlotData<void(int, const QString &)>(13, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 9 }, { QMetaType::QString, 12 },
        }}),
        // Slot 'loadProcessingRequests'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessProtected, QMetaType::Void),
        // Slot 'onRentalsSearchTextChanged'
        QtMocHelpers::SlotData<void(const QString &)>(15, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'onRentalsStatusFilterChanged'
        QtMocHelpers::SlotData<void(int)>(16, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<UserWindow, qt_meta_tag_ZN10UserWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject UserWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<MainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10UserWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10UserWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10UserWindowE_t>.metaTypes,
    nullptr
} };

void UserWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UserWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onEquipmentSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onEquipmentStatusFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onEquipmentTypeFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onAddNewEquipmentClicked(); break;
        case 4: _t->onEditEquipmentClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onDeleteEquipmentClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onRentEquipmentClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->onReserveEquipmentClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->loadProcessingRequests(); break;
        case 9: _t->onRentalsSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->onRentalsStatusFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *UserWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UserWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10UserWindowE_t>.strings))
        return static_cast<void*>(this);
    return MainWindow::qt_metacast(_clname);
}

int UserWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
