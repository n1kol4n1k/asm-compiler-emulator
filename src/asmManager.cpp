#include "../inc/asmManager.h"
#include <iostream>

namespace assembler
{
  Manager* Manager::inst = nullptr;

  Manager& Manager::GetInstance()
  {
    if(inst == nullptr)
    {
      inst = new Manager();
    }
    return *inst;
  }

  Manager::Manager() 
  {
    m_CurrSection = "undefined";
  }

  void Manager::RegisterSymArg(std::string arg)
  {
    argInfo info = std::make_pair(arg, true);
    m_CurrArgs.push_back(info);
  }
  void Manager::RegisterNumArg(int arg)
  {
    argInfo info = std::make_pair(std::to_string(arg), false);
    m_CurrArgs.push_back(info);
  }

  void Manager::ProcessLabel(std::string name)
  {
    m_IsContentOp = false;
    m_CurrLabels.push_back(name);
  }
  void Manager::ProcessSameLineLabel(std::string name)
  {
    if(m_IsContentOp == true)
    {
      m_Table.AssignValue(name, m_PrevLocation, m_CurrSection);
      m_IsContentOp = false;
    }
    //not content op in same line, push to section list
    else
    {
      m_CurrLabels.push_back(name);
    }
  }

  void Manager::ProcessGlobal()
  {
    m_IsContentOp = false;
    for(auto it : m_CurrArgs)
    {
      m_Table.RegisterGlobal(it.first);
    }
    m_CurrArgs.clear();
  }
  void Manager::ProcessExtern()
  {
    m_IsContentOp = false;
    //linker?
    
  }

  void Manager::ProcessSection(std::string name)
  {
    m_IsContentOp = false;
    auto section = m_MachineCode.find(name);
    if(section != m_MachineCode.end()) //found section
    {
      m_LocationCounter = m_MachineCode[name].size();
    }
    else
    {
      m_MachineCode.insert({name, std::vector<ubyte>()});
      m_LocationCounter = 0;
    }
    m_CurrSection = name;
    //u projektu se spominju sekcije sa istim imenom, sta onda?
  }

  void Manager::ProcessWord()
  {
    AssignLabels();
    for(auto it = m_CurrArgs.rbegin(); it != m_CurrArgs.rend(); it++)
    {
      if(it->second == false) //literal
      {
        int num = std::stoi(it->first);
        if(num < WORD_MIN || num > WORD_MAX)
        {
          //print error
          return;
        }
        m_MachineCode[m_CurrSection].push_back(num);
        m_MachineCode[m_CurrSection].push_back(num>>8);
      }
      else //symbol
      {
        word symValue = m_Table.GetSymbolValue(it->first, m_CurrSection, m_LocationCounter);
        m_MachineCode[m_CurrSection].push_back(symValue);
        m_MachineCode[m_CurrSection].push_back(symValue>>8);
      }
      m_LocationCounter+=2;
    }
    std::cout<<"\n";
    m_CurrArgs.clear();
  }

  void Manager::ProcessSkip(int literal)
  {
    AssignLabels();
    for(int i = 0; i < literal; i++)
    {
      m_MachineCode[m_CurrSection].push_back(0);
    }
    m_LocationCounter+=literal;
  }

  void Manager::ProcessEnd()
  {
    m_IsContentOp = false;
  }
  
  void Manager::FillPrevUnknownValues()
  {
    for(auto it : m_Table.GetAdvancingTable())
    { 
      std::string symName = it.first;
      if(m_Table.SymbolExist(symName) == false)
      {
        std::cerr<<"Abnormal behavior - symbol: "<<symName<<" exist in AdvancingTable but not in SymbolTable\n";
        return;
      }
      if(m_Table.IsKnown(symName) == false) //but extern.. ?
      {
        std::cerr<<"Symbol: "<<symName<<" value was used but not initialized\n";
        return;
      }
      word symValue = m_Table.GetSymbolValue(symName, m_CurrSection, m_LocationCounter);
      InsertWord(it.second.section, it.second.address, symValue);
    }
  }

  //helpers

  inline void Manager::InsertWord(std::string secName, addressType locCounter, word value)
  {
    m_MachineCode[secName][locCounter] = value;
    m_MachineCode[secName][++locCounter] = (value>>8);
  }
  inline void Manager::AssignLabels()
  {
    for(std::string label : m_CurrLabels)
    {
      m_Table.AssignValue(label, m_LocationCounter, m_CurrSection);
    }
    m_CurrLabels.clear();
    m_IsContentOp = true;
    m_PrevLocation = m_LocationCounter;
  }
  /*TODO: obracanje unapred.
  Testirati sekcije, generisanje koda.
  Tabela simbola, pogledati snimke za ASM
  Izlazni sadrzaj, bin datoteka?
  */
}