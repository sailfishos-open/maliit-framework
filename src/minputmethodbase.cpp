/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "minputmethodbase.h"
#include "minputcontextconnection.h"

#include <QGraphicsView>
#include <QDebug>
#include <QKeyEvent>
#include <QDBusInterface>

namespace {
    const char * const DBusIndicatorServiceName = "com.meego.core.MInputMethodStatusIndicator";
    const char * const DBusIndicatorPath = "/inputmethodstatusindicator";
    const char * const DBusIndicatorInterface = "com.meego.core.MInputMethodStatusIndicator";
};

class MInputMethodBasePrivate
{
public:
    MInputMethodBasePrivate(MInputContextConnection *icConnection);
    ~MInputMethodBasePrivate();

    void connectToIndicatorDBus();
    //! map the InputModeIndicator to icon id
    QString indicatorIconID(MInputMethodBase::InputModeIndicator mode);

    MInputContextConnection *icConnection;
    QDBusInterface *indicatorIface; // indicator server interface
    QMap<MInputMethodBase::InputModeIndicator, QString> indicatorMap;
};



MInputMethodBasePrivate::MInputMethodBasePrivate(MInputContextConnection *icConnection)
    : icConnection(icConnection),
      indicatorIface(0)
{
    connectToIndicatorDBus();
    indicatorMap.insert(MInputMethodBase::NoIndicator, "");
    indicatorMap.insert(MInputMethodBase::LatinLower, "icon-s-status-latin-lowercase");
    indicatorMap.insert(MInputMethodBase::LatinUpper, "icon-s-status-latin-uppercase");
    indicatorMap.insert(MInputMethodBase::LatinLocked, "icon-s-status-latin-caps");
    indicatorMap.insert(MInputMethodBase::CyrillicLower, "icon-s-status-cyrillic-lowercase");
    indicatorMap.insert(MInputMethodBase::CyrillicUpper, "icon-s-status-cyrillic-uppercase");
    indicatorMap.insert(MInputMethodBase::CyrillicLocked, "icon-s-status-cyrillic-caps");
    indicatorMap.insert(MInputMethodBase::Arabic, "icon-s-status-arabic");
    indicatorMap.insert(MInputMethodBase::Pinyin, "icon-s-status-pinyin");
    indicatorMap.insert(MInputMethodBase::Zhuyin, "icon-s-status-zhuyin");
    indicatorMap.insert(MInputMethodBase::Cangjie, "icon-s-status-cangjie");
    indicatorMap.insert(MInputMethodBase::NumAndSymLatched, "icon-s-status-number");
    indicatorMap.insert(MInputMethodBase::NumAndSymLocked, "icon-s-status-number-locked");
    indicatorMap.insert(MInputMethodBase::DeadKeyAcute, "icon-s-status-acute");
    indicatorMap.insert(MInputMethodBase::DeadKeyCaron, "icon-s-status-caron");
    indicatorMap.insert(MInputMethodBase::DeadKeyCircumflex, "icon-s-status-circumflex");
    indicatorMap.insert(MInputMethodBase::DeadKeyDiaeresis, "icon-s-status-diaeresis");
    indicatorMap.insert(MInputMethodBase::DeadKeyGrave, "icon-s-status-grave");
    indicatorMap.insert(MInputMethodBase::DeadKeyTilde, "icon-s-status-tilde");
}


MInputMethodBasePrivate::~MInputMethodBasePrivate()
{
    delete indicatorIface;
}

void MInputMethodBasePrivate::connectToIndicatorDBus()
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to the DBus session bus";
        return;
    }
    indicatorIface = new QDBusInterface(DBusIndicatorServiceName, DBusIndicatorPath,
                                        DBusIndicatorInterface, connection);

    if (!indicatorIface->isValid()) {
        qWarning() << "MInputContextDBusConnection was unable to connect to indicator server: "
                   << connection.lastError().message();
        delete indicatorIface;
        indicatorIface = 0;
    }
}

QString MInputMethodBasePrivate::indicatorIconID(MInputMethodBase::InputModeIndicator mode)
{
    return indicatorMap.value(mode);
}

///////////////

MInputMethodBase::MInputMethodBase(MInputContextConnection *icConnection, QObject *parent)
    : QObject(parent),
      d(new MInputMethodBasePrivate(icConnection))
{
    // nothing
}


MInputMethodBase::~MInputMethodBase()
{
    delete d;
}


MInputContextConnection *
MInputMethodBase::inputContextConnection() const
{
    return d->icConnection;
}

void MInputMethodBase::show()
{
    // empty default implementation
}

void MInputMethodBase::hide()
{
    // empty default implementation
}

void MInputMethodBase::setPreedit(const QString &)
{
    // empty default implementation
}

void MInputMethodBase::update()
{
    // empty default implementation
}

void MInputMethodBase::reset()
{
    // empty default implementation
}

void MInputMethodBase::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    // empty default implementation
    Q_UNUSED(pos);
    Q_UNUSED(preeditRect);
}

void MInputMethodBase::focusChanged(bool /* focusIn */)
{
    // empty default implementation
}

void MInputMethodBase::visualizationPriorityChanged(bool priority)
{
    // empty default implementation
    Q_UNUSED(priority);
}

void MInputMethodBase::appOrientationChanged(int angle)
{
    // empty default implementation
    Q_UNUSED(angle);
}

void MInputMethodBase::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    // empty default implementation
    Q_UNUSED(copyAvailable);
    Q_UNUSED(pasteAvailable);
}

void MInputMethodBase::registerToolbar(qlonglong id, const QString &fileName)
{
    // empty default implementation
    Q_UNUSED(id);
    Q_UNUSED(fileName);
}

void MInputMethodBase::unregisterToolbar(qlonglong id)
{
    // empty default implementation
    Q_UNUSED(id);
}

void MInputMethodBase::setToolbar(qlonglong id)
{
    // empty default implementation
    Q_UNUSED(id);
}

void MInputMethodBase::setToolbarItemAttribute(qlonglong id, const QString &item,
                                               const QString &attribute, const QVariant &value)
{
    // empty default implementation
    Q_UNUSED(id);
    Q_UNUSED(item);
    Q_UNUSED(attribute);
    Q_UNUSED(value);
}

void MInputMethodBase::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                       Qt::KeyboardModifiers modifiers,
                                       const QString &text, bool autoRepeat, int count,
                                       quint32 /* nativeScanCode */, quint32 /* nativeModifiers */)
{
    // default implementation, just sendKeyEvent back
    inputContextConnection()->sendKeyEvent(QKeyEvent(keyType, keyCode, modifiers, text, autoRepeat,
                                                     count));
}

void MInputMethodBase::setState(const QSet<MIMHandlerState> &state)
{
    // empty default implementation
    Q_UNUSED(state);
}

void MInputMethodBase::clientChanged()
{
    // empty default implementation
}

void MInputMethodBase::switchContext(M::InputMethodSwitchDirection direction, bool enableAnimation)
{
    // empty default implementation
    Q_UNUSED(direction);
    Q_UNUSED(enableAnimation);
}

void MInputMethodBase::sendInputModeIndicator(MInputMethodBase::InputModeIndicator mode)
{
    if (d->indicatorIface) {
        d->indicatorIface->call(QDBus::NoBlock, "setIconID", d->indicatorIconID(mode));
    }
}
