﻿// ------------------------------------------------------------------------------
//  <autogenerated>
//      This code was generated by a tool.
//      Mono Runtime Version: 4.0.30319.17020
// 
//      Changes to this file may cause incorrect behavior and will be lost if 
//      the code is regenerated.
//  </autogenerated>
// ------------------------------------------------------------------------------

namespace ExpressionBuilderGenerator {
    using System;
    
    
    public partial class AxlGenerator : AxlGeneratorBase {
        
        private int _nField;
        public int n {
            get {
                return this._nField;
            }
        }
        private int _mField;
        public int m {
            get {
                return this._mField;
            }
        }

        
        public virtual string TransformText() {
            this.GenerationEnvironment = null;
            
            #line 4 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n\n<!--\n  <autogenerated>\n    GENERATED FILE : DO NOT MODIFY THIS FILE\n    This code was generated by a tool. \n    Changes to this file may cause incorrect behavior and will be lost if \n    the code is regenerated.\n    Date of generation : \n    Generator : common/ArcanInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator\n  </autogenerated>\n-->\n\n<service name=\"ExpressionBuilderR");
            
            #line default
            #line hidden
            
            #line 17 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(n));
            
            #line default
            #line hidden
            
            #line 17 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write("vR");
            
            #line default
            #line hidden
            
            #line 17 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(m));
            
            #line default
            #line hidden
            
            #line 17 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write("\" version=\"1.0\" type=\"caseoption\">\n  <description>Service ExpressionBuilderR");
            
            #line default
            #line hidden
            
            #line 18 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(n));
            
            #line default
            #line hidden
            
            #line 18 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write("vR");
            
            #line default
            #line hidden
            
            #line 18 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(m));
            
            #line default
            #line hidden
            
            #line 18 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write("</description>\n  <interface name=\"IFunctionR");
            
            #line default
            #line hidden
            
            #line 19 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(n));
            
            #line default
            #line hidden
            
            #line 19 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write("vR");
            
            #line default
            #line hidden
            
            #line 19 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(m));
            
            #line default
            #line hidden
            
            #line 19 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator/AxlGenerator.tt"
            this.Write("\" inherited=\"false\" />\n\n  <options>\n    <complex name=\"constant\" type=\"ConstantType\" minOccurs=\"0\" maxOccurs=\"unbounded\">\n      <description>Constante locale de l'expression</description>\n      <simple name=\"name\" type=\"string\">\n        <description>Nom de la constante</description>\n      </simple>\n      <simple name=\"value\" type=\"real\">\n        <description>Valeur de la constante</description>\n      </simple>\n    </complex>\n    \n    <simple name=\"expression\" type=\"string\">\n      <description>Expression</description>\n    </simple>\n  \n  </options>\n</service>\n\t\n");
            
            #line default
            #line hidden
            return this.GenerationEnvironment.ToString();
        }
        
        protected virtual void Initialize() {
            if ((this.Errors.HasErrors == false)) {
                bool _nAcquired = false;
                if (((this.Session != null) && this.Session.ContainsKey("n"))) {
                    object data = this.Session["n"];
                    if (typeof(int).IsAssignableFrom(data.GetType())) {
                        this._nField = ((int)(data));
                        _nAcquired = true;
                    }
                    else {
                        this.Error("The type 'System.Int32' of the parameter 'n' did not match the type passed to the template");
                    }
                }
                if ((_nAcquired == false)) {
                    object data = System.Runtime.Remoting.Messaging.CallContext.LogicalGetData("n");
                    if ((data != null)) {
                        if (typeof(int).IsAssignableFrom(data.GetType())) {
                            this._nField = ((int)(data));
                            _nAcquired = true;
                        }
                        else {
                            this.Error("The type 'System.Int32' of the parameter 'n' did not match the type passed to the template");
                        }
                    }
                }
                bool _mAcquired = false;
                if (((this.Session != null) && this.Session.ContainsKey("m"))) {
                    object data = this.Session["m"];
                    if (typeof(int).IsAssignableFrom(data.GetType())) {
                        this._mField = ((int)(data));
                        _mAcquired = true;
                    }
                    else {
                        this.Error("The type 'System.Int32' of the parameter 'm' did not match the type passed to the template");
                    }
                }
                if ((_mAcquired == false)) {
                    object data = System.Runtime.Remoting.Messaging.CallContext.LogicalGetData("m");
                    if ((data != null)) {
                        if (typeof(int).IsAssignableFrom(data.GetType())) {
                            this._mField = ((int)(data));
                            _mAcquired = true;
                        }
                        else {
                            this.Error("The type 'System.Int32' of the parameter 'm' did not match the type passed to the template");
                        }
                    }
                }
            }

        }
    }
    
    public class AxlGeneratorBase {
        
        private global::System.Text.StringBuilder builder;
        
        private global::System.Collections.Generic.IDictionary<string, object> session;
        
        private global::System.CodeDom.Compiler.CompilerErrorCollection errors;
        
        private string currentIndent = string.Empty;
        
        private global::System.Collections.Generic.Stack<int> indents;
        
        private ToStringInstanceHelper _toStringHelper = new ToStringInstanceHelper();
        
        public virtual global::System.Collections.Generic.IDictionary<string, object> Session {
            get {
                return this.session;
            }
            set {
                this.session = value;
            }
        }
        
        public global::System.Text.StringBuilder GenerationEnvironment {
            get {
                if ((this.builder == null)) {
                    this.builder = new global::System.Text.StringBuilder();
                }
                return this.builder;
            }
            set {
                this.builder = value;
            }
        }
        
        protected global::System.CodeDom.Compiler.CompilerErrorCollection Errors {
            get {
                if ((this.errors == null)) {
                    this.errors = new global::System.CodeDom.Compiler.CompilerErrorCollection();
                }
                return this.errors;
            }
        }
        
        public string CurrentIndent {
            get {
                return this.currentIndent;
            }
        }
        
        private global::System.Collections.Generic.Stack<int> Indents {
            get {
                if ((this.indents == null)) {
                    this.indents = new global::System.Collections.Generic.Stack<int>();
                }
                return this.indents;
            }
        }
        
        public ToStringInstanceHelper ToStringHelper {
            get {
                return this._toStringHelper;
            }
        }
        
        public void Error(string message) {
            this.Errors.Add(new global::System.CodeDom.Compiler.CompilerError(null, -1, -1, null, message));
        }
        
        public void Warning(string message) {
            global::System.CodeDom.Compiler.CompilerError val = new global::System.CodeDom.Compiler.CompilerError(null, -1, -1, null, message);
            val.IsWarning = true;
            this.Errors.Add(val);
        }
        
        public string PopIndent() {
            if ((this.Indents.Count == 0)) {
                return string.Empty;
            }
            int lastPos = (this.currentIndent.Length - this.Indents.Pop());
            string last = this.currentIndent.Substring(lastPos);
            this.currentIndent = this.currentIndent.Substring(0, lastPos);
            return last;
        }
        
        public void PushIndent(string indent) {
            this.Indents.Push(indent.Length);
            this.currentIndent = (this.currentIndent + indent);
        }
        
        public void ClearIndent() {
            this.currentIndent = string.Empty;
            this.Indents.Clear();
        }
        
        public void Write(string textToAppend) {
            this.GenerationEnvironment.Append(textToAppend);
        }
        
        public void Write(string format, params object[] args) {
            this.GenerationEnvironment.AppendFormat(format, args);
        }
        
        public void WriteLine(string textToAppend) {
            this.GenerationEnvironment.Append(this.currentIndent);
            this.GenerationEnvironment.AppendLine(textToAppend);
        }
        
        public void WriteLine(string format, params object[] args) {
            this.GenerationEnvironment.Append(this.currentIndent);
            this.GenerationEnvironment.AppendFormat(format, args);
            this.GenerationEnvironment.AppendLine();
        }
        
        public class ToStringInstanceHelper {
            
            private global::System.IFormatProvider formatProvider = global::System.Globalization.CultureInfo.InvariantCulture;
            
            public global::System.IFormatProvider FormatProvider {
                get {
                    return this.formatProvider;
                }
                set {
                    if ((this.formatProvider == null)) {
                        throw new global::System.ArgumentNullException("formatProvider");
                    }
                    this.formatProvider = value;
                }
            }
            
            public string ToStringWithCulture(object objectToConvert) {
                if ((objectToConvert == null)) {
                    throw new global::System.ArgumentNullException("objectToConvert");
                }
                global::System.Type type = objectToConvert.GetType();
                global::System.Type iConvertibleType = typeof(global::System.IConvertible);
                if (iConvertibleType.IsAssignableFrom(type)) {
                    return ((global::System.IConvertible)(objectToConvert)).ToString(this.formatProvider);
                }
                global::System.Reflection.MethodInfo methInfo = type.GetMethod("ToString", new global::System.Type[] {
                            iConvertibleType});
                if ((methInfo != null)) {
                    return ((string)(methInfo.Invoke(objectToConvert, new object[] {
                                this.formatProvider})));
                }
                return objectToConvert.ToString();
            }
        }
    }
}
