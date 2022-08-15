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
        word value;
        ubyte size;
        SymType type;
        SymBind bind;
        ubyte ndx;
      };

      std::unordered_map<std::string, Content> m_Table;

    public:
      word GetSymbolValue(std::string name, addressType locationCounter);
  };
}