#include "BaseWindow.h"
#include "ui_BaseWindow.h"
#include "../../general/base/FacilityLib.h"
#include "../../general/base/CommonDatapack.h"
#include "NewGame.h"

using namespace CatchChallenger;

void BaseWindow::on_character_add_clicked()
{
    if((characterEntryListInWaiting.size()+characterEntryList.size())>=CommonSettings::commonSettings.max_character)
    {
        QMessageBox::warning(this,tr("Error"),tr("You have already the max characters count"));
        return;
    }
    if(newProfile!=NULL)
        delete newProfile;
    newProfile=new NewProfile(CatchChallenger::Api_client_real::client->get_datapack_base(),this);
    connect(newProfile,&NewProfile::finished,this,&BaseWindow::newProfileFinished);
    newProfile->show();
}

void BaseWindow::newProfileFinished()
{
    const QString &datapackPath=CatchChallenger::Api_client_real::client->get_datapack_base();
    if(CatchChallenger::CommonDatapack::commonDatapack.profileList.size()>1)
        if(!newProfile->ok())
        {
            if(characterEntryList.isEmpty() && CommonSettings::commonSettings.min_character>0)
                CatchChallenger::Api_client_real::client->tryDisconnect();
            return;
        }
    int profileIndex=0;
    if(CatchChallenger::CommonDatapack::commonDatapack.profileList.size()>1)
        profileIndex=newProfile->getProfileIndex();
    Profile profile=CatchChallenger::CommonDatapack::commonDatapack.profileList.at(profileIndex);
    newProfile->deleteLater();
    newProfile=NULL;
    NewGame nameGame(datapackPath+DATAPACK_BASE_PATH_SKIN,profile.forcedskin,this);
    if(!nameGame.haveSkin())
    {
        if(characterEntryList.isEmpty() && CommonSettings::commonSettings.min_character>0)
            CatchChallenger::Api_client_real::client->tryDisconnect();
        QMessageBox::critical(this,tr("Error"),QString("Sorry but no skin found into: %1").arg(QFileInfo(datapackPath+DATAPACK_BASE_PATH_SKIN).absoluteFilePath()));
        return;
    }
    nameGame.exec();
    if(!nameGame.haveTheInformation())
    {
        if(characterEntryList.isEmpty() && CommonSettings::commonSettings.min_character>0)
            CatchChallenger::Api_client_real::client->tryDisconnect();
        return;
    }
    CharacterEntry characterEntry;
    characterEntry.character_id=0;
    characterEntry.delete_time_left=0;
    characterEntry.last_connect=QDateTime::currentMSecsSinceEpoch()/1000;
    characterEntry.map=profile.map;
    characterEntry.played_time=0;
    characterEntry.pseudo=nameGame.pseudo();
    characterEntry.skin=nameGame.skin();
    CatchChallenger::Api_client_real::client->addCharacter(profileIndex,characterEntry.pseudo,characterEntry.skin);
    characterEntryListInWaiting << characterEntry;
    if((characterEntryListInWaiting.size()+characterEntryList.size())>=CommonSettings::commonSettings.max_character)
        ui->character_add->setEnabled(false);
    ui->stackedWidget->setCurrentWidget(ui->page_init);
    ui->label_connecting_status->setText(tr("Creating your new character"));
}

void BaseWindow::newCharacterId(const quint32 &characterId)
{
    CharacterEntry characterEntry=characterEntryListInWaiting.first();
    characterEntryListInWaiting.removeFirst();
    characterEntry.character_id=characterId;
    characterEntryList << characterEntry;
    updateCharacterList();
    ui->characterEntryList->item(ui->characterEntryList->count()-1)->setSelected(true);
    //if(characterEntryList.size()>=CommonSettings::commonSettings.min_character && characterEntryList.size()<=CommonSettings::commonSettings.max_character)
        on_character_select_clicked();
/*    else
        ui->stackedWidget->setCurrentWidget(ui->page_character);*/
}

void BaseWindow::updateCharacterList()
{
    ui->characterEntryList->clear();
    int index=0;
    while(index<characterEntryList.size())
    {
        const CharacterEntry &characterEntry=characterEntryList.at(index);
        QListWidgetItem * item=new QListWidgetItem();
        item->setData(99,characterEntry.character_id);
        QString text=characterEntry.pseudo+"\n"+QString("%1 played").arg(FacilityLib::timeToString(characterEntry.played_time));
        if(characterEntry.delete_time_left>0)
            text+="\n"+tr("%1 to be deleted").arg(FacilityLib::timeToString(characterEntry.played_time));
        item->setText(text);
        item->setIcon(QIcon(CatchChallenger::Api_client_real::client->get_datapack_base()+DATAPACK_BASE_PATH_SKIN+characterEntry.skin+"/front.png"));
        ui->characterEntryList->addItem(item);
        index++;
    }
}

void BaseWindow::on_character_back_clicked()
{
    CatchChallenger::Api_client_real::client->tryDisconnect();
}

void BaseWindow::on_character_select_clicked()
{
    QList<QListWidgetItem *> selectedItems=ui->characterEntryList->selectedItems();
    if(selectedItems.size()!=1)
        return;
    on_characterEntryList_itemDoubleClicked(selectedItems.first());
}

void BaseWindow::on_character_remove_clicked()
{

}

void BaseWindow::on_characterEntryList_itemDoubleClicked(QListWidgetItem *item)
{
    CatchChallenger::Api_client_real::client->selectCharacter(item->data(99).toUInt());
    ui->stackedWidget->setCurrentWidget(ui->page_init);
    ui->label_connecting_status->setText(tr("Selecting your character"));
}