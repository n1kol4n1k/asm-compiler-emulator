#include "../inc/asmSymTable.h"

namespace assembler
{
  word SymTable::GetSymbolValue(std::string name, std::string sec, addressType locationCounter)
  {
    //any case where we want to get symbol value, but we don't add that value to machine code?
    auto symbol = m_Table.find(name);
    if(symbol != m_Table.end()) //found, but what if isKnown == false? In that case, save locCounter in forward table
    {
      if(symbol->second.isKnown == false)
      {
        LocationInfo info;
        info.section = sec;
        info.address = locationCounter;
        m_AdvancingTable.push_back({name, info});
      }
      return symbol->second.value;
    }
    else //addressing in advance
    {
      Content cnt;
      m_Table.insert({name, cnt});
      LocationInfo info;
      info.section = sec;
      info.address = locationCounter;
      m_AdvancingTable.push_back({name, info});
      return cnt.value;
    }
  }

  void SymTable::RegisterGlobal(std::string name)
  {
    m_Table[name].bind = SymBind::GLOBAL;
  }

  void SymTable::RegisterExtern(std::string name)
  {
    //errors? What if we try to assign value to extern symbol
    m_Table[name].bind = SymBind::EXTERN;
  }

  void SymTable::AssignValue(std::string name, word value, std::string section)
  {
    m_Table[name].isKnown = true;
    m_Table[name].value = value;
    m_Table[name].section = section;
  }
}