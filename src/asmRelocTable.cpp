#include "../inc/asmRelocTable.h"

namespace assembler
{
  void RelocationTable::RegisterRelocation(std::string section, addressType off, RelocType type, std::string sym, byte add)
  {
    Content info;
    info.offset = off;
    info.type = type;
    info.symbol = sym;
    info.addent = add;
    m_Table[section].push_back(info);
  }
}