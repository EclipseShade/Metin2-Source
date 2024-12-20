#include "stdafx.h"

#include "ClientManager.h"

void CClientManager::UpdateHorseName(TPacketUpdateHorseName* data, CPeer* peer) {
    std::ostringstream queryHorseName;

    queryHorseName << "REPLACE INTO horse_name VALUES(" << data->dwPlayerID << ", '"
        << data->szHorseName << "')";

    std::auto_ptr<SQLMsg> pmsg_insert(CDBManager::instance().DirectQuery(queryHorseName.str().c_str()));

    ForwardPacket(HEADER_DG_UPDATE_HORSE_NAME, data, sizeof(TPacketUpdateHorseName), 0, peer);
}

void CClientManager::AckHorseName(DWORD dwPID, CPeer* peer) {
    std::ostringstream queryHorseName;
    queryHorseName << "SELECT name FROM horse_name WHERE id = " << dwPID;

    std::auto_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(queryHorseName.str().c_str()));

    TPacketUpdateHorseName packet;
    packet.dwPlayerID = dwPID;

    if (pmsg->Get()->uiNumRows == 0) {
        memset(packet.szHorseName, 0, sizeof(packet.szHorseName));
    } else {
        MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
        strlcpy(packet.szHorseName, row[0], sizeof(packet.szHorseName));
    }

    peer->EncodeHeader(HEADER_DG_ACK_HORSE_NAME, 0, sizeof(TPacketUpdateHorseName));
    peer->Encode(&packet, sizeof(TPacketUpdateHorseName));
}
