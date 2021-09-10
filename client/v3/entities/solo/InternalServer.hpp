// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_ENTITIES_SOLO_INTERNALSERVER_HPP_
#define CLIENT_V3_ENTITIES_SOLO_INTERNALSERVER_HPP_
#include <QString>
#include <QTimer>

#include "../../../general/base/Map_loader.hpp"
#include "../../../general/base/ProtocolParsing.hpp"
#include "../../../server/base/Client.hpp"
#include "../../../server/base/MapServer.hpp"
#include "../../../server/base/QtServer.hpp"
#include "../../../server/base/ServerStructures.hpp"
#include "QFakeServer.hpp"
#include "QFakeSocket.hpp"

namespace CatchChallenger {
class InternalServer : public QtServer {
  Q_OBJECT
 public:
  /// \param settings ref is destroyed after this call
  explicit InternalServer();
  virtual ~InternalServer();
  virtual void run();
 private slots:
  // starting function
  void start_internal_server();
  void timerGiftSlot();

 protected:
  QString sqlitePath();
 protected slots:
  // remove all finished client
  virtual void removeOneClient();
  virtual void serverIsReady();

 private:
  QTimer timerGift;
};
}  // namespace CatchChallenger

#endif // CLIENT_V3_ENTITIES_SOLO_INTERNALSERVER_HPP_
