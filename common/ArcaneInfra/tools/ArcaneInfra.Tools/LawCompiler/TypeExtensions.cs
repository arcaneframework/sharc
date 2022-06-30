using System;

namespace LawCompiler
{
  public static class TypeExtensions
  {
    public static string ToObjectName(this Type t) 
    {
      switch(t) {
      case Type.real: return "Real";
      case Type.integer: return "Integer";
      case Type.boolean: return "Boolean";
      default: return "undefined";
      }
    }
  }
}

