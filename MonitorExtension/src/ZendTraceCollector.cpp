#include "ZendTraceCollector.h"
#include <iomanip>
#include <iostream>
#include "hphp/runtime/base/execution-context.h"

HPHP::ZendTraceCollector::ZendTraceCollector()
  : m_frame(-1)
  , m_collecting(false)
{
}

HPHP::ZendTraceCollector::~ZendTraceCollector()
{
}

void HPHP::ZendTraceCollector::enterFunction(const ZendFunctionInfo& zfi)
{
  if ( isCollecting() ) {
    ++m_frame;
    ZendFrame frame(zfi, m_frame);
    m_frames.push_back( frame );
  }
}

void HPHP::ZendTraceCollector::leaveFunction(const ZendFunctionInfo& zfi)
{
  (void) zfi;
  if ( isCollecting() ) {
    --m_frame;
  }
}

void HPHP::ZendTraceCollector::dump()
{
  if ( isCollecting() ) {
    std::stringstream ss;
    FrameList_t::const_iterator iter = m_frames.begin();
    for(; iter != m_frames.end(); ++iter ) {
      ss << std::setw(5) << iter->m_level << std::setw(0) << " | ";
      for(int i=0; i<iter->m_level; ++i)
        ss << "  ";
      ss << iter->m_func.m_name << iter->m_func.m_args << " [ " << iter->m_func.m_filename << ", " << iter->m_func.m_linenumber <<  " ]\n";
    }
    std::cout << ss.str() << std::endl;
  }
}

void HPHP::ZendTraceCollector::clear()
{
  m_frames.clear();
  m_frame = -1;
  m_collecting = false;
}

void HPHP::ZendTraceCollector::onFatalError(const std::string& errorString)
{
  if ( isCollecting() ) {
    // Locate the last item on the frames
    if ( !m_frames.empty() ) {
      ZendFrame& fr = *m_frames.rbegin();
      
      // add exception frame
      ZendFunctionInfo zfi;
      zfi.m_name = "***ERROR*** : " + errorString;
      zfi.m_linenumber = g_vmContext->getLastErrorLine();
      zfi.m_filename   = g_vmContext->getLastErrorPath().data();
      ZendFrame frame(zfi, fr.m_level);
      m_frames.push_back( frame );
    }
  }
}
