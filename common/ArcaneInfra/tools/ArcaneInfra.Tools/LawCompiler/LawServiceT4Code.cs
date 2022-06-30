using System;
using System.Collections.Generic;
using System.Linq;

namespace LawCompiler
{
  public partial class LawServiceHeaderT4
  {
    public Law Law { get; set; }

    public LawServiceHeaderT4 ()
    {
    }
      
    public IEnumerable<Property> Outputs { 
      get {
        var p = Law.outputs;
        if(p == null) {
          return new List<Property>();
        } else {
          return new List<Property>(p);
        }
      }
    }

    public IEnumerable<Property> Inputs { 
      get {
        var p = Law.inputs;
        if(p == null) {
          return new List<Property>();
        } else {
          return new List<Property>(p);
        }
      }
    }

    public IEnumerable<Property> Parameters { 
      get {
        var p = Law.parameters;
        if(p == null) {
          return new List<Property>();
        } else {
          return new List<Property>(p);
        }
      }
    }

    public IEnumerable<Property> All { 
      get {
        return Inputs.Concat(Outputs).Concat(Parameters);
      }
    }
    

  }

  public partial class LawServiceCppT4
  {
    public Law Law { get; set; }
    
    public LawServiceCppT4 ()
    {
    }
    
    public IEnumerable<Property> Outputs { 
      get {
        var p = Law.outputs;
        if(p == null) {
          return new List<Property>();
        } else {
          return new List<Property>(p);
        }
      }
    }
    
    public IEnumerable<Property> Inputs { 
      get {
        var p = Law.inputs;
        if(p == null) {
          return new List<Property>();
        } else {
          return new List<Property>(p);
        }
      }
    }
    
    public IEnumerable<Property> Parameters { 
      get {
        var p = Law.parameters;
        if(p == null) {
          return new List<Property>();
        } else {
          return new List<Property>(p);
        }
      }
    }
  }
}

