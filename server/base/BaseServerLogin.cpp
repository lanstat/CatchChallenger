#include "BaseServerLogin.h"
#include "../../general/base/CommonDatapack.h"
#include "../../general/base/FacilityLibGeneral.h"
#include "../../general/base/GeneralVariable.h"
#include "../VariableServer.h"
#include "../VariableServer.h"

#include <QDebug>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <iostream>

using namespace CatchChallenger;

#ifdef Q_OS_LINUX
FILE * BaseServerLogin::fpRandomFile=NULL;
#endif
#ifndef CATCHCHALLENGER_CLASS_ONLYGAMESERVER
BaseServerLogin::TokenLink BaseServerLogin::tokenForAuth[];
quint32 BaseServerLogin::tokenForAuthSize=0;
#endif

BaseServerLogin::BaseServerLogin()
    #ifndef CATCHCHALLENGER_CLASS_LOGIN
    : databaseBaseLogin(NULL)
    #endif
{
}

BaseServerLogin::~BaseServerLogin()
{
}

void BaseServerLogin::preload_the_randomData()
{
    #ifndef CATCHCHALLENGER_CLASS_ONLYGAMESERVER
    BaseServerLogin::tokenForAuthSize=0;
    #endif
    #ifdef Q_OS_LINUX
    if(BaseServerLogin::fpRandomFile!=NULL)
        fclose(BaseServerLogin::fpRandomFile);
    BaseServerLogin::fpRandomFile = fopen("/dev/urandom","rb");
    if(fpRandomFile==NULL)
    {
        std::cerr << "Unable to open /dev/urandom to generate random token" << std::endl;
        /* allow poor quality number:
         * 1) more easy to run, allow start include if /dev/urandom can't be read
         * 2) it's for very small server (Lan) or internal communication */
        #if ! defined(CATCHCHALLENGER_CLIENT) && ! defined(CATCHCHALLENGER_SOLO)
        abort();
        #endif
    }
    #endif

    //to have previsible data
    /*if(GlobalServerData::serverSettings.benchmark)
    {
        srand(0);
        QDataStream randomDataStream(&GlobalServerData::serverPrivateVariables.randomData, QIODevice::WriteOnly);
        randomDataStream.setVersion(QDataStream::Qt_4_4);
        int index=0;
        while(index<CATCHCHALLENGER_SERVER_RANDOM_INTERNAL_SIZE)
        {
            randomDataStream << quint8(rand()%256);
            index++;
        }
    }
    else
    {*/
    //}
}

void BaseServerLogin::unload()
{
    unload_the_randomData();
}

void BaseServerLogin::unload_the_randomData()
{
    #ifdef Q_OS_LINUX
    if(BaseServerLogin::fpRandomFile!=NULL)
    {
        fclose(BaseServerLogin::fpRandomFile);
        BaseServerLogin::fpRandomFile=NULL;
    }
    #endif
    #ifndef CATCHCHALLENGER_CLASS_ONLYGAMESERVER
    BaseServerLogin::tokenForAuthSize=0;
    #endif
    //GlobalServerData::serverPrivateVariables.randomData.clear();
}