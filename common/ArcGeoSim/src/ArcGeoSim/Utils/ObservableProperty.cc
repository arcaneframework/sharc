// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "Utils.h"
#include "ObservableProperty.h"

#ifdef USE_BOOST_SIGNAL
/*--------------------------------------------------------*/

const ObservableProperty::NameType& ObservableProperty::getName() {
  return m_name;
}

/*--------------------------------------------------------*/

ObservableProperty::ConnectionType ObservableProperty::connectObserver(ObserverType obs) {
  return m_signal.connect(obs);
}

/*--------------------------------------------------------*/

void ObservableProperty::removeConnection(ConnectionType& obs) {
  obs.disconnect();
}

/*--------------------------------------------------------*/

void ObservableProperty::blockConnection(ConnectionType& obs) {
  obs.block();
}


/*--------------------------------------------------------*/

void ObservableProperty::unblockConnection(ConnectionType& obs) {
  obs.unblock();
}
#endif
/*--------------------------------------------------------*/

void ObservableProperty::notifyChange() {
#ifdef USE_BOOST_SIGNAL
  m_signal();
#endif
}
