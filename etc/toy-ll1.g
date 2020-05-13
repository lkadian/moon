<START> ::= <prog> 

<aParams> ::= <expr> <aParamsTailRept0> 
<aParams> ::= EPSILON !sem_end_aparams!

<aParamsTail> ::= ',' <expr> 

<aParamsTailRept0> ::= <aParamsTail> <aParamsTailRept0> 
<aParamsTailRept0> ::= EPSILON !sem_end_aparams!

<addOp> ::= '+' !sem_push_+!
<addOp> ::= '-' !sem_push_-!
<addOp> ::= 'or' !sem_push_or!

<arithExpr> ::= <term> <arithExprRightRec>

<arithExprRightRec> ::= EPSILON
<arithExprRightRec> ::= <addOp> <term> !sem_op_addop! <arithExprRightRec> 

<arraySize> ::= '[' <arraySizeFactor> 

<arraySizeFactor> ::= !sem_push_dim! ']' 
<arraySizeFactor> ::= 'intNum' !sem_push_dim! ']' 

<arraySizeRept0> ::= <arraySize> <arraySizeRept0>
<arraySizeRept0> ::= EPSILON !sem_end_dimlist!

<assignOp> ::= '=' !sem_push_=!

<assignStatOrFunctionCall> ::= !sem_start_var! <assignStatOrFunctionCallFactor1>

<assignStatOrFunctionCallFactor1> ::= !sem_start_datamemorfcall! 'id' !sem_push_id! <assignStatOrFunctionCallFactor2> 

<assignStatOrFunctionCallFactor2> ::= !sem_start_indicelist! <indiceRept0> !sem_end_datamember! <continueAssignStatFactor> 
<assignStatOrFunctionCallFactor2> ::= '(' !sem_start_aparams! <aParams> ')' !sem_end_fcall! <continueFunctionCallFactor> 

<classBodyRept0> ::= !sem_start_memVarOrFuncDecl! <visibility>  <memberDecl> <classBodyRept0> 
<classBodyRept0> ::= EPSILON 

<classDecl> ::= !sem_start_class! 'class' 'id' !sem_push_id! !sem_start_inheritlist! <inheritsOpt> !sem_end_inheritlist! '{' !sem_start_memblist! <classBodyRept0> !sem_end_memblist! '}' ';' !sem_end_class!

<classDeclRept0> ::= <classDecl> <classDeclRept0> 
<classDeclRept0> ::= EPSILON 

<continueAssignStatFactor> ::= '.' <assignStatOrFunctionCallFactor1> 
<continueAssignStatFactor> ::= !sem_end_var! <assignOp> <expr> !sem_op_assign!

<continueFunctionCallFactor> ::= '.' <assignStatOrFunctionCallFactor1> 
<continueFunctionCallFactor> ::= !sem_end_var! EPSILON

<continueFunctionCallOrVariableFactor> ::= '.' <functionCallOrVariableFactor1> 
<continueFunctionCallOrVariableFactor> ::= EPSILON

<continueOrEndVariableFactor> ::= '.' <variableFactor1> 
<continueOrEndVariableFactor> ::= EPSILON 

<continueVariableFactor> ::= '.' <variableFactor1> 

<expr> ::= !sem_start_arithOrRel! <arithExpr> <exprArithOrRel>

<exprArithOrRel> ::= <relOp> <arithExpr> !sem_end_relexpr!
<exprArithOrRel> ::= EPSILON !sem_end_arithexpr!

<fParams> ::= !sem_start_fparams! <type> 'id' !sem_push_id! !sem_start_dimlist! <arraySizeRept0> !sem_end_fparams! <fParamsTailRept0> 
<fParams> ::= EPSILON 

<fParamsTail> ::= ',' !sem_start_fparams! <type> 'id' !sem_push_id! !sem_start_dimlist! <arraySizeRept0> !sem_end_fparams!

<fParamsTailRept0> ::= <fParamsTail> <fParamsTailRept0> 
<fParamsTailRept0> ::= EPSILON 

<factor> ::= <functionCallOrVariable> 
<factor> ::= 'intNum' !sem_push_intNum!
<factor> ::= 'floatNum' !sem_push_floatNum!
<factor> ::= '(' <arithExpr> ')' 
<factor> ::= !sem_start_not! 'not' <factor> !sem_end_not!
<factor> ::= !sem_start_sign! <sign> <factor> !sem_end_sign!

<funcBody> ::=  !sem_start_funcbody! <funcLocalOpt> 'do' !sem_start_statlist! <statementRept0> 'end' !sem_end_funcbody!

<funcDef> ::= !sem_start_funcdef! <funcHead> <funcBody> !sem_end_funcdef!

<funcDefRept0> ::= <funcDef> <funcDefRept0> 
<funcDefRept0> ::= EPSILON

<funcHead> ::= 'id' !sem_push_id! !sem_start_scoperes! <scopeResOp> '('  !sem_start_fparamslist! <fParams> !sem_end_fparamslist! ')' ':' <typeOrVoid> 

<funcLocalOpt> ::= 'local' !sem_start_vardecllist! <varDeclRept0> !sem_end_vardecllist!
<funcLocalOpt> ::= EPSILON 

<functionCallOrVariable> ::= !sem_start_var! <functionCallOrVariableFactor1> !sem_end_var!

<functionCallOrVariableFactor1> ::= !sem_start_datamemorfcall! 'id' !sem_push_id! <functionCallOrVariableFactor2> 

<functionCallOrVariableFactor2> ::= !sem_start_indicelist! <indiceRept0> !sem_end_datamember! <continueFunctionCallOrVariableFactor> 
<functionCallOrVariableFactor2> ::= '(' !sem_start_aparams! <aParams> ')' !sem_end_fcall! <continueFunctionCallOrVariableFactor> 

<indice> ::= '[' <arithExpr> ']' 

<indiceRept0> ::= <indice> <indiceRept0> 
<indiceRept0> ::= EPSILON !sem_end_indicelist!

<inheritsOpt> ::= 'inherits' 'id' !sem_push_id! <inheritsRept0> 
<inheritsOpt> ::= EPSILON 

<inheritsRept0> ::= ',' 'id' !sem_push_id! <inheritsRept0> 
<inheritsRept0> ::= EPSILON 

<memberDecl> ::= 'id' <memberFuncDeclOrVarDeclFactor> 
<memberDecl> ::= 'integer' !sem_push_type! <memberVarDeclFactor> 
<memberDecl> ::= 'float' !sem_push_type! <memberVarDeclFactor> 

<memberFuncDeclOrVarDeclFactor> ::= !sem_push_id! '(' !sem_start_fparamslist! <fParams> !sem_end_fparamslist! ')' ':' <typeOrVoid> ';' !sem_end_memberfuncdecl!
<memberFuncDeclOrVarDeclFactor> ::= !sem_push_type! <memberVarDeclFactor> 

<memberVarDeclFactor> ::= 'id' !sem_push_id! !sem_start_dimlist! <arraySizeRept0> ';' !sem_end_membervardecl!

<multOp> ::= '*' !sem_push_*!
<multOp> ::= '/' !sem_push_/!
<multOp> ::= 'and' !sem_push_and!

<prog> ::= !sem_start_prog! !sem_start_classlist! <classDeclRept0> !sem_end_classlist! !sem_start_funcdeflist! <funcDefRept0> !sem_end_funcdeflist! !sem_start_main! 'main' <funcBody> !sem_end_main! !sem_end_prog!

<relExpr> ::= !sem_start_relexpr! <arithExpr> <relOp> <arithExpr> !sem_end_relexpr!

<relOp> ::= 'eq' !sem_push_eq! 
<relOp> ::= 'neq' !sem_push_neq! 
<relOp> ::= 'lt' !sem_push_lt! 
<relOp> ::= 'gt' !sem_push_gt! 
<relOp> ::= 'leq' !sem_push_leq! 
<relOp> ::= 'geq' !sem_push_geq! 

<scopeResOp> ::= 'sr' 'id' !sem_push_id! !sem_end_scoperes!
<scopeResOp> ::= EPSILON !sem_end_scoperes!

<sign> ::= '+' !sem_push_sign!
<sign> ::= '-' !sem_push_sign!

<statBlock> ::= 'do' !sem_start_statlist! <statementRept0> 'end' 
<statBlock> ::= !sem_start_statlist! <statement> !sem_end_statlist!
<statBlock> ::= !sem_start_statlist! EPSILON !sem_end_statlist!

<statement> ::= !sem_start_ifstat! 'if' '(' <relExpr> ')' 'then' <statBlock> 'else' <statBlock> ';' !sem_end_ifstat!
<statement> ::= !sem_start_while! 'while' '(' <relExpr> ')' <statBlock> ';' !sem_end_while!
<statement> ::= !sem_start_read! 'read' '(' <variable> ')' ';' !sem_end_read!
<statement> ::= !sem_start_write! 'write' '(' <expr> ')' ';' !sem_end_write!
<statement> ::= !sem_start_return! 'return' '(' <expr> ')' ';' !sem_end_return!
<statement> ::= <assignStatOrFunctionCall> ';'

<statementRept0> ::= <statement> <statementRept0> 
<statementRept0> ::= EPSILON !sem_end_statlist!

<term> ::= <factor> <termRightRec>

<termRightRec> ::= EPSILON
<termRightRec> ::= <multOp> <factor> !sem_op_multop! <termRightRec> 

<type> ::= 'integer' !sem_push_type!
<type> ::= 'float' !sem_push_type!
<type> ::= 'id' !sem_push_type!

<typeOrVoid> ::= <type> 
<typeOrVoid> ::= 'void' !sem_push_type!

<varDecl> ::= !sem_start_vardecl! <type> 'id' !sem_push! !sem_start_dimlist! <arraySizeRept0> ';' !sem_end_vardecl!

<varDeclRept0> ::= <varDecl> <varDeclRept0> 
<varDeclRept0> ::= EPSILON 

<variable> ::= !sem_start_var! <variableFactor1> 

<variableFactor1> ::= !sem_start_datamemorfcall! 'id' !sem_push_id! <variableFactor2> 

<variableFactor2> ::= !sem_start_indicelist! <indiceRept0> !sem_end_datamember! <continueOrEndVariableFactor> !sem_end_var!
<variableFactor2> ::= '(' !sem_start_aparams! <aParams> ')' !sem_end_fcall! <continueVariableFactor>

<visibility> ::= 'public' !sem_push!
<visibility> ::= 'private' !sem_push!


