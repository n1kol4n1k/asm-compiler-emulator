#include <unordered_map>
#include <vector>
#include <string>
#include "types.h"

namespace assembler
{
  enum RelocType
  {
    r_word, 
    r_pc
  };

  class RelocationTable 
  {
    private:
      struct Content
      {
        addressType offset;
        RelocType type;
        std::string symbol;
        byte addent;
      };

      std::unordered_map<std::string, std::vector<Content>> m_Table;

    public:
      void RegisterRelocation(std::string section, addressType off, RelocType type, std::string sym, byte add);
      inline std::vector<Content>& GetRelocTable(std::string sec) { return m_Table[sec]; }
  };
}