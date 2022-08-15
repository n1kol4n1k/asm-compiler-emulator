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
    
  }
  void Manager::ProcessGlobal()
  {
    std::cout<<"Global sa arg: ";
    for(auto it : m_CurrArgs)
    {
      std::cout<<it.first<<"-"<<it.second<<" ";
    }
    std::cout<<"\n";
    m_CurrArgs.clear();
  }
  void Manager::ProcessExtern()
  {
    std::cout<<"Extern sa arg: ";
    for(auto it : m_CurrArgs)
    {
      std::cout<<it.first<<"-"<<it.second<<" ";
    }
    std::cout<<"\n";
    m_CurrArgs.clear();
  }
  void Manager::ProcessSection(std::string name)
  {

  }
  void Manager::ProcessWord()
  {
    std::cout<<"Word sa arg: ";
    for(auto it : m_CurrArgs)
    {
      std::cout<<it.first<<"-"<<it.second<<" ";
      if(it.second == false) //literal
      {
        int num = std::stoi(it.first);
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
        
      }
      m_LocationCounter+=2;
    }
    std::cout<<"\n";
    m_CurrArgs.clear();
  }
  void Manager::ProcessSkip(int literal)
  {
 
  }
  inline void Manager::InsertWord(std::string secName, addressType locCounter, word value)
  {
    m_MachineCode[secName][locCounter] = value;
    m_MachineCode[secName][locCounter] = (value>>8);
  }

  //TODO: sekcije, labele, extern, global
}