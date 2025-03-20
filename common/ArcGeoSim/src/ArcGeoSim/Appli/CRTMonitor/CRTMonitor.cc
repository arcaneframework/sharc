// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

#include <arcane/utils/String.h>
#include <arcane/utils/PlatformUtils.h>

#ifdef WIN32
#include <crtdbg.h>
#endif /* WIN32 */

/*---------------------------------------------------------------------------*/

class CRTMonitor
{
public:
  CRTMonitor();
public:
  static void initialize();
private:
  static CRTMonitor m_instance;
};

/*---------------------------------------------------------------------------*/

CRTMonitor CRTMonitor::m_instance;

/*---------------------------------------------------------------------------*/

CRTMonitor::
CRTMonitor()
{
  CRTMonitor::initialize();
}

/*---------------------------------------------------------------------------*/

void
CRTMonitor::
initialize()
{
 if (Arcane::platform::getEnvironmentVariable("ARCGEOSIM_ENABLE_WIN32CRT").null())
    {
 #ifdef WIN32
      // Disable CRT windows popup except if enabled explicitly by user

      // See http://msdn.microsoft.com/en-us/library/1y71x448.aspx
      //     http://msdn.microsoft.com/en-us/library/0yysf5e6.aspx
      /*
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ERROR , _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_WARN  , _CRTDBG_MODE_DEBUG);
      */
      _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
      _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR );
      _CrtSetReportMode(_CRT_ERROR , _CRTDBG_MODE_FILE);
      _CrtSetReportFile(_CRT_ERROR , _CRTDBG_FILE_STDERR );
      _CrtSetReportMode(_CRT_WARN  , _CRTDBG_MODE_FILE);
      _CrtSetReportFile(_CRT_WARN  , _CRTDBG_FILE_STDERR );
#else  /* WIN32 */
  // nothing todo : no enabled in config.xml
#endif /* WIN32 */
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
