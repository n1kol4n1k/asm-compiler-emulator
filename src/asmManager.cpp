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

  //Assembly directives

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

  //Assembly commands

  void Manager::ProcessHalt()
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_halt);
    m_LocationCounter++;
  }
  void Manager::ProcessInt(std::string regD)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_int);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), 0b1111);
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessIret()
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_iret);
    m_LocationCounter++;
  }
  void Manager::ProcessCall(operandInfo op)
  {
    std::string symbol;
    std::string reg;
    UnpackStrings(op, symbol, reg);
    std::cout<<"Call - Type: "<<op.type<<" Symbol: "<<symbol<<" Reg: "<<reg<<" Literal: "<<op.literal<<"\n";
  }
  void Manager::ProcessRet()
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_ret);
    m_LocationCounter++;
  }
  void Manager::ProcessJmp(operandInfo op)
  {
    AssignLabels();
    ProcessJumpInstruction(InstructionTypes::c_jmp, op);
  }
  void Manager::ProcessJeq(operandInfo op)
  {
    
  }
  void Manager::ProcessJne(operandInfo op)
  {
    
  }
  void Manager::ProcessJgt(operandInfo op)
  {
    
  }
  void Manager::ProcessPush(std::string regD)
  {
    //to be done with combination of other instructions
  }
  void Manager::ProcessPop(std::string regD)
  {
    //to be done with combination of other instructions
  }
  void Manager::ProcessXchg(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_xchg);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessAdd(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_add);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessSub(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_sub);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessMul(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_mul);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessDiv(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_div);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessCmp(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_cmp);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessNot(std::string regD)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_not);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), 0b1111);
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessAnd(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_and);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessOr(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_or);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessXor(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_xor);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessTest(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_test);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessShl(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_shl);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessShr(std::string regD, std::string regS)
  {
    AssignLabels();
    m_MachineCode[m_CurrSection].push_back(InstructionTypes::c_shr);
    ubyte regsDescr = CreateByte(RegNameToIndex(regD), RegNameToIndex(regS));
    m_MachineCode[m_CurrSection].push_back(regsDescr);
    m_LocationCounter += 2;
  }
  void Manager::ProcessLdr(std::string regD, operandInfo op)
  {
    
  }
  void Manager::ProcessStr(std::string regD, operandInfo op)
  {
    
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

  void Manager::UnpackStrings(operandInfo& op, std::string& symbol, std::string& reg)
  {
    if(op.symbol != nullptr)
    {
      symbol = op.symbol;
    }
    else
    {
      symbol = "";
    }
    if(op.reg != nullptr)
    {
      reg = op.reg;
    }
    else
    {
      reg = "";
    }
  }

  ubyte Manager::RegNameToIndex(std::string reg)
  {
    if(reg == "r0")
    {
      return 0b0000;
    }
    else if(reg == "r1")
    {
      return 0b0001;
    }
    else if(reg == "r2")
    {
      return 0b0010;
    }
    else if(reg == "r3")
    {
      return 0b0011;
    }
    else if(reg == "r4")
    {
      return 0b0100;
    }
    else if(reg == "r5")
    {
      return 0b0101;
    }
    else if(reg == "r6")
    {
      return 0b0110;
    }
    else if(reg == "r7")
    {
      return 0b0111;
    }
    else if(reg == "psw")
    {
      return 0b1000;
    }
    else 
    {
      std::cerr<<"Greska! Registar: "<<reg<<" ne postoji u sistemu!\n";
      return 0b1111;
    }
  }

  ubyte Manager::CreateByte(ubyte up_4b, ubyte low_4b)
  {
    ubyte ret = low_4b;
    ret |= up_4b << 4;
    return ret;
  }

  void Manager::ProcessJumpInstruction(InstructionTypes jumpType, operandInfo op)
  {
    m_MachineCode[m_CurrSection].push_back(jumpType);
    m_LocationCounter++;

    ubyte regsDescr;
    ubyte addrMode;
    if(op.type == OperandSyntax::noPrefixLiteral)
    {
      regsDescr = CreateByte(0b1111, 0b1111);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_not_used, AdressingMethod::c_imm);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;

      int num = op.literal;
      if(num < WORD_MIN || num > WORD_MAX)
      {
        //print error
        return;
      }
      m_MachineCode[m_CurrSection].push_back(num>>8);
      m_MachineCode[m_CurrSection].push_back(num);
      m_LocationCounter += 2;
    }
    else if(op.type == OperandSyntax::noPrefixSymbol)
    {
      //linker ce srediti ovo ako bude razmestanja
      regsDescr = CreateByte(0b1111, 0b1111);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_not_used, AdressingMethod::c_imm);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;
      //zeroes? Because linker will fix regardless
      word symValue = m_Table.GetSymbolValue(op.symbol, m_CurrSection, m_LocationCounter);

      m_MachineCode[m_CurrSection].push_back(symValue>>8);
      m_MachineCode[m_CurrSection].push_back(symValue);
      m_LocationCounter += 2;
    }
    else if(op.type == OperandSyntax::percentSymbol) // PC rel
    {
      //poslednje vezbe
      regsDescr = CreateByte(0b1111, RegNameToIndex("r7")); //PC = R7
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_not_used, AdressingMethod::c_regdir_addition);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;
      //linker will fix this with PC rel
      m_MachineCode[m_CurrSection].push_back(0);
      m_MachineCode[m_CurrSection].push_back(0);
      m_LocationCounter += 2;
      //relocation table?
    }
    else if(op.type == OperandSyntax::asteriskLiteral)
    {
      regsDescr = CreateByte(0b1111, 0b1111);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_not_used, AdressingMethod::c_mem);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;

      int num = op.literal;
      if(num < WORD_MIN || num > WORD_MAX)
      {
        //print error
        return;
      }
      m_MachineCode[m_CurrSection].push_back(num>>8);
      m_MachineCode[m_CurrSection].push_back(num);
      m_LocationCounter += 2;
    }
    else if(op.type == OperandSyntax::asteriskSymbol)
    {
      //linker ce srediti ovo ako bude razmestanja
      regsDescr = CreateByte(0b1111, 0b1111);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_not_used, AdressingMethod::c_mem);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;
      //zeroes? Because linker will fix regardless
      word symValue = m_Table.GetSymbolValue(op.symbol, m_CurrSection, m_LocationCounter);

      m_MachineCode[m_CurrSection].push_back(symValue>>8);
      m_MachineCode[m_CurrSection].push_back(symValue);
      m_LocationCounter += 2;
    }
    else if(op.type == OperandSyntax::asteriskReg)
    {
      regsDescr = CreateByte(0b1111, RegNameToIndex(op.reg));
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_not_used, AdressingMethod::c_regdir);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;
    }
    else if(op.type == OperandSyntax::asteriskBracketReg)
    {
      regsDescr = CreateByte(0b1111, RegNameToIndex(op.reg));
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_noup, AdressingMethod::c_regind);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;
    }
    else if(op.type == OperandSyntax::asteriskBracketRegLiteral)
    {
      regsDescr = CreateByte(0b1111, RegNameToIndex(op.reg));
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_noup, AdressingMethod::c_regind_addition);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;

      int num = op.literal;
      if(num < WORD_MIN || num > WORD_MAX)
      {
        //print error
        return;
      }
      m_MachineCode[m_CurrSection].push_back(num>>8);
      m_MachineCode[m_CurrSection].push_back(num);
      m_LocationCounter += 2;
    }
    else if(op.type == OperandSyntax::asteriskBracketRegSymbol)
    {
      regsDescr = CreateByte(0b1111, RegNameToIndex(op.reg));
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      addrMode = CreateByte(RegIndUpdate::c_noup, AdressingMethod::c_regind_addition);
      m_MachineCode[m_CurrSection].push_back(regsDescr);
      m_LocationCounter += 2;

      //zeroes? Because linker will fix regardless
      word symValue = m_Table.GetSymbolValue(op.symbol, m_CurrSection, m_LocationCounter);

      m_MachineCode[m_CurrSection].push_back(symValue>>8);
      m_MachineCode[m_CurrSection].push_back(symValue);
      m_LocationCounter += 2;
    }
    else
    {
      std::cerr<<"Error: jump instructions don't support chosen operand syntax\n";
    }
  }
  
  /*TODO: obracanje unapred.
  Testirati sekcije, generisanje koda.
  Tabela simbola, pogledati snimke za ASM
  Izlazni sadrzaj, bin datoteka?
  */
}