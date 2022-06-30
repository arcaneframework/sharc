using System;
using System.Linq;
using System.Collections.Generic;

namespace LawCompiler
{
  public partial class LawT4
  {
    public law Model { get; private set; }

    public IEnumerable<Property> Inputs { get; private set; }
    public IEnumerable<Property> Outputs { get; private set; }
    public IEnumerable<Property> InOutputs { get; private set; }
    public IEnumerable<Property> Parameters { get; private set; }

    public IEnumerable<Property> AllProperties { get; private set; }

    public String Signature { get; private set; }
    public String CallSignature { get; private set; }
    public String ArgSignature { get; private set; }

    public String InvokerCallSignature { get; private set; }
	
    public String InvokerCallSignatureWithArrayDerivate { get; private set; }
	
    public bool Debug { get; private set; }

    public MultiThreadMode MultiThread { get; private set; }

    public LawT4 (law model, bool debug, MultiThreadMode MultiThreadValue)
    {
      Model = model;
      MultiThread = MultiThreadValue;

      if (Model.input != null) {
        Inputs = new List<Property> (Model.input);
      } else {
        Inputs = new List<Property> ();
      }

      if (Model.output != null) {
        Outputs = new List<Property> (Model.output);
      } else {
        Outputs = new List<Property> ();
      }

      if (Model.parameter != null) {
        Parameters = new List<Property> (Model.parameter);
      } else {
        Parameters = new List<Property> ();
      }
      
      var inout = new List<Property> ();
      inout.AddRange (Inputs);
      inout.AddRange (Outputs);
      InOutputs = inout;

      var all = new List<Property> ();
      all.AddRange (Inputs);
      all.AddRange (Outputs);
      all.AddRange (Parameters);
      AllProperties = all;

      Debug = debug;
      
      {
        var types = new List<String> ();
        types.AddRange (Inputs.Select (prop => prop.InSignatureType));
        foreach (var prop in Outputs) {
          types.Add (prop.OutSignatureType);
          foreach (var p in Inputs) {
            types.Add (prop.outSignatureType (p));
          }
        }
        types.AddRange (Parameters.Select (prop => prop.InSignatureType));
        Signature = String.Join (",", types);
      }
      {
        int i = 0;
        var types = new List<String> ();
        foreach (var prop in Inputs) {
          types.Add (prop.InSignatureType + " a" + i);
          i++;
        }
        foreach (var prop in Outputs) {
          types.Add (prop.OutSignatureType  + " a" + i);
          i++;
          foreach (var p in Inputs) {
            types.Add (prop.outSignatureType (p) + " a" + i);
            i++;
          }
        }
        foreach (var prop in Parameters) {
          types.Add (prop.InSignatureType + " a" + i);
          i++;
        }
        CallSignature = String.Join (",", types);
        var args = new List<String> ();
        for (int j = 0; j < i; ++j) {
          args.Add ("a" + j);
        }
        ArgSignature = String.Join (",", args);
      }
      {
        var names = new List<String> ();
        names.AddRange (Inputs.Select (prop => prop.name));
        foreach (var prop in Outputs) {
          names.Add (prop.name);
          foreach (var wrt in Inputs) {
            names.Add (prop.name + "_wrt_" + wrt.name);
          }
        }
        names.AddRange (Parameters.Select (prop => prop.name));
        InvokerCallSignature = String.Join (",", names);
      }
      {
		var names = new List<String> ();
		names.AddRange (Inputs.Select (prop => prop.name));
		foreach (var prop in Outputs) {
		  names.Add (prop.name);
		  int index = 0;
		  foreach (var wrt in Inputs) {
			names.Add (prop.name + "_wrt_in[" + index +"]");
			index++;
		  }
		}
		names.AddRange (Parameters.Select (prop => prop.name));
		InvokerCallSignatureWithArrayDerivate = String.Join (",", names);
	  }
    }
  }
}

