#include "asmSymTable.h"

#define ASMManager assembler::Manager::GetInstance()

namespace assembler
{
  class Manager
  {
    private:
      Manager();
      static Manager* inst;

      SymTable m_Table;
      addressType m_LocationCounter = 0;
      std::unordered_map<std::string, std::vector<ubyte>> m_MachineCode; 
      std::string m_CurrSection;
      //bool -> isSym
      using argInfo = std::pair<std::string, bool>;
      std::vector<argInfo> m_CurrArgs;
      std::vector<std::string> m_CurrLabels;

      addressType m_PrevLocation = 0; //for same line label, ali samo kada je uz to direktiva koja sadrzi sadrzaj
      bool m_IsContentOp = false;

    public:
      static Manager& GetInstance();

      void ProcessLabel(std::string name);
      void ProcessSameLineLabel(std::string name);
      void ProcessGlobal();
      void ProcessExtern();
      void ProcessSection(std::string name);
      void ProcessWord();
      void ProcessSkip(int literal);

      void RegisterSymArg(std::string arg);
      void RegisterNumArg(int arg);

    private: //helper functions
      void InsertWord(std::string secName, addressType locCounter, word value);
      void AssignLabels();
  };
}