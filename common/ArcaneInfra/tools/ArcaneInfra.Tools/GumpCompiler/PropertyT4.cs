﻿// ------------------------------------------------------------------------------
//  <autogenerated>
//      This code was generated by a tool.
//      Mono Runtime Version: 4.0.30319.17020
// 
//      Changes to this file may cause incorrect behavior and will be lost if 
//      the code is regenerated.
//  </autogenerated>
// ------------------------------------------------------------------------------

namespace GumpCompiler {
    using System;
    
    
    public partial class PropertyT4 : PropertyT4Base {
        
        public virtual string TransformText() {
            this.GenerationEnvironment = null;
            
            #line 2 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
 
var gump = this.Session["gump"] as gump;
var path = this.Session["path"] as string; 
var property = this.Session["property"] as Property; 
var entities = property.EntitiesOfProperty (gump);

            
            #line default
            #line hidden
            
            #line 8 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("// ------------------------------------------------------------------------------\n//  <autogenerated>\n//      This code was generated by tool ");
            
            #line default
            #line hidden
            
            #line 10 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( GumpCompiler.Version.Name ));
            
            #line default
            #line hidden
            
            #line 10 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(" v");
            
            #line default
            #line hidden
            
            #line 10 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( GumpCompiler.Version.Number ));
            
            #line default
            #line hidden
            
            #line 10 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(".\n//      Changes to this file may cause incorrect behavior and will be lost if \n//      the code is regenerated.\n//  </autogenerated>\n// ------------------------------------------------------------------------------\n#ifndef ");
            
            #line default
            #line hidden
            
            #line 15 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( gump.model.@namespace.ToUpper() ));
            
            #line default
            #line hidden
            
            #line 15 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("_GENERATED_BY_GUMP_");
            
            #line default
            #line hidden
            
            #line 15 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.name.ToUpper() ));
            
            #line default
            #line hidden
            
            #line 15 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("_H\n#define ");
            
            #line default
            #line hidden
            
            #line 16 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( gump.model.@namespace.ToUpper() ));
            
            #line default
            #line hidden
            
            #line 16 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("_GENERATED_BY_GUMP_");
            
            #line default
            #line hidden
            
            #line 16 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.name.ToUpper() ));
            
            #line default
            #line hidden
            
            #line 16 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("_H\n\n#include \"ArcGeoSim/Gump/Property.h\"\n#include \"ArcGeoSim/Physics/Law2/Property.h\"\n#include \"");
            
            #line default
            #line hidden
            
            #line 20 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( path ));
            
            #line default
            #line hidden
            
            #line 20 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("/Properties/PropertyKind.h\"\n");
            
            #line default
            #line hidden
            
            #line 21 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
 
foreach(var e in entities) { 

            
            #line default
            #line hidden
            
            #line 24 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("#include \"");
            
            #line default
            #line hidden
            
            #line 24 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( path ));
            
            #line default
            #line hidden
            
            #line 24 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("/Entities/");
            
            #line default
            #line hidden
            
            #line 24 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( e.name ));
            
            #line default
            #line hidden
            
            #line 24 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(".h\"\n");
            
            #line default
            #line hidden
            
            #line 25 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
 
} 

            
            #line default
            #line hidden
            
            #line 28 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("\nnamespace ");
            
            #line default
            #line hidden
            
            #line 29 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( gump.model.@namespace ));
            
            #line default
            #line hidden
            
            #line 29 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(" {\n  \n  struct ");
            
            #line default
            #line hidden
            
            #line 31 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.name ));
            
            #line default
            #line hidden
            
            #line 31 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(" \n    : public Gump::PropertyT<Gump::e");
            
            #line default
            #line hidden
            
            #line 32 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.Dim() ));
            
            #line default
            #line hidden
            
            #line 32 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(", Arcane::");
            
            #line default
            #line hidden
            
            #line 32 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.Type() ));
            
            #line default
            #line hidden
            
            #line 32 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(">\n  {\n    using Base = Gump::PropertyT<Gump::e");
            
            #line default
            #line hidden
            
            #line 34 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.Dim() ));
            
            #line default
            #line hidden
            
            #line 34 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(", Arcane::");
            
            #line default
            #line hidden
            
            #line 34 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.Type() ));
            
            #line default
            #line hidden
            
            #line 34 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(">;\n\n    ");
            
            #line default
            #line hidden
            
            #line 36 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.name ));
            
            #line default
            #line hidden
            
            #line 36 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("(const Base& e) \n      : Base(e) {} \n  \n    ");
            
            #line default
            #line hidden
            
            #line 39 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.name ));
            
            #line default
            #line hidden
            
            #line 39 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("(const Gump::Property& e) \n      : Base(e) {} \n\n");
            
            #line default
            #line hidden
            
            #line 42 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
 
foreach(var entity in entities) { 

            
            #line default
            #line hidden
            
            #line 45 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("    ");
            
            #line default
            #line hidden
            
            #line 45 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.name ));
            
            #line default
            #line hidden
            
            #line 45 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("(const ");
            
            #line default
            #line hidden
            
            #line 45 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( entity.name ));
            
            #line default
            #line hidden
            
            #line 45 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("& e) \n      : Base(");
            
            #line default
            #line hidden
            
            #line 46 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.Kind () ));
            
            #line default
            #line hidden
            
            #line 46 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(", e) {} \n\n    ");
            
            #line default
            #line hidden
            
            #line 48 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.name ));
            
            #line default
            #line hidden
            
            #line 48 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("(const Gump::EnumeratorT<");
            
            #line default
            #line hidden
            
            #line 48 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( entity.name ));
            
            #line default
            #line hidden
            
            #line 48 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(">& e) \n      : Base(");
            
            #line default
            #line hidden
            
            #line 49 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.Kind () ));
            
            #line default
            #line hidden
            
            #line 49 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(", e) {} \n");
            
            #line default
            #line hidden
            
            #line 50 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
 
} 

            
            #line default
            #line hidden
            
            #line 53 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("    \n  };\n  \n} \n\n#endif /* ");
            
            #line default
            #line hidden
            
            #line 58 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( gump.model.@namespace.ToUpper() ));
            
            #line default
            #line hidden
            
            #line 58 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("_GENERATED_BY_GUMP_");
            
            #line default
            #line hidden
            
            #line 58 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture( property.name.ToUpper() ));
            
            #line default
            #line hidden
            
            #line 58 "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/GumpCompiler/PropertyT4.tt"
            this.Write("_H */\n");
            
            #line default
            #line hidden
            return this.GenerationEnvironment.ToString();
        }
        
        protected virtual void Initialize() {
        }
    }
    
    public class PropertyT4Base {
        
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
