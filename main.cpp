# include <iostream> // cout, endl
# include <string.h> // string
# include <vector> // vector
# include <cstdlib> // atoi, system
# include <iomanip> // setw, setprecision
# include <stdio.h>
# include <stdlib.h>
# include <string>
# include <sstream>
# include <math.h>
# include <queue>
# include <cmath>

using namespace std;
enum Type{ IDENT = 1, CONSTANT = 2, INT = 3, FLOAT = 4, CHAR = 5, BOOL = 6, STRING = 7, VOID = 8,
      IF = 9, ELSE = 10, WHILE = 11, DO = 12, RETURN = 13, GE = 14, LE = 15, EQ = 16, NEQ = 17,
      AND = 18, OR = 19, PE = 20, ME = 21, TE = 22, DE = 23, RE = 24, PP = 25, MM = 26, RS = 27,
      LS = 28, SYMBOL = 29, DONE = 30// Done先不判斷是否為UndefinedToken，要判斷後面有無() 
};

enum Funct_id_type{   // 被定義的是 ID 還是 Function 
  FUNCTIONTYPE = 1, IDENTTYPE = 2 
};

class Token {                      // 讀取的Token
public :
  string mName ;
  Type mType ;
  int mLine ;
  int mColumn  ;
  string mDetail ; // 全域變數被定義時的內容，除了全域變數其他情況沒用到 
};

class Funct_or_id{ // 暫存BeDefined型別 
  public :
  Funct_id_type mType ;   // ID or Function Type 
  vector <Token> mFunct_Local_Id ; // Function裡的ID
  vector <Token> mGlobal_Id ; // 全域變數 
};

class StoreFunctionType{ // 儲存HadDefined的Function型別 
  public :
  string mFunctionName ; // Function裡的ID
  vector <string> mLocal_Id ; // 區域變數
  string mDetail ; // Function裡的內容 
};

static int uLineNum = 0, uColumnNum = 0 ; // 紀錄Token的位址
static string uDetail ; // 存所有讀的東西
static vector<Token> uBeJudgeTokenTable; // 存放核對過文法的Token
static vector<Token> uErrorLeftTokenTable; // 因Error存留下來的Token

static Funct_or_id uNeedDefinedObject ; // 未確認語法正確前，要定義的Object 
static vector<StoreFunctionType> uHadDefined_Function_Table; // 存被定義的Function，方便印出
static vector<Token> uHadDefined_Id_Table; // 存被定義的ID，方便印出

static string uErrorToken ; // 紀錄錯誤的Token
static bool uDontcheckDefined  ; // 避開defined判斷
static Token uNowPeekToken ; // PeekToken後用來存放Token的Buffer，若沒有Get則不會Peek新的Token
static Token uNowGetToken ; // GetToken後存放Token的Buffer
static bool usetline ; // 是否還未設定 
static int ulastline ; // 上次結束後的line數 
// -----------工具------------
Token PeekToken() ;
void KeepTrack( char input ) ;
bool GetNextChar( char &next ) ;
Token GetToken() ;
void SetNeedDefinedObject( Token token, Funct_id_type type, bool Local ) ; // 暫存需要被定義的Object
void StoreTrueDefine() ; 
bool UnrecognizedToken( char word ) ;
bool UnexpectedToken( Type &tokentype, string tokenValue, bool NumorDot, string &errorToken ) ;
bool UndefinedIdentifier( string Name ) ;
string Reform() ;
void CleanErrorLine() ;
// -----------工具------------

// -----------文法------------
bool User_input() ;
bool Definition() ;
bool Type_specifier( Token token ) ;
bool Function_definition_or_declarators( Token firsttoken ) ;
bool Rest_of_declarators() ;
bool Function_definition_without_id( Token firsttoken ) ;
bool Formal_parameter_list() ;
bool Compound_statement() ;
bool Declaration() ;
bool Statement() ;
bool Expression() ;
bool Basic_expression() ;
bool Rest_of_identifier_started_basic_exp() ;
bool Rest_of_PPMM_identifier_started_basic_exp() ;
bool Sign() ;
bool Actual_parameter_list() ;
bool Assignment_operator() ;
bool Romce_and_romloe() ;
bool Rest_of_maybe_logical_and_exp() ;
bool Maybe_logical_and_exp() ;
bool Rest_of_maybe_logical_and_exp() ;
bool Maybe_bit_or_exp() ;
bool Rest_of_maybe_bit_or_exp() ;
bool Maybe_bit_ex_or_exp() ;
bool Rest_of_maybe_bit_ex_or_exp() ;
bool Maybe_bit_and_exp() ;
bool Rest_of_maybe_bit_and_exp() ;
bool Maybe_equality_exp() ;
bool Rest_of_maybe_equality_exp() ;
bool Maybe_relational_exp() ;
bool Rest_of_maybe_relational_exp() ;
bool Maybe_shift_exp() ;
bool Rest_of_maybe_shift_exp() ;
bool Maybe_additive_exp() ;
bool Rest_of_maybe_additive_exp() ;
bool Maybe_mult_exp() ;
bool Rest_of_maybe_mult_exp() ;
bool Unary_exp() ;
bool Signed_unary_exp() ;
bool Unsigned_unary_exp() ;
bool Rest_of_maybe_logical_or_exp() ; 
// -----------文法------------


// ------------------------------------------Syntax Test------------------------------------------

// ----------------------GetToken-------------------------
string Reform() {
  // 規則: Function"("前面不空白，每個Token隔1空白，每條指令要換行且前面空兩格
  //       最後面"}"前面沒空白，之後沒換行 
  string recontent = "" ;
  recontent = uBeJudgeTokenTable.at( 0 ).mName ;
  int spacesize = -1 ; // 空白大小會隨著多一層("{")+2，隨少一層("}")-2
  for ( int i = 1 ; i < uBeJudgeTokenTable.size() ; i++ ) {

    if ( uBeJudgeTokenTable.at( i ).mName == "}" || uBeJudgeTokenTable.at( i ).mName == "[" 
         || ( uBeJudgeTokenTable.at( i ).mName == "(" && uBeJudgeTokenTable.at( i - 1 ).mType == IDENT )
         || ( uBeJudgeTokenTable.at( i ).mName == ")" && uBeJudgeTokenTable.at( i - 1 ).mName == "(" )
         || ( uBeJudgeTokenTable.at( i ).mName == "++" && uBeJudgeTokenTable.at( i - 1 ).mType == IDENT )
         || ( uBeJudgeTokenTable.at( i ).mName == "--" && uBeJudgeTokenTable.at( i - 1 ).mType == IDENT )
         || ( uBeJudgeTokenTable.at( i ).mType == IDENT && uBeJudgeTokenTable.at( i - 1 ).mName == "++" )
         || ( uBeJudgeTokenTable.at( i ).mType == IDENT && uBeJudgeTokenTable.at( i - 1 ).mName == "--" )
         || uBeJudgeTokenTable.at( i ).mName == "," ) {
      // ex: test() 、 a[10] 、 int a, b 、 a++ 、 --a 
      recontent = recontent + uBeJudgeTokenTable.at( i ).mName ; // 不放空格 
    } // if
    else {
      // 空格間距是1，每行第一個Token會有空白!!!!! 
      recontent = recontent + " " + uBeJudgeTokenTable.at( i ).mName ; 
    } // else 

    // --------------------以上為Token之間的空白-------------------------- 
    if ( uBeJudgeTokenTable.at( i ).mName == "{" ) spacesize = spacesize + 2 ;
    else if  ( uBeJudgeTokenTable.at( i ).mName == "}" ) spacesize = spacesize - 2 ;
    // 多一層、少一層紀錄空白大小
    string space = "" ;
    for ( int j = 0 ; j < spacesize ; j++ ) {
      // 計算每行第一個Token前面空白的大小，記得要包含1個間距空白
      space = space + " " ;
    } // for

    if ( uBeJudgeTokenTable.at( i ).mName == "{" || uBeJudgeTokenTable.at( i ).mName == ";" 
         || uBeJudgeTokenTable.at( i ).mName == "}" ) {
      // 換行、Token前面的空白
      if ( uBeJudgeTokenTable.at( i ).mName == "}" && spacesize == -1 ) { 
      // 最後一個"}"下行不用換行
      } // if
      else if ( ( i + 2 ) <= uBeJudgeTokenTable.size() && uBeJudgeTokenTable.at( i + 1 ).mName == "}" ) { 
      // 下一個是"}"，下一行前面會少兩格
        string space2 = "" ;
        for ( int k = 0 ; k < spacesize - 1 ; k++ ) {
          space2 = space2 + " " ;
        } // for

        recontent = recontent + "\n" + space2 ;
      } // else if
      else {
        recontent = recontent + "\n" + space ;
      } // else

    } // if

  } // for

  return recontent ;
} // Reform()

bool UnrecognizedToken( char word ) {

  // 判斷有無Unrecognized Token
  if ( ( word >= 'A' && word <= 'Z' ) || ( word >= 'a' && word <= 'z' ) || ( word >= '0' && word <= '9' )
       || word == '(' || word == ')' || word == '[' || word == ']' || word == '{' || word == '}'
       || word == '+' || word == '-' || word == '*' || word == '/' || word == '%' || word == '^'
       || word == '>' || word == '<' || word == '=' || word == '!' || word == '&' || word == '|'
       || word == ';' || word == ',' || word == '?' || word == ':' || word == '"' || word == '.'
       || word == ' ' || word == '\t' || word == '\n' ) { // || word == '\'' || word == '"' || word == '.'

    return true ;
  } // if
  else {
    CleanErrorLine() ;
    cout << "> Line " << uLineNum << " : unrecognized token with first char : '" << word << "'" << endl ;
    uErrorToken = word ;
    string error = "UnrecognizedToken" ;
    throw error ;
    return false;
  } // else

} // UnrecognizedToken()

bool UnexpectedToken( Type &tokentype, string tokenValue ) {

  // ----------------------Identifier-------------------
  // 將特別的IDENT 重定義型別
  if ( tokentype == IDENT ) {
    if ( tokenValue == "true" || tokenValue == "false" ) {
      tokentype = CONSTANT ;
    } // if
    else if ( tokenValue == "int" ) {
      tokentype = INT ;
    } // else if
    else if ( tokenValue == "float" ) {
      tokentype = FLOAT ;
    } // else if
    else if ( tokenValue == "char" ) {
      tokentype = CHAR ;
    } // else if
    else if ( tokenValue == "bool" ) {
      tokentype = BOOL ;
    } // else if
    else if ( tokenValue == "string" ) {
      tokentype = STRING ;
    } // else if
    else if ( tokenValue == "void" ) {
      tokentype = VOID ;
    } // else if
    else if ( tokenValue == "if" ) {
      tokentype = IF ;
    } // else if
    else if ( tokenValue == "else" ) {
      tokentype = ELSE ;
    } // else if
    else if ( tokenValue == "while" ) {
      tokentype = WHILE ;
    } // else if
    else if ( tokenValue == "do" ) {
      tokentype = DO ;
    } // else if
    else if ( tokenValue == "return" ) {
      tokentype = RETURN ;
    } // else if

  } // if
  // ----------------------Constant-------------------
  if ( tokentype == CONSTANT ) {
  // SIGN 和 NUM 可以是整數或Float
    int num = 0;
    // 計算 '.'數量
    for ( int i = 0; i < tokenValue.size() ; i++ ) {
      if ( tokenValue.at( i ) == '.' )  num++ ;
    } // for

    if ( num > 1 ) {

      // 這組字串(NUM、SIGN)超過一個逗號，ERROR!!
      int dotnum = 0;     // 計算'.'、數量
      string numAfter2Dot = "";
      // 紀錄第二個逗號之後的數字，用來報錯
      for ( int i = 0 ; i < tokenValue.length() ; i++ ) {
        if ( tokenValue.at( i ) == '.' ) dotnum++;
        if ( dotnum == 2 ) {
          numAfter2Dot = numAfter2Dot + tokenValue.at( i );
        } // if
      } // for

      uErrorToken = numAfter2Dot;      // 第二個'.'之後的字串傳上去
      string error = "UnexpectedToken" ;
      throw error ;
      return false;
    } // if
  } // if

  return true;
} // UnexpectedToken()

bool UndefinedIdentifier( string Name ) {
  // 未定義Debug，有分全域變數、Function、區域變數
  if ( uNeedDefinedObject.mType == FUNCTIONTYPE ) {
  // Function裡面的statement，需判斷他有沒有被定義在     "區域變數、全域變數、Function"
    for ( int i = 0; i < uNeedDefinedObject.mFunct_Local_Id.size() ; i++ ) {
      if ( Name == uNeedDefinedObject.mFunct_Local_Id.at( i ).mName ) {
        // 區域變數 
        return true;
      } // if
    } // for

    for ( int i = 0; i < uHadDefined_Id_Table.size() ; i++ ) {
      if ( Name == uHadDefined_Id_Table.at( i ).mName ) {
        // 全域變數 
        return true;
      } // if
    } // for

    for ( int i = 0; i < uHadDefined_Function_Table.size() ; i++ ) {
      if ( Name == uHadDefined_Function_Table.at( i ).mFunctionName ) {
        // 其他Function
        return true;
      } // if
    } // for 
  } // if
  else if ( uNeedDefinedObject.mType == IDENTTYPE ) {
  // main裡面的Statement，需判斷他有沒有被定義在全域變數
    for ( int i = 0; i < uHadDefined_Id_Table.size() ; i++ ) {
      if ( Name == uHadDefined_Id_Table.at( i ).mName ) {
        // 全域變數 
        return true;
      } // if
    } // for

    for ( int i = 0; i < uHadDefined_Function_Table.size() ; i++ ) {
      if ( Name == uHadDefined_Function_Table.at( i ).mFunctionName ) {
        // 其他Function
        return true;
      } // if
    } // for

  } // else if 

  CleanErrorLine() ;
  cout << "> Line " << uLineNum << " : undefined identifier : '" << Name << "\'" << endl;
  uErrorToken = Name ;
  string error = "UndefinedIdentifier" ;
  throw error ;
  return false;
} // UndefinedIdentifier()

void KeepTrack( char input ) {      // 紀錄Get到的char位址

  if ( input == '\n' ) {      // 換行
    uColumnNum = 0;
    uLineNum++;
  } // if
  else {
    uColumnNum++ ;
  } // else

  // 這邊是為了怕沒有獨到換行而uLineNum還是0的狀態
  // ex: a + b ; & 
  // Error:& ，可在那之前沒換行，只能自行幫
  // if ( uLineNum == 0 ) uLineNum = 1 ; 

} // KeepTrack()

void CleanErrorLine() {

  // 將出現error的那行刪除
  // Error token以前的行數全清除
  /*
  for ( int i = 0 ; i < uBeJudgeTokenTable.size() ; i++ ) {
    if ( uBeJudgeTokenTable.at(i).mLine > uLineNum ) {
    // 大於ErrorToken的行數存起來留到下次
      uErrorLeftTokenTable.push_back( uBeJudgeTokenTable.at( i ) ) ;
    } // if
  } // for
  */

  char clean = '\0' ;
  clean = cin.peek() ;
  while ( clean != '\n' && clean != EOF ) {
  // 不把換行讀掉，Ulinenum靠Line增加
    scanf( "%c", &clean ) ;
    clean = cin.peek() ;
  } // while

} // CleanErrorLine()

char SkipWhiteSpaceCommentChar() {
  // 跳過WhiteSpace + 註解 
  char nextchar = cin.peek();
  while ( nextchar == ' ' || nextchar == '\t' || nextchar == '\n' ) {
    scanf( "%c", &nextchar );
    KeepTrack( nextchar ) ;
    nextchar = cin.peek();
  } // while

  KeepTrack( nextchar ) ; 
  char next = cin.peek() ;
  if ( next == '/' ) {
    // 1.若不是Comment，只是'/'則讀入輸出。
    // 2.是Comment，則清除註解
    next = getchar() ;   // 讀入
    KeepTrack( next ) ;                   // 紀錄Get到的字元位置
    char next2 = cin.peek();
    if ( next2 == '/' ) {  // 遇到comment
      next = getchar() ;
      next = cin.peek() ;
      while ( next != '\n' ) {  // 將comment讀完，直到換行
        scanf( "%c", &next );
        next = cin.peek() ;
      } // while

      // KeepTrack( next );        // while出來為'\n'，重新計算Line
      // 遞迴!!，避免多行註解，呼叫自身
      next = SkipWhiteSpaceCommentChar();
    } // if

    return next ;
  } // if
    
  next = getchar() ;   // 讀入
  KeepTrack( next ) ;                   // 紀錄Get到的字元位置
  // cout << ulastline << "  " << uLineNum << endl ; 
  if ( usetline == true && ulastline != 0 ) {
  // 在每次指令的第一個Token設定LineNum
  // 發生Error or 新指令就會計算 
  // 1.上次最後的Line數與現在的Line數一樣，代表同一行有兩個指令，line設為1 
  // 2.不一樣則現在linenum-上次linenum相減
    if ( ulastline == uLineNum ) uLineNum = 1 ; 
    else uLineNum = uLineNum - ulastline ;
    usetline = false ;
  } // if
  // 計算uLineNum
  UnrecognizedToken( next ) ; // 判斷有無Error 
  return  next ;

} // SkipWhiteSpaceCommentChar()

bool GetNextChar( char &next ) {
  // 得到一個char且判斷是否合法
  next = '\0' ;
  scanf( "%c", &next );
  KeepTrack( next );
  if ( next == '/' ) {
    // 1.若不是Comment，只是'/'則讀入輸出。
    // 2.是Comment，則清除註解
    char next2 = cin.peek();
    if ( next2 == '/' ) {  // 遇到comment
      KeepTrack( next );        // 紀錄Get到的字元位置
      scanf( "%c", &next );
      next = cin.peek() ;
      while ( next != '\n' ) {  // 將comment讀完，直到換行
        scanf( "%c", &next );
        next = cin.peek() ;
      } // while

      // KeepTrack( next );    // while出來為'\n'，重新計算Line
      // 遞迴!!，避免多行註解，呼叫自身
      GetNextChar( next );
    } // if
  } // if

  if ( !UnrecognizedToken( next ) ) {
   // 判斷是否UNrecognized
    return '\0' ;
  } // if

  return  next ;
} // GetNextChar()

Token PeekToken() { // 將Token放到NowPeekToken裡

  if ( uNowPeekToken.mName != "" ) {
    // 避免一開始還沒讀東西，而ERROR，加上uNowPeekToken.mName != "" 等條件
    if ( uNowPeekToken.mName != uNowGetToken.mName
         || uNowPeekToken.mColumn != uNowGetToken.mColumn
         || uNowPeekToken.mLine != uNowGetToken.mLine ) {
      // 如果沒有GetToken，則不Peek新Token
      return uNowPeekToken ;
    } // if 
  } // if


  if ( uErrorLeftTokenTable.size() > 0 ) {
    // 把上次的Token取出
    // cout << ulastline << "  " << uLineNum << endl ; 
    if ( usetline == true && ulastline != 0 ) {
    // 在每次指令的第一個Token設定LineNum
    // 發生Error or 新指令就會計算 
    // 1.上次最後的Line數與現在的Line數一樣，代表同一行有兩個指令，line設為1 
    // 2.不一樣則現在linenum-上次linenum相減
      if ( ulastline == uLineNum ) uLineNum = 1 ; 
      else uLineNum = uLineNum - ulastline ;
      usetline = false ;
    } // if
    // 計算uLineNum  
    Token token = uErrorLeftTokenTable.front() ;
    uErrorLeftTokenTable.erase( uErrorLeftTokenTable.begin() ) ;
    token.mColumn = uColumnNum ;
    token.mLine = uLineNum ;
    uNowPeekToken = token ; // 把Token存到Buffer中
    return token ;
  } // if
  
  // ---------以上為確認uBeJudgeTokenTable是否還有東西-----------
  // 實際上為GetToken
  // 會主動跳過WhiteSpace
  // 將切出來的Token放到暫存的Buffer裡
  char next = '\0' ;
  char input = SkipWhiteSpaceCommentChar() ;
  // -----------------跳過空白、Comment------------------ 
  Token token ;  // Token，ID型別

  if ( input == '(' || input == ')' || input == '[' || input == ']' || input == '{' || input == '}'
       || input == '+' || input == '-' || input == '*' || input == '/' || input == '%' || input == '^'
       || input == '>' || input == '<' || input == '=' || input == '!' || input == '&' || input == '|'
       || input == ';' || input == ',' || input == '?' || input == ':' ) {
    // ----------符號型別(SYMBOL)--------------
    token.mType = SYMBOL ;  // 若是特定符號型別，需再做設定

    if ( input == '<' ) {
      next = cin.peek() ;
      if ( next == '=' ) {       // '<='、LE
        GetNextChar( next ) ;
        token.mName = "<=" ;
        token.mType = LE ;
      } // if
      else if ( next == '<' ) {  // '<<'、LS
        GetNextChar( next ) ;
        token.mName = "<<" ;
        token.mType = LS ;
      } // else if
      else  {                    // '<'
        token.mName = "<" ;
      } // else
    } // if
    else if ( input == '>' ) {
      next = cin.peek() ;
      if ( next == '=' ) {       // '>='、GE
        GetNextChar( next ) ;
        token.mName = ">=" ;
        token.mType = GE ;
      } // if
      else if ( next == '>' ) {  // '>>'、RS
        GetNextChar( next ) ;
        token.mName = ">>" ;
        token.mType = RS ;
      } // else if 
      else {                     // '>'
        token.mName = ">" ;
      } // else
    } // else if
    else if ( input == '=' ) {
      next = cin.peek() ;
      if ( next == '=' ) {        // '=='、EQ
        GetNextChar( next ) ;
        token.mName = "==" ;
        token.mType = EQ ;
      } // if
      else {
        token.mName = "=" ;      // '='
      } // else
    } // else if
    else if ( input == '!' ) {
      next = cin.peek() ;
      if ( next == '=' ) {         // '!='、NEQ
        GetNextChar( next ) ;
        token.mName = "!=" ;
        token.mType = NEQ ;
      } // if
      else {
        token.mName = "!" ;      // '!'
      } // else
    } // else if
    else if ( input == '&' ) {
      next = cin.peek() ;
      if ( next == '&' ) {      // '&&'、AND
        GetNextChar( next ) ;
        token.mName = "&&" ;
        token.mType = AND ;
      } // if
      else {
        token.mName = "&" ;     // '&'
      } // else
    } // else if
    else if ( input == '|' ) {
      next = cin.peek() ;
      if ( next == '|' ) {      // '||'、OR
        GetNextChar( next ) ;
        token.mName = "||" ;
        token.mType = OR ;
      } // if
      else {
        token.mName = "|" ;     // '|'
      } // else
    } // else if
    else if ( input == '+' ) {
      next = cin.peek() ;
      if ( next == '=' ) {       // '+='、PE
        GetNextChar( next ) ;
        token.mName = "+=" ;
        token.mType = PE ;
      } // if
      else if ( next == '+' ) {   // '++'、PP
        GetNextChar( next ) ;
        token.mName = "++" ;
        token.mType = PP ;
      } // else if
      else {
        token.mName = "+" ;     // '+'
      } // else
    } // else if
    else if ( input == '-' ) {
      next = cin.peek() ;
      if ( next == '=' ) {      // '-='、ME
        GetNextChar( next ) ;
        token.mName = "-=" ;
        token.mType = ME ;
      } // if
      else if ( next == '-' ) {   // '--'、MM
        GetNextChar( next ) ;
        token.mName = "--" ;
        token.mType = MM ;
      } // else if
      else {
        token.mName = "-" ;     // '-'
      } // else
    } // else if
    else if ( input == '*' ) {
      next = cin.peek() ;
      if ( next == '=' ) {      // '*='、TE
        GetNextChar( next ) ;
        token.mName = "*=" ;
        token.mType = TE ;
      } // if
      else {
        token.mName = "*" ;     // '*'
      } // else
    } // else if
    else if ( input == '/' ) {
      next = cin.peek() ;
      if ( next == '=' ) {      // '/='、DE
        GetNextChar( next ) ;
        token.mName = "/=" ;
        token.mType = DE ;
      } // if
      else {
        token.mName = "/" ;     // '/'
      } // else
    } // else if
    else if ( input == '%' ) {
      next = cin.peek() ;
      if ( next == '=' ) {      // '%='、RE
        GetNextChar( next ) ;
        token.mName = "%=" ;
        token.mType = RE ;
      } // if
      else {
        token.mName = "%" ;     // '%'
      } // else
    } // else if
    else {               // 其他符號
      token.mName = input ;
    } // else

  } // if
  else if ( ( input >= '0' && input <= '9' ) || input == '.' || input == '\'' || input == '"' ) {
    token.mType = CONSTANT ;
    // -----------------Constant---------------------
    token.mName = input ; // 存入

    if ( input == '.' ) {
    // EX: .aa、.123、.1.2，點開頭
      input = cin.peek() ;
      if ( input < '0' || input > '9' ) {
        // 小數點後面不是數字，ERROR
        UnrecognizedToken( '.' ) ;
      } // if
      else {
        while ( input >= '0' && input <= '9' ) {
        // 碰到非數字跳出
          scanf( "%c", &input ) ;
          KeepTrack( input ) ;
          token.mName = token.mName + input ;
          input = cin.peek() ; // 查看下個是否為數字
        } // while
      } // else
    } // if
    else if ( input == '\'' || input == '"' ) {
    // EX: 'a'、"Hi, there"      '、"開頭
      if ( input == '\'' ) {
        scanf( "%c", &input ) ;
        if ( input != '\'' ) {
          token.mName = token.mName + input ;  // 'a'
          scanf( "%c", &input ) ;
          KeepTrack( input ) ;
          if ( input != '\'' ) {
          // '後面第二個不是'，ERROR!
            UnrecognizedToken( '\'' ) ;
          } // if
        } // if
        else {
        // ''、沒有東西、ERROR
          // 留給Unexpected判斷
        } // else
      } // if
      else {
        input = cin.peek() ;
        while ( input != '"'  ) {     // 碰到"跳出
          scanf( "%c", &input ) ;
          KeepTrack( input ) ;
          token.mName = token.mName + input ;
          input = cin.peek() ;
        } // while

        scanf( "%c", &input ) ;
        KeepTrack( input ) ;
        token.mName = token.mName + input ;
        token.mType = IDENT ; // "abc" ，是IDENT Token 
      } // else
    } // else if 
    else {
    // EX: 12.1、123，數字開頭
      input = cin.peek() ;
      while ( ( input >= '0' && input <= '9' ) ||  input == '.'  ) {
        // 碰到非數字、dot跳出
        // Dot數量問題留到Unexpected判斷
        scanf( "%c", &input ) ;
        KeepTrack( input ) ;
        token.mName = token.mName + input;
        input = cin.peek() ;     // 先判斷下一個Char是否正確在讀
      } // while
    } // else
  } // else if 
  else if ( ( input >= 'A' && input <= 'Z' ) || ( input >= 'a' && input <= 'z' ) ) {
    token.mType = IDENT ;
    // -----------------------IDENT-------------------------
    // 英文字母開頭 EX: ab123
    token.mName = input ; // 存入
    input = cin.peek() ;
    while ( ( input >= 'A' && input <= 'Z' ) || ( input >= 'a' && input <= 'z' )
            || ( input >= '0' && input <= '9' ) || input == '_' ) {
      // 讀到不是英文、數字、底線停止!!
      input = getchar() ;
      KeepTrack( input ) ;
      token.mName = token.mName + input;
      input = cin.peek() ;  // 先判斷下一個Char是否正確在讀
    } // while
  } // else if
  else {
  // Unexpect Error 針對 錯誤的Token
  // 將錯誤的Token讀出來
    token.mName = input ;
    while ( input != ' ' || input != '\t' || input != '\n' || input != EOF ) {
      token.mName = token.mName + next ;
      input = getchar() ;
    } // while
    // Error且recognized的Token，UnexpectedToken
    uErrorToken = token.mName ;
    string error = "UnexpectedToken" ;
    throw error ;
  } // else

  // ------------------以下為判斷Unexpected 和 Undefined------------------
  if ( !UnexpectedToken( token.mType, token.mName ) ) {
    // Token有違規的文法
  } // if

  token.mColumn = uColumnNum ;
  token.mLine = uLineNum ;
  uNowPeekToken = token ; // 把Token存到Buffer中
  return token ;

} // PeekToken()

Token GetToken() {

  uNowGetToken = uNowPeekToken ; // Peek的Token放到NowGetToken裡
  int position = uNowGetToken.mName.find_first_of( "\"" ) ; 
  if ( uNowGetToken.mType == IDENT && uDontcheckDefined == false 
       && uNowGetToken.mName != "cout" && uNowGetToken.mName != "cin" && uNowGetToken.mName != "Done"
       && uNowGetToken.mName != "ListAllVariables" && uNowGetToken.mName != "ListAllFunctions"
       && uNowGetToken.mName != "ListVariable" && uNowGetToken.mName != "ListFunction"
       && position < 0 ) {
    // "abccd"，字串不需要被判斷 
    // 不是被定義的，判斷有無Defined過
    UndefinedIdentifier( uNowGetToken.mName )  ;
  } // if

  uBeJudgeTokenTable.push_back( uNowGetToken ) ; // 把Get到的Token記錄起來
  return uNowGetToken ;
} // GetToken()

void SetNeedDefinedObject( Token token, Funct_id_type type, bool Local ) {
// 將可能會被定義的東西先暫存起來 
// 如果是Local == true 代表SetNeedDefinedObject是Function，除了第一個token其他都為區域變數
  if ( type == FUNCTIONTYPE && !Local ) {
  // Function
    uNeedDefinedObject.mFunct_Local_Id.push_back( token ) ;
  } // if
  else if ( type == IDENTTYPE && Local ) {
  // Function 的 區域變數
    uNeedDefinedObject.mFunct_Local_Id.push_back( token ) ;
  } // else if
  else if ( type == IDENTTYPE && !Local ) {
  // 全域變數
    uNeedDefinedObject.mGlobal_Id.push_back( token ) ;
  } // else if  
  else cout << "Store error" << endl ;

} // SetNeedDefinedObject()

void StoreTrueDefine() {
// 文法正確，把要被定義的東西放到Table中
  bool exist = false ; // 紀錄是否重定義
  int address = 0 ; // 記錄被定義的位置
  if ( uNeedDefinedObject.mType == FUNCTIONTYPE ) {   
  // Function、區域變數
    uDetail = Reform() ; // 將Function內容排好，再放到mDetail 
    for ( int i = 0 ; i < uHadDefined_Function_Table.size() ; i++  ) {
      string a = uNeedDefinedObject.mFunct_Local_Id.at( 0 ).mName ;
      if ( a == uHadDefined_Function_Table.at( i ).mFunctionName ) {
        address = i ; // 紀錄已存在Function的位置 
        exist = true ;
      } // if 
    } // for
    // -------判斷Function是否已被定義過，有則重定義----------
    if ( exist ) {
    // 被定義過Function 
      string a = "> New definition of " + uNeedDefinedObject.mFunct_Local_Id.at( 0 ).mName ;
      cout << a + "() entered ..." << endl ;
      a = uNeedDefinedObject.mFunct_Local_Id.at( 0 ).mName ;
      uHadDefined_Function_Table.at( address ).mDetail = uDetail ; // 存內容
      uHadDefined_Function_Table.at( address ).mLocal_Id.clear() ; // 將區域變數的數量清除
      for ( int i = 1 ; i < uNeedDefinedObject.mFunct_Local_Id.size() ; i++ ) {
      // 存區域變數
        a = uNeedDefinedObject.mFunct_Local_Id.at( i ).mName ;
        uHadDefined_Function_Table.at( address ).mLocal_Id.push_back( a ) ;
      } // for
      // ---------------區域變數排序-----------------------
      for ( int i = 0 ; i < uHadDefined_Function_Table.at( address ).mLocal_Id.size() ; i++ ) {
        for ( int j = i  ; j < uHadDefined_Function_Table.at( address ).mLocal_Id.size() ; j++ ) {
          if ( strcmp( uHadDefined_Function_Table.at( address ).mLocal_Id.at( i ).c_str(), 
                       uHadDefined_Function_Table.at( address ).mLocal_Id.at( j ).c_str() ) > 0 ) {
            string a = uHadDefined_Function_Table.at( address ).mLocal_Id.at( j ) ;
            string cover = uHadDefined_Function_Table.at( address ).mLocal_Id.at( i ) ;
            uHadDefined_Function_Table.at( address ).mLocal_Id.at( j ) = cover ;
            uHadDefined_Function_Table.at( address ).mLocal_Id.at( i ) = a ;
          } // if 
        } // for
      } // for
      // ---------------區域變數排序--------------------
    } // if
    else {
      // 沒有被定義過
      StoreFunctionType storeFu ;
      storeFu.mFunctionName = uNeedDefinedObject.mFunct_Local_Id.at( 0 ).mName ; // 存Function名字
      storeFu.mDetail = uDetail ; // 存內容 
      cout << "> Definition of " + storeFu.mFunctionName + "() entered ..." << endl ;
      for ( int i = 1 ; i < uNeedDefinedObject.mFunct_Local_Id.size() ; i++ ) {
      // 存區域變數，第一個是Function，後面才是區域變數 
        storeFu.mLocal_Id.push_back( uNeedDefinedObject.mFunct_Local_Id.at( i ).mName ) ;
      } // for
      // ---------------區域變數排序-----------------------
      for ( int i = 0 ; i < storeFu.mLocal_Id.size() ; i++ ) {
        for ( int j = i  ; j < storeFu.mLocal_Id.size() ; j++ ) {
          if ( strcmp( storeFu.mLocal_Id.at( i ).c_str(), storeFu.mLocal_Id.at( j ).c_str() ) > 0 ) {
            string a = storeFu.mLocal_Id.at( j ) ; 
            storeFu.mLocal_Id.at( j ) = storeFu.mLocal_Id.at( i ) ;
            storeFu.mLocal_Id.at( i ) = a ;
          } // if 
        } // for
      } // for
      // ---------------區域變數排序--------------------
      uHadDefined_Function_Table.push_back( storeFu ) ; // 存Function 
    } // else
    // -------------Function排序-----------------------
    for ( int i = 0 ; i < uHadDefined_Function_Table.size() ; i++ ) {
      for ( int j = i  ; j < uHadDefined_Function_Table.size() ; j++ ) {
        string a = uHadDefined_Function_Table.at( j ).mFunctionName ;
        if ( strcmp( uHadDefined_Function_Table.at( i ).mFunctionName.c_str(), a.c_str() ) > 0 ) {
          StoreFunctionType test = uHadDefined_Function_Table.at( j ) ;
          uHadDefined_Function_Table.at( j ) = uHadDefined_Function_Table.at( i ) ;
          uHadDefined_Function_Table.at( i ) = test ;
        } // if 
      } // for
    } // for
    // -------------Function排序-----------------------
  } // if 
  else if ( uNeedDefinedObject.mType == IDENTTYPE ) {
    // 全域變數
    cout << "> " ;
    int detailnum = 0 ;
    for ( int j = 0 ; j < uNeedDefinedObject.mGlobal_Id.size() ; j++ ) {
      // 掃過需要被定義的全域變數，一個一個設定
      int address = 0 ; // 紀錄要放進去mDetail的位置 
      exist = false ;  // 若有被定義過，需重定義，exist設為true  
      for ( int i = 0 ; i < uHadDefined_Id_Table.size() ; i++  ) {
        string a = uNeedDefinedObject.mGlobal_Id.at( j ).mName ;
        if ( uHadDefined_Id_Table.at( i ).mName == a ) {
          address = i ; // 紀錄被定義過的ID在Table裡的位置
          exist = true ;
        } // if
      } // for

      string typeDetail = uBeJudgeTokenTable.at( 0 ).mName ;
      int k = detailnum + 1 ; 
      while ( k < uBeJudgeTokenTable.size() && uBeJudgeTokenTable.at( k ).mName != ";"
              && uBeJudgeTokenTable.at( k ).mName != "," ) {
        // 紀錄定義方式，ListVariable會用到
        // 碰到,、;則跳出 
        // detailnum是紀錄下個全域變數的位置
        if ( uBeJudgeTokenTable.at( k ).mName == "[" || uBeJudgeTokenTable.at( k ).mName == ","  ) 
          typeDetail = typeDetail + uBeJudgeTokenTable.at( k ).mName ;
        else 
          typeDetail = typeDetail + " " + uBeJudgeTokenTable.at( k ).mName ;

        k++ ;
      } // while

      detailnum = k ; // k出迴圈代表此位置是;、,，設定detailnum下次再把k+1執行
      typeDetail = typeDetail + " ;" ; // 在最後加上分號 

      if ( exist ) {
        // 被定義過
        string a = uHadDefined_Id_Table.at( address ).mName ;
        uHadDefined_Id_Table.at( address ).mDetail = typeDetail ; // 紀錄定義方式，ListVariable會用到
        cout << "New definition of " + a + " entered ..." << endl ; 
      } // if
      else {
      // 沒有被定義過
        Token storeId ;
        storeId.mName = uNeedDefinedObject.mGlobal_Id.at( j ).mName ; // 存全域名字
        storeId.mDetail = typeDetail ; // 紀錄定義方式，ListVariable會用到
        cout << "Definition of " + storeId.mName + " entered ..." << endl ;
        uHadDefined_Id_Table.push_back( storeId ) ; // 存Function 
      } // else

    } // for

    // ----------------------全域變數排序-----------------------
    for ( int i = 0 ; i < uHadDefined_Id_Table.size() ; i++ ) {
      for ( int j = i  ; j < uHadDefined_Id_Table.size() ; j++ ) {
        string a = uHadDefined_Id_Table.at( j ).mName ;
        if ( strcmp( uHadDefined_Id_Table.at( i ).mName.c_str(), a.c_str() ) > 0 ) {
          Token test = uHadDefined_Id_Table.at( j ) ;
          uHadDefined_Id_Table.at( j ) = uHadDefined_Id_Table.at( i ) ;
          uHadDefined_Id_Table.at( i ) = test ; 
        } // if 
      } // for
    } // for
    // ------------------------全域變數排序----------------------
  } // else if

} // StoreTrueDefine()
// ----------------------GetToken-------------------------

// ----------------------Grammer-------------------------
bool User_input( bool &Done ) {
// user_input : ( definition | statement ) { definition | statement }
  while ( 1 ) {
    // 持續回圈，直到不符合文法
    if ( Definition() ) {
      // 定義正確，將uNeedDefinedObject放到uHadDefinedTokenTable裡
      StoreTrueDefine() ; 
      return true ;
    } // if
    else if ( Statement() ) {
      cout << "> " ; 
      if ( uBeJudgeTokenTable.at( 0 ).mName == "Done" && uBeJudgeTokenTable.at( 1 ).mName == "("
           && uBeJudgeTokenTable.at( 2 ).mName == ")" ) {
        // 讀取到Done()，結束
        Done = true ;
        return true ;
      } // if
      else {
        if ( uBeJudgeTokenTable.at( 0 ).mName == "ListAllVariables" 
             && uBeJudgeTokenTable.at( 1 ).mName == "("
             && uBeJudgeTokenTable.at( 2 ).mName == ")" ) {
          // 讀取到ListAllVariables()
          // 印出全域變數名字
          for ( int i = 0 ; i < uHadDefined_Id_Table.size() ; i++ ) {
            cout << uHadDefined_Id_Table.at( i ).mName << endl ;
          } // for 
        } // if
        else if ( uBeJudgeTokenTable.at( 0 ).mName == "ListAllFunctions" 
                  && uBeJudgeTokenTable.at( 1 ).mName == "("
                  && uBeJudgeTokenTable.at( 2 ).mName == ")" ) {
          // 讀取到ListAllFunctions()
          // 印出所有Function名字，且排序過
          for ( int i = 0 ; i < uHadDefined_Function_Table.size() ; i++ ) {
            cout << uHadDefined_Function_Table.at( i ).mFunctionName << "()" << endl ;
          } // for
        } // else if
        else if ( uBeJudgeTokenTable.at( 0 ).mName == "ListVariable" 
                  && uBeJudgeTokenTable.at( 1 ).mName == "(" ) {
          // 讀取到ListVariable("") 
          // 印出全域變數，名字在第三個
          int length = uBeJudgeTokenTable.at( 2 ).mName.length()  ;
          string funtname = uBeJudgeTokenTable.at( 2 ).mName.substr( 1, length - 2 ) ;
          // 把""切掉
          for ( int i = 0 ; i < uHadDefined_Id_Table.size() ; i++ ) {
            if ( funtname == uHadDefined_Id_Table.at( i ).mName ) {
              cout << uHadDefined_Id_Table.at( i ).mDetail << endl ;
            } // if  
          } // for
        } // else if
        else if ( uBeJudgeTokenTable.at( 0 ).mName == "ListFunction" 
                  && uBeJudgeTokenTable.at( 1 ).mName == "("
                  && uBeJudgeTokenTable.at( 3 ).mName == ")" ) {
          // 讀取到ListFunction( "" )，Function名字在第2個
          // 印出Function內容
          int length = uBeJudgeTokenTable.at( 2 ).mName.length() ;
          string funtname = uBeJudgeTokenTable.at( 2 ).mName.substr( 1, length - 2 ) ;
          // 把""切掉 
          for ( int i = 0 ; i < uHadDefined_Function_Table.size() ; i++ ) {
            if ( funtname == uHadDefined_Function_Table.at( i ).mFunctionName ) {
              cout << uHadDefined_Function_Table.at( i ).mDetail << endl ;
            } // if
          } // for
        } // else if

        cout << "Statement executed ..." << endl ;
      } // else

      return true ; 
    } // else if
    else return false ; 

  } // while

} // User_input()

bool Definition() {
// definition : VOID Identifier function_definition_without_ID      VOID開頭是Function 
//             | type_specifier Identifier function_definition_or_declarators
//               其他開頭除了Function以外，還可能是ID；String ad[100] ; 
  Token token = PeekToken() ;
  if ( token.mType == VOID ) {
    token = GetToken() ; 
    token = PeekToken() ;
    if ( token.mType == IDENT ) {
      uDontcheckDefined = true ; 
      token = GetToken() ;
      uDontcheckDefined = false ; 
      // Function
      if ( Function_definition_without_id( token ) ) return true ;
    } // if

    uErrorToken = uNowPeekToken.mName ;   // 一定為Defintion卻錯誤 
    string error = "UnexpectedToken" ;
    throw error ;
  } // if
  else if ( Type_specifier( token ) ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mType == IDENT ) {
      uDontcheckDefined = true ; 
      token = GetToken() ;
      uDontcheckDefined = false ; 
      // Function or 全域變數 
      if ( Function_definition_or_declarators( token ) ) return true ;
    } // if

    uErrorToken = uNowPeekToken.mName ;   // 一定為Defintion卻錯誤 
    string error = "UnexpectedToken" ;
    throw error ;
  } // else if

  return false ;
} // Definition()

bool Type_specifier( Token token ) {
// type_specifier : INT | CHAR | FLOAT | STRING | BOOL
  if ( token.mType == INT || token.mType == CHAR || token.mType == FLOAT
       || token.mType == STRING || token.mType == BOOL ) {
    return true ;
  } // if

  return false ;
} // Type_specifier()

bool Function_definition_or_declarators( Token firsttoken ) {
// 傳進來firsttoken是因為不確定它是Function還是全域變數
// function_definition_or_declarators : function_definition_without_ID
                                     // | rest_of_declarators
  if ( Function_definition_without_id( firsttoken ) ) {
    // Function
    uNeedDefinedObject.mType = FUNCTIONTYPE ;
    return true ;
  } // if
  else {
    // 全域變數，由於原先設定為Function，需要改成Ident型別
    uNeedDefinedObject.mType = IDENTTYPE ;
    SetNeedDefinedObject( firsttoken, IDENTTYPE, false ) ;
    // 將要存的Function or ID 暫存起來 ( 要存入的Token, 型別, 是否為區域變數 )
    if ( Rest_of_declarators() ) return true ;
  } // else 

  return false ;
} // Function_definition_or_declarators()

bool Rest_of_declarators() {
// rest_of_declarators : [ '[' Constant ']' ] { ',' Identifier [ '[' Constant ']' ] } ';'
// 區域變數、全域變數都會用到此Function 
  Token token = PeekToken() ;
  if ( token.mName == "[" ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mType == CONSTANT ) {
      token = GetToken() ;
      token = PeekToken() ;
      if ( token.mName == "]" ) {
        token = GetToken() ;
      } // if
      else return false ;
    } // if
    else return false ;

    // 會跑到這邊代表!，有[CONSTANT]但下個while沒有先Peek
    // 故這邊要Peek
    // 如果沒有進來[CONSTANT]，上面就有Peek過
  } // if

  token = PeekToken() ;
  while ( token.mName == "," ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mType == IDENT ) {
      uDontcheckDefined = true ; 
      token = GetToken() ;
      uDontcheckDefined = false ;
      // 區域變數 or 全域變數
      // 將要存的Function or ID 暫存起來 ( 要存入的Token, 型別, 是否為區域變數 )
      if ( uNeedDefinedObject.mType == FUNCTIONTYPE ) SetNeedDefinedObject( token, IDENTTYPE, true ) ;
      else if ( uNeedDefinedObject.mType == IDENTTYPE ) SetNeedDefinedObject( token, IDENTTYPE, false ) ;
      token = PeekToken() ;
      if ( token.mName == "[" ) {
        token = GetToken() ;
        token = PeekToken() ;
        if ( token.mType == CONSTANT ) {
          token = GetToken() ;
          token = PeekToken() ;
          if ( token.mName == "]" ) {
            token = GetToken() ;
          } // if
          else return false ;
        } // if
        else return false ;
      } // if
    } // if
    else return false ;

    token = PeekToken() ;
  } // while

  token = PeekToken() ;
  if ( token.mName == ";" ) {
    token = GetToken() ;
    return true ;
  } // if

  return false ;
} // Rest_of_declarators()

bool Function_definition_without_id( Token firsttoken ) {
// function_definition_without_ID : 
// 		'(' [ VOID | formal_parameter_list ] ')' compound_statement
  Token token = PeekToken() ;
  if ( token.mName == "(" ) {
    // Function
    uNeedDefinedObject.mType = FUNCTIONTYPE ; // Function型別的定義 
    SetNeedDefinedObject( firsttoken, FUNCTIONTYPE, false ) ; 
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mType == VOID ) {
      token = GetToken() ;
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        if ( Compound_statement() ) return true ;
      } // if
    } // if
    else if ( Formal_parameter_list() ) {
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        if ( Compound_statement() ) return true ;
      } // if
    } // else if
    else if ( token.mName == ")" ) {
      token = GetToken() ;
      if ( Compound_statement() ) return true ;
    } // else if 
  } // if

  return false ;
} // Function_definition_without_id()

bool Formal_parameter_list() {
// formal_parameter_list :
// type_specifier [ '&' ] Identifier [ '[' Constant ']' ] 
// { ',' type_specifier [ '&' ] Identifier [ '[' Constant ']' ] }
// 這邊為區域變數 
  Token token = PeekToken() ;
  if ( Type_specifier( token ) ) {
    token = GetToken() ;
    token = PeekToken() ; // 有可能是& or IDENT 
    if ( token.mName == "&" ) {
      token = GetToken() ;
      uDontcheckDefined = true ;
      token = PeekToken() ; // IDENT 
      uDontcheckDefined = false ; 
    } // if

    if ( token.mType == IDENT ) {
      uDontcheckDefined = true ; 
      token = GetToken() ;
      uDontcheckDefined = false ; 
      SetNeedDefinedObject( token, IDENTTYPE, true ) ; // 將要存的Function or ID 暫存起來 ( 要存入的Token, 型別, 是否為區域變數 )
      token = PeekToken() ;
      if ( token.mName == "[" ) {
        token = GetToken() ;
        token = PeekToken() ;
        if ( token.mType == CONSTANT ) {
          token = GetToken() ;
          token = PeekToken() ;
          if ( token.mName == "]" ) {
            token = GetToken() ;
          } // if
          else return false ;
        } // if
        else return false ;
      } // if
    } // if
    else return false ;
  } // if
  else return false ;
  
  token = PeekToken() ;
  while ( token.mName == "," ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( Type_specifier( token ) ) {
      token = GetToken() ;
      token = PeekToken() ;    // 有可能是& or IDENT 
      if ( token.mName == "&" ) {
        token = GetToken() ;
        uDontcheckDefined = true ;
        token = PeekToken() ; // IDENT
        uDontcheckDefined = false ; 
      } // if

      if ( token.mType == IDENT ) {
        uDontcheckDefined = true ; 
        token = GetToken() ;
        uDontcheckDefined = false ; 
        SetNeedDefinedObject( token, IDENTTYPE, true ) ;
        // 將要存的Function or ID 暫存起來 ( 要存入的Token, 型別, 是否為區域變數 )
        token = PeekToken() ;
        if ( token.mName == "[" ) {
          token = GetToken() ;
          token = PeekToken() ;
          if ( token.mType == CONSTANT ) {
            token = GetToken() ;
            token = PeekToken() ;
            if ( token.mName == "]" ) {
              token = GetToken() ;
            } // if
            else return false ;
          } // if
          else return false ;
        } // if
      } // if
      else return false ;
    } // if
    else return false ;

    token = PeekToken() ;
  } // while

  return true ;
} // Formal_parameter_list()

bool Compound_statement() {
// compound_statement : '{' { declaration | statement } '}'
// 這邊是Function裡的文法
// Declaration 為區域變數 
  Token token = PeekToken() ;
  if ( token.mName == "{" ) {
    uNeedDefinedObject.mType = FUNCTIONTYPE ; 
    token = GetToken() ;
    bool out = false ;
    while ( !out ) {
      if ( Declaration() || Statement() ) {
      } // if
      else out = true ; // 不滿足Declaration、Statement 
    } // while

    token = PeekToken() ;
    if ( token.mName == "}" ) {
      token = GetToken() ;
      return true ;
    } // if
    else {
      // 非常尷尬的Return false 有兩個意思可能不是Defintion，也可能是文法錯誤
      uErrorToken = token.mName ;      // 第二個'.'之後的字串傳上去
      string error = "UnexpectedToken" ;
      throw error ; 
    } // else  
  } // if

  return false ;
} // Compound_statement()

bool Declaration() {
// declaration : type_specifier Identifier rest_of_declarators
  Token token = PeekToken() ;
  if ( Type_specifier( token ) ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mType == IDENT ) {
      uDontcheckDefined = true ; 
      token = GetToken() ;
      uDontcheckDefined = false ; 
      // 區域變數 
      SetNeedDefinedObject( token, IDENTTYPE, true ) ; // 將要存的Function or ID 暫存起來 ( 要存入的Token, 型別, 是否為區域變數 )
      if ( Rest_of_declarators() ) {
        return true ;
      } // if
    } // if
  } // if

  return false ;
} // Declaration()

bool Statement() {
/* 
    statement : ';'     // the null statement
    | expression ';'   expression here should not be empty
    | RETURN [ expression ] ';'
    | compound_statement      // Function 
    | IF '(' expression ')' statement [ ELSE statement ]
    | WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' ';'
*/
  Token token = PeekToken() ;
  if ( token.mName == ";" ) {
    token = GetToken() ;
    return true ;
  } // if
  else if ( Expression() ) {
    token = PeekToken() ;
    if ( token.mName == ";" ) {
      token = GetToken() ;
      return true ;
    } // if
  } // else if
  else if ( token.mType == RETURN ) {
    token = GetToken() ;
    Expression();
    token = PeekToken() ;
    if ( token.mName == ";" ) {
      token = GetToken() ;
      return true ;
    } // if
  } // else if
  else if ( Compound_statement() ) {
    return true ;
  } // else if 
  else if ( token.mType == IF ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mName == "(" ) {
      token = GetToken() ;
      Expression() ;
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        if ( Statement() ) {
          token = PeekToken() ;
          if ( token.mType == ELSE ) {
            token = GetToken() ;
            if ( !Statement() ) return false ;
          } // if

          return true ; //  [ ELSE Statement ]
        } // if
      } // if
    } // if
  } // else if
  else if ( token.mType == WHILE ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mName == "(" ) {
      token = GetToken() ;
      Expression() ;
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        if ( Statement() ) {
          return true ;
        } // if
      } // if
    } // if
  } // else if
  else if ( token.mType == DO ) {
    token = GetToken() ;
    if ( Statement() ) {
      token = PeekToken() ;
      if ( token.mType == WHILE ) {
        token = GetToken() ;
        token = PeekToken() ;
        if ( token.mName == "(" ) {
          token = GetToken() ;
          Expression();
          token = PeekToken() ;
          if ( token.mName == ")" ) {
            token = GetToken() ;
            token = PeekToken() ;
            if ( token.mName == ";" ) {
              token = GetToken() ;
              return true ;
            } // if
          } // if
        } // if
      } // if
    } // if
  } // else if

  return false ;
} // Statement()

bool Expression() {
// expression : basic_expression { ',' basic_expression }
  if ( Basic_expression() ) {
    Token token = PeekToken() ;
    while ( token.mName == "," ) {
      token = GetToken() ;
      if ( !Basic_expression() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Expression()

bool Basic_expression() {
/*
  basic_expression : 
  1.  Identifier rest_of_Identifier_started_basic_exp
  2.  | ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
  3.  | sign { sign } signed_unary_exp romce_and_romloe
  4.  | ( Constant | '(' expression ')' ) romce_and_romloe
*/
  Token token = PeekToken() ;
  if ( token.mType == IDENT ) {
    token = GetToken() ;
    if ( Rest_of_identifier_started_basic_exp() ) return true ;
  } // if
  else if ( token.mType == PP || token.mType == MM ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mType == IDENT ) {
      token = GetToken() ;
      if ( Rest_of_PPMM_identifier_started_basic_exp() ) return true ;
    } // if
  } // else if
  else if ( Sign() ) {
    while ( Sign() ) { } ;
    if ( Signed_unary_exp() ) {
      if ( Romce_and_romloe() ) return true ;
    } // if
  } // else if
  else if ( token.mType == CONSTANT || token.mName == "(" ) {
    token = GetToken() ;
    if ( token.mName == "(" ) {
      if ( Expression() ) {
        token = PeekToken() ;
        if ( token.mName == ")" ) {
          token = GetToken() ;
          if ( Romce_and_romloe() ) return true ;
        } // if 
      } // if

      return false ; 
    } // if
    else {
      if ( Romce_and_romloe() ) {
        return true ;
      } // if 
    } // else 
  } // else if

  return false ;
} // Basic_expression()

bool Rest_of_identifier_started_basic_exp() {
// rest_of_Identifier_started_basic_exp :
// 1.  [ '[' expression ']' ] 
//     ( assignment_operator basic_expression | [ PP | MM ] romce_and_romloe )  
// 2.  |'(' [ actual_parameter_list ] ')' romce_and_romloe  
// 得先判斷 '['、'('  ，因為Romce_and_romloe就算都沒有也是True 
  Token token = PeekToken() ;
  if ( token.mName == "[" ) {
    if ( token.mName == "["  ) {
      token = GetToken() ;
      if ( Expression() ) {
        token = PeekToken() ;
        if ( token.mName == "]" ) {
          token = GetToken() ;
        } // if
      } // if
    } // if

    if ( Assignment_operator() ) {
      if ( Basic_expression() ) {
        return true ;
      } // if
    } // if
    else {
      token = PeekToken() ;
      if ( token.mType == PP || token.mType == MM ) {
        token = GetToken() ;
        if ( Romce_and_romloe() ) {
          return true ;
        } // if
      } // else if
      else if ( Romce_and_romloe() ) {
        return true ;
      } // else if
    } // else

    return false ;
  } // if
  else if ( token.mName == "(" ) {
    if ( token.mName == "(" ) {
      token = GetToken() ;
      Actual_parameter_list() ; 
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        if ( Romce_and_romloe() ) return true ;
      } // if   
    } // if
  } // else if 
  else if ( Assignment_operator() ) { 
    if ( Basic_expression() ) {
      return true ;
    } // if
  } // else if
  else if ( token.mType == PP || token.mType == MM ) {
    token = GetToken() ;
    if ( Romce_and_romloe() ) {
      return true ;
    } // if
  } // else if
  else if ( Romce_and_romloe() ) {
    return true ;
  } // else if


  return false ;
} // Rest_of_identifier_started_basic_exp()

bool Rest_of_PPMM_identifier_started_basic_exp() {
// rest_of_PPMM_Identifier_started_basic_exp : 
//    [ '[' expression ']' ] romce_and_romloe
  Token token = PeekToken() ;
  if ( token.mName == "[" ) {
    token = GetToken() ;
    if ( Expression() ) {
      token = PeekToken() ;
      if ( token.mName == "]" ) {
        token = GetToken() ;
        if ( Romce_and_romloe() ) return true ;
      } // if
    } // if
  } // if
  else {
    if ( Romce_and_romloe() ) return true ;
  } // else

  return false ;
} // Rest_of_PPMM_identifier_started_basic_exp()

bool Sign() {
// sign : '+' | '-' | '!'
  Token token = PeekToken() ;
  if ( token.mName == "+" || token.mName == "-" || token.mName == "!" ) {
    token = GetToken() ;
    return true ;
  } // if

  return false ;
} // Sign()

bool Actual_parameter_list() {
// actual_parameter_list : basic_expression { ',' basic_expression }
  if ( Basic_expression() ) {
    Token token = PeekToken() ;
    while ( token.mName == "," ) {
      token = GetToken() ;
      if ( !Basic_expression() ) {
        return false ;
      } // if

      token = PeekToken() ;
    } // while

    return true ; // while裡 需用!判斷，不對的話則return false
  } // if

  return false ;
} // Actual_parameter_list()

bool Assignment_operator() {
// assignment_operator : '=' | TE | DE | RE | PE | ME
  Token token = PeekToken() ;
  if ( token.mName == "=" || token.mType == TE || token.mType == DE 
       || token.mType == RE || token.mType == PE || token.mType == ME ) {
    token = GetToken() ;
    return true ;
  } // if

  return false ;
} // Assignment_operator()

bool Romce_and_romloe() {
//  rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp // 即romce_and_romloe
//    : rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]
  if ( Rest_of_maybe_logical_or_exp() ) {
    Token token = PeekToken() ;
    if ( token.mName == "?" ) {
      token = GetToken() ;
      if ( Basic_expression() ) {
        token = PeekToken() ;
        if ( token.mName == ":" ) {
          token = GetToken() ;
          if ( Basic_expression() ) return true ;
        } // if
      } // if

      return false ;
    } // if

    return true ;
  } // if

  return false ;
} // Romce_and_romloe()

bool Rest_of_maybe_logical_or_exp() {
// rest_of_maybe_logical_OR_exp : 
// rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }
  if ( Rest_of_maybe_logical_and_exp() ) {
    Token token = PeekToken() ;
    while ( token.mType == OR ) {
      token = GetToken() ;
      if ( !Maybe_logical_and_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_logical_or_exp()

bool Maybe_logical_and_exp() {
// maybe_logical_AND_exp : maybe_bit_OR_exp { AND maybe_bit_OR_exp }
  if ( Maybe_bit_or_exp() ) {
    Token token = PeekToken() ;
    while ( token.mType == AND ) {
      token = GetToken() ;
      if ( !Maybe_bit_or_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Maybe_logical_and_exp()

bool Rest_of_maybe_logical_and_exp() {
// rest_of_maybe_logical_AND_exp : 
// rest_of_maybe_bit_OR_exp { AND maybe_bit_OR_exp }
  if ( Rest_of_maybe_bit_or_exp() ) {
    Token token = PeekToken() ;
    while ( token.mType == AND ) {
      token = GetToken() ;
      if ( !Maybe_bit_or_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_logical_and_exp()

bool Maybe_bit_or_exp() {
// maybe_bit_OR_exp : maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }
  if ( Maybe_bit_ex_or_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "|" ) {
      token = GetToken() ;
      if ( !Maybe_bit_ex_or_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Maybe_bit_or_exp()

bool Rest_of_maybe_bit_or_exp() {
// rest_of_maybe_bit_OR_exp : 
// rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }
  if ( Rest_of_maybe_bit_ex_or_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "|" ) {
      token = GetToken() ;
      if ( !Maybe_bit_ex_or_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_bit_or_exp()

bool Maybe_bit_ex_or_exp() {
// maybe_bit_ex_OR_exp : maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
  if ( Maybe_bit_and_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "^" ) {
      token = GetToken() ;
      if ( !Maybe_bit_and_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Maybe_bit_ex_or_exp()

bool Rest_of_maybe_bit_ex_or_exp() {
// rest_of_maybe_bit_ex_OR_exp : 
// rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
  if ( Rest_of_maybe_bit_and_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "^" ) {
      token = GetToken() ;
      if ( !Maybe_bit_and_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_bit_ex_or_exp()

bool Maybe_bit_and_exp() {
// maybe_bit_AND_exp : maybe_equality_exp { '&' maybe_equality_exp }
  if ( Maybe_equality_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "&" ) {
      token = GetToken() ;
      if ( !Maybe_equality_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Maybe_bit_and_exp()

bool Rest_of_maybe_bit_and_exp() {
// rest_of_maybe_bit_AND_exp : 
// rest_of_maybe_equality_exp { '&' maybe_equality_exp }
  if ( Rest_of_maybe_equality_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "&" ) {
      token = GetToken() ;
      if ( !Maybe_equality_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_bit_and_exp()

bool Maybe_equality_exp() {
// maybe_equality_exp : 
// maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp }
  if ( Maybe_relational_exp() ) {
    Token token = PeekToken() ;
    while ( token.mType == EQ || token.mType == NEQ ) {
      token = GetToken() ;
      if ( !Maybe_relational_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Maybe_equality_exp()

bool Rest_of_maybe_equality_exp() {
// rest_of_maybe_equality_exp : 
// rest_of_maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp }
  if ( Rest_of_maybe_relational_exp() ) {
    Token token = PeekToken() ;
    while ( token.mType == EQ || token.mType == NEQ ) {
      token = GetToken() ;
      if ( !Maybe_relational_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_equality_exp()

bool Maybe_relational_exp() {
// maybe_relational_exp : 
// maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }
  if ( Maybe_shift_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "<" || token.mName == ">" || token.mType == LE || token.mType == GE ) {
      token = GetToken() ;
      if ( !Maybe_shift_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Maybe_relational_exp()

bool Rest_of_maybe_relational_exp() {
// rest_of_maybe_relational_exp : 
// rest_of_maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }
  if ( Rest_of_maybe_shift_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "<" || token.mName == ">" || token.mType == LE || token.mType == GE ) {
      token = GetToken() ;
      if ( !Maybe_shift_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_relational_exp()

bool Maybe_shift_exp() {
// maybe_shift_exp : 
// maybe_additive_exp { ( LS | RS ) maybe_additive_exp }
  if ( Maybe_additive_exp() ) {
    Token token = PeekToken() ;
    while ( token.mType == LS || token.mType == RS ) {
      token = GetToken() ;
      if ( !Maybe_additive_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Maybe_shift_exp()

bool Rest_of_maybe_shift_exp() {
// rest_of_maybe_shift_exp : 
// rest_of_maybe_additive_exp { ( LS | RS ) maybe_additive_exp }
  if ( Rest_of_maybe_additive_exp() ) {
    Token token = PeekToken() ;
    while ( token.mType == LS || token.mType == RS ) {
      token = GetToken() ;
      if ( !Maybe_additive_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_shift_exp()

bool Maybe_additive_exp() {
// maybe_additive_exp : maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
  if ( Maybe_mult_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "+" || token.mName == "-" ) {
      token = GetToken() ;
      if ( !Maybe_mult_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Maybe_additive_exp()

bool Rest_of_maybe_additive_exp() {
// rest_of_maybe_additive_exp : 
// rest_of_maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
  if ( Rest_of_maybe_mult_exp() ) {
    Token token = PeekToken() ;
    while ( token.mName == "+" || token.mName == "-" ) {
      token = GetToken() ;
      if ( !Maybe_mult_exp() ) return false ;
      token = PeekToken() ;
    } // while

    return true ;
  } // if

  return false ;
} // Rest_of_maybe_additive_exp()

bool Maybe_mult_exp() {
// maybe_mult_exp : unary_exp rest_of_maybe_mult_exp
  if ( Unary_exp() ) {
    if ( Rest_of_maybe_mult_exp() ) return true ;
  } // if

  return false ;
} // Maybe_mult_exp()

bool Rest_of_maybe_mult_exp() {
// rest_of_maybe_mult_exp : 
// { ( '*' | '/' | '%' ) unary_exp }  /* could be empty ! */
  // bool out = false ; // 跳出迴圈用
  Token token = PeekToken() ;
  while ( token.mName == "*" || token.mName == "/" || token.mName == "%" ) {
    token = GetToken() ;
    if ( !Unary_exp() ) return false ; // 兩個條件同時發生
    token = PeekToken() ;
  } // while

  return true ; // 理論上不會跑到這，但文法要求得寫
} // Rest_of_maybe_mult_exp()

bool Unary_exp() {
// unary_exp : sign { sign } signed_unary_exp
            // | unsigned_unary_exp
            // | ( PP | MM ) Identifier [ '[' expression ']' ]
  if ( Sign() ) {
    while ( Sign() ) { } // while

    if ( Signed_unary_exp() ) {
      return true ;
    } // if
  } // if
  else if ( Unsigned_unary_exp() ) {
    return true ;
  } // else if
  else {
    Token token = PeekToken() ;
    if ( token.mType == PP || token.mType == MM ) {
      token = GetToken() ;
      token = PeekToken() ;
      if ( token.mType == IDENT ) {
        token = GetToken() ;
        token = PeekToken() ;
        if ( token.mName == "[" ) {
          token = GetToken() ;
          if ( Expression() ) {
            token = PeekToken() ;
            if ( token.mName == "]" ) {
              token = GetToken() ;
              return true ;
            } // if
          } // if

          return false ;
        } // if

        return true  ;
      } // if
    } // if
  } // else if

  return false ;
} // Unary_exp()

bool Signed_unary_exp() {
// signed_unary_exp : 
// 1. Identifier [ '(' [ actual_parameter_list ] ')' |'[' expression ']' ]
// 2. | Constant
// 3. | '(' expression ')'
  Token token = PeekToken() ;
  if ( token.mType == IDENT ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mName == "(" ) {
      token = GetToken() ;
      Actual_parameter_list() ;
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        return true ;
      } // if

      return false ;
    } // if
    else if ( token.mName == "[" ) {
      token = GetToken() ;
      if ( Expression() ) {
        token = PeekToken() ;
        if ( token.mName == "]" ) {
          token = GetToken() ;
          return true ;
        } // if
      } // if

      return false ;
    } // else if

    return true ;
  } // if
  else if ( token.mType == CONSTANT ) {
    token = GetToken() ;
    return true ;
  } // else if
  else if ( token.mName == "(" ) {
    token = GetToken() ;
    if ( Expression() ) {
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        return true ;
      } // if
    } // if

    return false ;
  } // else if

  return false ;
} // Signed_unary_exp()

bool Unsigned_unary_exp() {
// unsigned_unary_exp : 
// 1.  Identifier [ '(' [ actual_parameter_list ] ')' | [ '[' expression ']' ] [ ( PP | MM ) ] ]
// 2.  | Constant
// 3.  | '(' expression ')'
  Token token = PeekToken() ;
  if ( token.mType == IDENT ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mName == "(" ) {
      token = GetToken() ;
      Actual_parameter_list() ;
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        return true ;
      } // if 

      return false ;
    } // if
    else if ( token.mName == "[" ) {
      token = GetToken() ;
      if ( Expression() ) {
        token = PeekToken() ;
        if ( token.mName == "]" ) {
          token = GetToken() ;
          token = PeekToken() ;
          if ( token.mType == PP || token.mType == MM ) {
            token = GetToken() ;
          } // if

          return true ;
        } // if
      } // if

      return false ;
    } // else if
    else if ( token.mType == PP || token.mType == MM ) {
      token = GetToken() ;
    } // else if 

    return true ;
  } // if
  else if ( token.mType == CONSTANT ) {
    token = GetToken() ;
    return true ;
  } // else if
  else if ( token.mName == "(" ) {
    token = GetToken() ;
    if ( Expression() ) {
      token = PeekToken() ;
      if ( token.mName == ")" ) {
        token = GetToken() ;
        return true ;
      } // if
    } // if
  } // else if

  return false ;
} // Unsigned_unary_exp()

// ----------------------Grammer-------------------------
int main() {
  int uTestNum = 0;
  cin >> uTestNum;
  bool done = false ;
  cout << "Our-C running ..." << endl;
  uLineNum = 0, uColumnNum = 0 ;
  ulastline = 0 ;
  while ( !done ) {
    usetline = true ; // 設定一次設為false
    uDetail = "" ;
    uColumnNum = 0 ; 
    uNowGetToken.mName = "" ;
    uNowPeekToken.mName = "" ;
    uBeJudgeTokenTable.clear() ;
    uNeedDefinedObject.mGlobal_Id.clear() ; // 暫存需要被定義的全域變數 
    uNeedDefinedObject.mFunct_Local_Id.clear() ; // 暫存需要被定義的Function或區域變數 
    uErrorToken = "" ; // 發生錯誤，用來記錄錯誤Token
    try {
      if ( User_input( done ) ) {
        if ( uNowPeekToken.mName != uNowGetToken.mName
             || uNowPeekToken.mLine != uNowPeekToken.mLine 
             || uNowPeekToken.mColumn != uNowPeekToken.mColumn ) { 
          // 有多Peek但沒有Get，則把Peek的東西留到下次
          uErrorLeftTokenTable.push_back( uNowPeekToken ) ;
          // 設定上次grammer最後位置、為最後Peek的Token位置
          ulastline = uBeJudgeTokenTable.back().mLine ;
        } // if
        else ulastline = uLineNum ; // 正常結束，沒有多Peek

      } // if
      else {
        // 這邊錯誤是文法上的錯誤
        cout << "> Line " << uLineNum << " : unexpected token : '" << uNowPeekToken.mName << "'" << endl ;
        CleanErrorLine();
        ulastline = uLineNum ; // Error!清掉整行、代表沒有多Peek
      } // else

    } // try
    catch( string message ) {
      // 跳出UnrecognizedToken
      if ( message == "UnexpectedToken" ) {
        // Token的錯誤 
        cout << "> Line " << uLineNum << " : unexpected token : '" << uErrorToken << "'" << endl ; 
        CleanErrorLine();
      } // if

      ulastline = uLineNum ; // Error!清掉整行、代表沒有多Peek
    } // catch

  } // while

  cout << "Our-C exited ..." ;

} // main()



