#include <unordered_map>
#include <vector>
#include <string>
#include "types.h"

namespace assembler
{
  enum SymType
  {
    NOTYP,
    SCTN
  };

  enum SymBind
  {
    GLOBAL, 
    LOCAL
  };

  class SymTable 
  {
    private:
      struct Content
      {
        word value = 0;
        ubyte size;
        SymType type;
        SymBind bind = SymBind::LOCAL;
        ubyte ndx;
        bool isKnown = false;
      };

      std::unordered_map<std::string, Content> m_Table;
      
      struct LocationInfo
      {
        std::string section;
        addressType address;
      };
      std::vector<std::pair<std::string, LocationInfo>> m_AdvancingTable;

    public:
      word GetSymbolValue(std::string name, std::string sec, addressType locationCounter);
      void RegisterGlobal(std::string name);
      void AssignValue(std::string name, word value);
      std::vector<std::pair<std::string, LocationInfo>> GetAdvancingTable() { return m_AdvancingTable; };
      bool IsKnown(std::string name) { return m_Table[name].isKnown; }
      bool SymbolExist(std::string name) { return m_Table.find(name) != m_Table.end(); }
  };
}