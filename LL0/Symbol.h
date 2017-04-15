#pragma once
#include <string>


namespace LL0
{
  
  class Symbol
  {
  public:
    Symbol()
    {
      vindex  = 0;
      scope   = -1;
    }

    static Symbol null() { return Symbol(); }

  public:
    bool  isNull() const { return vindex==0; }
    

  public:
    std::string name;
    int vindex;
    int scope;

  };

}
