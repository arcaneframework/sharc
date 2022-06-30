using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LawCompiler
{
  public static class EnumerableExtensions
  {
    public static IEnumerable<string> AtBegin(this IEnumerable<string> e, string c) 
    {
      return e.Select (p => c + p);
    }

    public static IEnumerable<string> AtEnd(this IEnumerable<string> e, string c) 
    {
      return e.Select (p => p + c);
    }

    public static string Pack(this IEnumerable<string> e) 
    {
      var sb = new StringBuilder ();
      foreach(var p in e) {
        sb.Append (p);
      }
      return sb.ToString ();
    }

    public static string Pack(this IEnumerable<string> e, string c) 
    {
      var ee = new List<string> (e);
      var sb = new StringBuilder ();
      foreach(var p in ee.Take(ee.Count-1)) {
        sb.Append (p);
        sb.Append (c);
      }
      sb.Append (ee.Last());
      return sb.ToString ();
    }

    public static string ToSignature(this IEnumerable<string> e) 
    {
      var ee = new List<string> (e);
      var sb = new StringBuilder ();
      foreach(var p in ee.Take(ee.Count-1)) {
        sb.Append (p);
        sb.Append (",");
      }
      sb.Append (ee.Last());
      return sb.ToString ();
    }
  }
}

