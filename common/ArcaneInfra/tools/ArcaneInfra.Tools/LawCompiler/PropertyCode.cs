using System;

namespace LawCompiler
{
  public partial class Property
  {
    public Property ()
    {
    }

    public string PropertyType { 
      get {
        return "Law::" + dimension.ToObjectName () + type.ToObjectName () + "Property";
      }
    }

    public string Name { 
      get {
        return name.Replace("-","_");
      }
    }
  
    public string ObjectName { 
      get {
        return name.Replace("-","");
      }
    }
  }
}

