#include "../inc/asmSymTable.h"
#include <iostream>

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
    if(m_Table[name].isKnown == true)
    {
      std::cerr<<"Error: tried to declare as extern symbol that is assigned in this module\n";
      return;
    }
    m_Table[name].bind = SymBind::EXTERN;
  }

  void SymTable::AssignValue(std::string name, word value, std::string section, bool isSection)
  {
    if(m_Table[name].bind == SymBind::EXTERN)
    {
      std::cerr<<"Error: tried to assign value to extern symbol\n";
      return;
    }
    if(isSection == true && m_Table[name].bind == SymBind::GLOBAL)
    {
      std::cerr<<"Error: tried to create a section to global symbol\n";
      return;
    }
    if(isSection == true)
    {
      m_Table[name].type = SymType::SCTN;
    }
    else
    {
      m_Table[name].type = SymType::NOTYP;
    }
    m_Table[name].isKnown = true;
    m_Table[name].value = value;
    m_Table[name].section = section;
  }

  void SymTable::InsertIfNotExist(std::string name)
  {
    Content cnt;
    m_Table.insert({name, cnt});
  }

  bool SymTable::IsUndefined()
  {
    for(auto it : m_Table)
    {
      if(it.second.bind != SymBind::GLOBAL && it.second.isKnown == false)
      {
        return true;
      }
    }
    return false;
  }
  
  std::vector<std::string> SymTable::GetSections()
  {
    std::vector<std::string> ret;
    for(auto it : m_Table)
    {
      if(it.second.type == SymType::SCTN)
      {
        ret.push_back(it.first);
      }
    }
    return ret;
  }

}