using System;

namespace LawCompiler
{
  public static class DimensionExtensions
  {
    public static string ToObjectName(this Dimension d) 
    {
      switch(d) {
      case Dimension.scalar: return "Scalar";
      case Dimension.vectorial: return "Vectorial";
      default: return "undefined";
      }
    }
  }
}

