﻿/*
    This file is part of JQNetwork
    
    Library introduce: https://github.com/188080501/JQNetwork

    Copyright: Jason

    Contact email: Jason@JasonServer.com
    
    GitHub: https://github.com/188080501/
*/

#ifndef JQNETWORK_INCLUDE_JQNETWORK_LAN_H
#define JQNETWORK_INCLUDE_JQNETWORK_LAN_H

// JQNetwork lib import
#include <JQNetworkFoundation>

struct JQNetworkLanSettings
{
    QString dutyMark;

    QHostAddress multicastGroupAddress;
    quint16 multicastGroupAddressBindPort = 0;
    quint16 broadcastAddressBindPort = 0;

    int checkLoopInterval = 15 * 1000;
    int lanNodeTimeoutInterval = 45 * 1000;

    std::function< void( const JQNetworkLanNode & ) > lanNodeOnlineCallback;
    std::function< void( const JQNetworkLanNode & ) > lanNodeActiveCallback;
    std::function< void( const JQNetworkLanNode & ) > lanNodeOfflineCallback;
    std::function< void() > lanNodeListChangedCallback;

    int globalProcessorThreadCount = 1;
};

struct JQNetworkLanNode
{
    QString nodeMarkSummary;
    qint64 lastActiveTime = 0;
    QList< QHostAddress > ipList;
    QVariant appendData;
    QHostAddress matchAddress;
    bool isSelf = false;
};

struct JQNetworkLanAddressEntries
{
    QHostAddress ip;
    QHostAddress netmask;
    QHostAddress ipSegment;
    bool isVmAddress;
};

class JQNetworkLan: public QObject
{
    Q_OBJECT

public:
    JQNetworkLan(const JQNetworkLanSettingsSharedPointer &lanSettings);

    ~JQNetworkLan();

    JQNetworkLan(const JQNetworkLan &) = delete;

    JQNetworkLan &operator =(const JQNetworkLan &) = delete;

    static JQNetworkLanSharedPointer createLan(
            const QHostAddress &multicastGroupAddress,
            const quint16 &multicastGroupAddressBindPort,
            const quint16 &broadcastAddressBindPort,
            const QString &dutyMark = ""
        );

    static QList< JQNetworkLanAddressEntries > getLanAddressEntries();

    inline JQNetworkLanSettingsSharedPointer lanSettings();

    inline QString nodeMarkSummary() const;

    bool begin();

    QHostAddress matchLanAddressEntries(const QList< QHostAddress > &ipList);

    QList< JQNetworkLanNode > availableLanNodes();

private:
    void refreshLanAddressEntries();

    bool refreshUdp();

    void checkLoop();

    QByteArray makeData(const bool &requestOffline);

    void sendOnline();

    void sendOffline();

    void onUdpSocketReadyRead(QUdpSocket *udpSocket);

    inline void onLanNodeStateOnline(const JQNetworkLanNode &lanNode);

    inline void onLanNodeStateActive(const JQNetworkLanNode &lanNode);

    inline void onLanNodeStateOffline(const JQNetworkLanNode &lanNode);

    inline void onLanNodeListChanged();

private:
    // Thread pool
    static QWeakPointer< JQNetworkThreadPool > globalProcessorThreadPool_;
    QSharedPointer< JQNetworkThreadPool > processorThreadPool_;

    // Settings
    JQNetworkLanSettingsSharedPointer lanSettings_;

    // Socket
    QSharedPointer< QUdpSocket > udpSocketForMulticastGroupAddress_;
    QSharedPointer< QUdpSocket > udpSocketForBroadcastAddress_;

    // Data
    QList< JQNetworkLanAddressEntries > lanAddressEntries_;
    QMap< QString, JQNetworkLanNode > lanNodes_;

    // Other
    QString nodeMarkSummary_;
    QMutex mutex_;
    QSharedPointer< QTimer > timerForCheckLoop_;
    int checkLoopCounting_ = -1;
};

#include "jqnetwork_lan.inc"

#endif//JQNETWORK_INCLUDE_JQNETWORK_LAN_H
