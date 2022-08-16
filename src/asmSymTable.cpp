#include "../inc/asmSymTable.h"

namespace assembler
{
  word SymTable::GetSymbolValue(std::string name, addressType locationCounter)
  {
    auto symbol = m_Table.find(name);
    if(symbol != m_Table.end()) //found
    {
      return symbol->second.value;
    }
    else //addressing in advance
    {
      Content cnt;
      m_Table.insert({name, cnt});
      //treba nam loccounter za tabelu obracanja unapred, da znamo gde da uradimo ispravku
      return cnt.value;
    }
  }

  void SymTable::RegisterGlobal(std::string name)
  {
    m_Table[name].bind = SymBind::GLOBAL;
  }

  void SymTable::AssignValue(std::string name, word value)
  {
    m_Table[name].isKnown = true;
    m_Table[name].value = value;
  }
}