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

    public:
      word GetSymbolValue(std::string name, addressType locationCounter);
      void RegisterGlobal(std::string name);
      void AssignValue(std::string name, word value);
  };
}