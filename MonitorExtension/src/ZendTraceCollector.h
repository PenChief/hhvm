#ifndef ZENDTRACECOLLECTOR_H
#define ZENDTRACECOLLECTOR_H

#include <string>
#include <sstream>
#include <list>

namespace HPHP
{
  
/**
 * @class ZendFunctionInfo
 * @brief a class representing stack function
 */
struct ZendFunctionInfo {
  std::string m_name;
  std::string m_filename;
  int m_linenumber;
  std::string m_args;
  
  ZendFunctionInfo() : m_linenumber(-1) 
  {}
  
  ZendFunctionInfo(const ZendFunctionInfo& other) {
    *this = other;
  }
  
  ZendFunctionInfo& operator=(const ZendFunctionInfo& other) {
    this->m_filename   = other.m_filename;
    this->m_linenumber = other.m_linenumber;
    this->m_name       = other.m_name;
    this->m_args       = other.m_args;
    return *this;
  }
};

/**
 * @class ZendFrame
 * @brief a class representing a stack entry
 */
struct ZendFrame {
  ZendFunctionInfo m_func;
  int m_level;

  ZendFrame(const ZendFunctionInfo& func, int level)
    : m_func(func)
    , m_level(level)
  {}
  
  ZendFrame() : m_level(-1)
  {}
};

class ZendTraceCollector
{
public:
  typedef std::list<ZendFrame> FrameList_t;

protected:
  int         m_frame;
  FrameList_t m_frames;
  bool        m_collecting;

public:
  ZendTraceCollector();
  virtual ~ZendTraceCollector();

  void dump();
  void enterFunction(const ZendFunctionInfo& zfi);
  void leaveFunction(const ZendFunctionInfo& zfi);
  void clear();
  void onFatalError(const std::string& errorString);

  void setCollecting(bool collecting) {
    this->m_collecting = collecting;
  }
  bool isCollecting() const {
    return m_collecting;
  }
};

}

#endif // ZENDTRACECOLLECTOR_H
