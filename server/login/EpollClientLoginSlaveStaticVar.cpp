#include "EpollClientLoginSlave.h"

#include <iostream>

using namespace CatchChallenger;

QList<unsigned int> EpollClientLoginSlave::maxAccountIdList;
QList<unsigned int> EpollClientLoginSlave::maxCharacterIdList;
QList<unsigned int> EpollClientLoginSlave::maxClanIdList;
bool EpollClientLoginSlave::maxAccountIdRequested=false;
bool EpollClientLoginSlave::maxCharacterIdRequested=false;
bool EpollClientLoginSlave::maxMonsterIdRequested=false;
char EpollClientLoginSlave::maxAccountIdRequest[]={0x11/*reply server to client*/,0x01/*query id*/,0x00/*the init reply query number*/};
char EpollClientLoginSlave::maxCharacterIdRequest[]={0x11/*reply server to client*/,0x02/*query id*/,0x00/*the init reply query number*/};
char EpollClientLoginSlave::maxMonsterIdRequest[]={0x11/*reply server to client*/,0x03/*query id*/,0x00/*the init reply query number*/};
char EpollClientLoginSlave::selectCharaterRequestOnMaster[]={0x02/*reply server to client*/,0x07/*query id*/,0x00/*the init reply query number*/};

unsigned char EpollClientLoginSlave::protocolReplyProtocolNotSupported[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01/*reply size*/,0x02/*return code*/};
unsigned char EpollClientLoginSlave::protocolReplyServerFull[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01/*reply size*/,0x03/*return code*/};
unsigned char EpollClientLoginSlave::protocolReplyCompressionNone[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01+TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT/*reply size*/,0x04/*return code*/};
unsigned char EpollClientLoginSlave::protocolReplyCompresssionZlib[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01+TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT/*reply size*/,0x05/*return code*/};
unsigned char EpollClientLoginSlave::protocolReplyCompressionXz[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01+TOKEN_SIZE_FOR_CLIENT_AUTH_AT_CONNECT/*reply size*/,0x06/*return code*/};

char EpollClientLoginSlave::characterSelectionIsWrongBufferCharacterNotFound[];
char EpollClientLoginSlave::characterSelectionIsWrongBufferCharacterAlreadyConnectedOnline[];
char EpollClientLoginSlave::characterSelectionIsWrongBufferServerInternalProblem[];
char EpollClientLoginSlave::characterSelectionIsWrongBufferServerNotFound[];
quint8 EpollClientLoginSlave::characterSelectionIsWrongBufferSize=0;
unsigned char EpollClientLoginSlave::loginIsWrongBufferReply[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01/*reply size*/,0x00/*temp return code*/};

unsigned char EpollClientLoginSlave::loginInProgressBuffer[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01/*reply size*/,0x06/*return code*/};
unsigned char EpollClientLoginSlave::addCharacterIsWrongBuffer[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01/*reply size*/,0x00/*temp return code*/,0x00,0x00,0x00,0x00};
unsigned char EpollClientLoginSlave::addCharacterReply[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x05/*reply size*/,0x00/*temp return code*/};
unsigned char EpollClientLoginSlave::removeCharacterReply[]={0xC1/*reply server to client*/,0x00/*the init reply query number*/,0x01/*reply size*/,0x00/*temp return code*/};
char EpollClientLoginSlave::baseDatapackSum[];
char EpollClientLoginSlave::loginGood[];
unsigned int EpollClientLoginSlave::loginGoodSize=0;

char EpollClientLoginSlave::serverServerList[];
unsigned int EpollClientLoginSlave::serverServerListSize=0;
char EpollClientLoginSlave::serverServerListComputedMessage[];
unsigned int EpollClientLoginSlave::serverServerListComputedMessageSize=0;
unsigned int EpollClientLoginSlave::serverServerListCurrentPlayerSize=0;
char EpollClientLoginSlave::serverLogicalGroupList[];
unsigned int EpollClientLoginSlave::serverLogicalGroupListSize=0;
char EpollClientLoginSlave::serverLogicalGroupAndServerList[];
unsigned int EpollClientLoginSlave::serverLogicalGroupAndServerListSize=0;
EpollClientLoginSlave::ProxyMode EpollClientLoginSlave::proxyMode=EpollClientLoginSlave::ProxyMode::Reconnect;

const unsigned char EpollClientLoginSlave::protocolHeaderToMatch[] = PROTOCOL_HEADER_LOGIN;

EpollPostgresql EpollClientLoginSlave::databaseBaseLogin;
EpollPostgresql EpollClientLoginSlave::databaseBaseCommon;

char EpollClientLoginSlave::replyToRegisterLoginServerCharactersGroup[1024];
unsigned int EpollClientLoginSlave::replyToRegisterLoginServerCharactersGroupSize=0;