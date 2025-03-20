// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include <arcane/CaseOptionBase.h>
#include <arcane/XmlNode.h>
#include <arcane/XmlNodeList.h>
#include <arcane/utils/ITraceMng.h>

namespace Arcane {
  extern bool
  _caseOptionConvert(const Arcane::CaseOptionBase& co,
                     const String& value,
                     XmlNode& obj)
  {
    XmlNode root = co.rootElement();
    XmlNodeList children = root.children(co.name());
    if (children.size() > 1)
      co.traceMng()->fatal() << "XmlNode case-option only implemented for single occurency. Check option " << co.name() << " from " << root.xpathFullName() << " " << children.size();
    if (children.empty())
        obj = XmlNode(); // null node
    else
      obj = children[0];
    return false;
  }
}






