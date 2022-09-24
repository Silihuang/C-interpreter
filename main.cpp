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
      LS = 28, SYMBOL = 29, DONE = 30// Done�����P�_�O�_��UndefinedToken�A�n�P�_�᭱���L() 
};

enum Funct_id_type{   // �Q�w�q���O ID �٬O Function 
  FUNCTIONTYPE = 1, IDENTTYPE = 2 
};

class Token {                      // Ū����Token
public :
  string mName ;
  Type mType ;
  int mLine ;
  int mColumn  ;
  string mDetail ; // �����ܼƳQ�w�q�ɪ����e�A���F�����ܼƨ�L���p�S�Ψ� 
};

class Funct_or_id{ // �ȦsBeDefined���O 
  public :
  Funct_id_type mType ;   // ID or Function Type 
  vector <Token> mFunct_Local_Id ; // Function�̪�ID
  vector <Token> mGlobal_Id ; // �����ܼ� 
};

class StoreFunctionType{ // �x�sHadDefined��Function���O 
  public :
  string mFunctionName ; // Function�̪�ID
  vector <string> mLocal_Id ; // �ϰ��ܼ�
  string mDetail ; // Function�̪����e 
};

static int uLineNum = 0, uColumnNum = 0 ; // ����Token����}
static string uDetail ; // �s�Ҧ�Ū���F��
static vector<Token> uBeJudgeTokenTable; // �s��ֹ�L��k��Token
static vector<Token> uErrorLeftTokenTable; // �]Error�s�d�U�Ӫ�Token

static Funct_or_id uNeedDefinedObject ; // ���T�{�y�k���T�e�A�n�w�q��Object 
static vector<StoreFunctionType> uHadDefined_Function_Table; // �s�Q�w�q��Function�A��K�L�X
static vector<Token> uHadDefined_Id_Table; // �s�Q�w�q��ID�A��K�L�X

static string uErrorToken ; // �������~��Token
static bool uDontcheckDefined  ; // �׶}defined�P�_
static Token uNowPeekToken ; // PeekToken��ΨӦs��Token��Buffer�A�Y�S��Get�h���|Peek�s��Token
static Token uNowGetToken ; // GetToken��s��Token��Buffer
static bool usetline ; // �O�_�٥��]�w 
static int ulastline ; // �W�������᪺line�� 
// -----------�u��------------
Token PeekToken() ;
void KeepTrack( char input ) ;
bool GetNextChar( char &next ) ;
Token GetToken() ;
void SetNeedDefinedObject( Token token, Funct_id_type type, bool Local ) ; // �Ȧs�ݭn�Q�w�q��Object
void StoreTrueDefine() ; 
bool UnrecognizedToken( char word ) ;
bool UnexpectedToken( Type &tokentype, string tokenValue, bool NumorDot, string &errorToken ) ;
bool UndefinedIdentifier( string Name ) ;
string Reform() ;
void CleanErrorLine() ;
// -----------�u��------------

// -----------��k------------
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
// -----------��k------------


// ------------------------------------------Syntax Test------------------------------------------

// ----------------------GetToken-------------------------
string Reform() {
  // �W�h: Function"("�e�����ťաA�C��Token�j1�ťաA�C�����O�n����B�e���Ũ��
  //       �̫᭱"}"�e���S�ťաA����S���� 
  string recontent = "" ;
  recontent = uBeJudgeTokenTable.at( 0 ).mName ;
  int spacesize = -1 ; // �ťդj�p�|�H�ۦh�@�h("{")+2�A�H�֤@�h("}")-2
  for ( int i = 1 ; i < uBeJudgeTokenTable.size() ; i++ ) {

    if ( uBeJudgeTokenTable.at( i ).mName == "}" || uBeJudgeTokenTable.at( i ).mName == "[" 
         || ( uBeJudgeTokenTable.at( i ).mName == "(" && uBeJudgeTokenTable.at( i - 1 ).mType == IDENT )
         || ( uBeJudgeTokenTable.at( i ).mName == ")" && uBeJudgeTokenTable.at( i - 1 ).mName == "(" )
         || ( uBeJudgeTokenTable.at( i ).mName == "++" && uBeJudgeTokenTable.at( i - 1 ).mType == IDENT )
         || ( uBeJudgeTokenTable.at( i ).mName == "--" && uBeJudgeTokenTable.at( i - 1 ).mType == IDENT )
         || ( uBeJudgeTokenTable.at( i ).mType == IDENT && uBeJudgeTokenTable.at( i - 1 ).mName == "++" )
         || ( uBeJudgeTokenTable.at( i ).mType == IDENT && uBeJudgeTokenTable.at( i - 1 ).mName == "--" )
         || uBeJudgeTokenTable.at( i ).mName == "," ) {
      // ex: test() �B a[10] �B int a, b �B a++ �B --a 
      recontent = recontent + uBeJudgeTokenTable.at( i ).mName ; // ����Ů� 
    } // if
    else {
      // �Ů涡�Z�O1�A�C��Ĥ@��Token�|���ť�!!!!! 
      recontent = recontent + " " + uBeJudgeTokenTable.at( i ).mName ; 
    } // else 

    // --------------------�H�W��Token�������ť�-------------------------- 
    if ( uBeJudgeTokenTable.at( i ).mName == "{" ) spacesize = spacesize + 2 ;
    else if  ( uBeJudgeTokenTable.at( i ).mName == "}" ) spacesize = spacesize - 2 ;
    // �h�@�h�B�֤@�h�����ťդj�p
    string space = "" ;
    for ( int j = 0 ; j < spacesize ; j++ ) {
      // �p��C��Ĥ@��Token�e���ťժ��j�p�A�O�o�n�]�t1�Ӷ��Z�ť�
      space = space + " " ;
    } // for

    if ( uBeJudgeTokenTable.at( i ).mName == "{" || uBeJudgeTokenTable.at( i ).mName == ";" 
         || uBeJudgeTokenTable.at( i ).mName == "}" ) {
      // ����BToken�e�����ť�
      if ( uBeJudgeTokenTable.at( i ).mName == "}" && spacesize == -1 ) { 
      // �̫�@��"}"�U�椣�δ���
      } // if
      else if ( ( i + 2 ) <= uBeJudgeTokenTable.size() && uBeJudgeTokenTable.at( i + 1 ).mName == "}" ) { 
      // �U�@�ӬO"}"�A�U�@��e���|�֨��
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

  // �P�_���LUnrecognized Token
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
  // �N�S�O��IDENT ���w�q���O
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
  // SIGN �M NUM �i�H�O��Ʃ�Float
    int num = 0;
    // �p�� '.'�ƶq
    for ( int i = 0; i < tokenValue.size() ; i++ ) {
      if ( tokenValue.at( i ) == '.' )  num++ ;
    } // for

    if ( num > 1 ) {

      // �o�զr��(NUM�BSIGN)�W�L�@�ӳr���AERROR!!
      int dotnum = 0;     // �p��'.'�B�ƶq
      string numAfter2Dot = "";
      // �����ĤG�ӳr�����᪺�Ʀr�A�Ψӳ���
      for ( int i = 0 ; i < tokenValue.length() ; i++ ) {
        if ( tokenValue.at( i ) == '.' ) dotnum++;
        if ( dotnum == 2 ) {
          numAfter2Dot = numAfter2Dot + tokenValue.at( i );
        } // if
      } // for

      uErrorToken = numAfter2Dot;      // �ĤG��'.'���᪺�r��ǤW�h
      string error = "UnexpectedToken" ;
      throw error ;
      return false;
    } // if
  } // if

  return true;
} // UnexpectedToken()

bool UndefinedIdentifier( string Name ) {
  // ���w�qDebug�A���������ܼơBFunction�B�ϰ��ܼ�
  if ( uNeedDefinedObject.mType == FUNCTIONTYPE ) {
  // Function�̭���statement�A�ݧP�_�L���S���Q�w�q�b     "�ϰ��ܼơB�����ܼơBFunction"
    for ( int i = 0; i < uNeedDefinedObject.mFunct_Local_Id.size() ; i++ ) {
      if ( Name == uNeedDefinedObject.mFunct_Local_Id.at( i ).mName ) {
        // �ϰ��ܼ� 
        return true;
      } // if
    } // for

    for ( int i = 0; i < uHadDefined_Id_Table.size() ; i++ ) {
      if ( Name == uHadDefined_Id_Table.at( i ).mName ) {
        // �����ܼ� 
        return true;
      } // if
    } // for

    for ( int i = 0; i < uHadDefined_Function_Table.size() ; i++ ) {
      if ( Name == uHadDefined_Function_Table.at( i ).mFunctionName ) {
        // ��LFunction
        return true;
      } // if
    } // for 
  } // if
  else if ( uNeedDefinedObject.mType == IDENTTYPE ) {
  // main�̭���Statement�A�ݧP�_�L���S���Q�w�q�b�����ܼ�
    for ( int i = 0; i < uHadDefined_Id_Table.size() ; i++ ) {
      if ( Name == uHadDefined_Id_Table.at( i ).mName ) {
        // �����ܼ� 
        return true;
      } // if
    } // for

    for ( int i = 0; i < uHadDefined_Function_Table.size() ; i++ ) {
      if ( Name == uHadDefined_Function_Table.at( i ).mFunctionName ) {
        // ��LFunction
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

void KeepTrack( char input ) {      // ����Get�쪺char��}

  if ( input == '\n' ) {      // ����
    uColumnNum = 0;
    uLineNum++;
  } // if
  else {
    uColumnNum++ ;
  } // else

  // �o��O���F�ȨS���W�촫���uLineNum�٬O0�����A
  // ex: a + b ; & 
  // Error:& �A�i�b�����e�S����A�u��ۦ���
  // if ( uLineNum == 0 ) uLineNum = 1 ; 

} // KeepTrack()

void CleanErrorLine() {

  // �N�X�{error������R��
  // Error token�H�e����ƥ��M��
  /*
  for ( int i = 0 ; i < uBeJudgeTokenTable.size() ; i++ ) {
    if ( uBeJudgeTokenTable.at(i).mLine > uLineNum ) {
    // �j��ErrorToken����Ʀs�_�ӯd��U��
      uErrorLeftTokenTable.push_back( uBeJudgeTokenTable.at( i ) ) ;
    } // if
  } // for
  */

  char clean = '\0' ;
  clean = cin.peek() ;
  while ( clean != '\n' && clean != EOF ) {
  // ���⴫��Ū���AUlinenum�aLine�W�[
    scanf( "%c", &clean ) ;
    clean = cin.peek() ;
  } // while

} // CleanErrorLine()

char SkipWhiteSpaceCommentChar() {
  // ���LWhiteSpace + ���� 
  char nextchar = cin.peek();
  while ( nextchar == ' ' || nextchar == '\t' || nextchar == '\n' ) {
    scanf( "%c", &nextchar );
    KeepTrack( nextchar ) ;
    nextchar = cin.peek();
  } // while

  KeepTrack( nextchar ) ; 
  char next = cin.peek() ;
  if ( next == '/' ) {
    // 1.�Y���OComment�A�u�O'/'�hŪ�J��X�C
    // 2.�OComment�A�h�M������
    next = getchar() ;   // Ū�J
    KeepTrack( next ) ;                   // ����Get�쪺�r����m
    char next2 = cin.peek();
    if ( next2 == '/' ) {  // �J��comment
      next = getchar() ;
      next = cin.peek() ;
      while ( next != '\n' ) {  // �NcommentŪ���A���촫��
        scanf( "%c", &next );
        next = cin.peek() ;
      } // while

      // KeepTrack( next );        // while�X�Ӭ�'\n'�A���s�p��Line
      // ���j!!�A�קK�h����ѡA�I�s�ۨ�
      next = SkipWhiteSpaceCommentChar();
    } // if

    return next ;
  } // if
    
  next = getchar() ;   // Ū�J
  KeepTrack( next ) ;                   // ����Get�쪺�r����m
  // cout << ulastline << "  " << uLineNum << endl ; 
  if ( usetline == true && ulastline != 0 ) {
  // �b�C�����O���Ĥ@��Token�]�wLineNum
  // �o��Error or �s���O�N�|�p�� 
  // 1.�W���̫᪺Line�ƻP�{�b��Line�Ƥ@�ˡA�N��P�@�榳��ӫ��O�Aline�]��1 
  // 2.���@�˫h�{�blinenum-�W��linenum�۴�
    if ( ulastline == uLineNum ) uLineNum = 1 ; 
    else uLineNum = uLineNum - ulastline ;
    usetline = false ;
  } // if
  // �p��uLineNum
  UnrecognizedToken( next ) ; // �P�_���LError 
  return  next ;

} // SkipWhiteSpaceCommentChar()

bool GetNextChar( char &next ) {
  // �o��@��char�B�P�_�O�_�X�k
  next = '\0' ;
  scanf( "%c", &next );
  KeepTrack( next );
  if ( next == '/' ) {
    // 1.�Y���OComment�A�u�O'/'�hŪ�J��X�C
    // 2.�OComment�A�h�M������
    char next2 = cin.peek();
    if ( next2 == '/' ) {  // �J��comment
      KeepTrack( next );        // ����Get�쪺�r����m
      scanf( "%c", &next );
      next = cin.peek() ;
      while ( next != '\n' ) {  // �NcommentŪ���A���촫��
        scanf( "%c", &next );
        next = cin.peek() ;
      } // while

      // KeepTrack( next );    // while�X�Ӭ�'\n'�A���s�p��Line
      // ���j!!�A�קK�h����ѡA�I�s�ۨ�
      GetNextChar( next );
    } // if
  } // if

  if ( !UnrecognizedToken( next ) ) {
   // �P�_�O�_UNrecognized
    return '\0' ;
  } // if

  return  next ;
} // GetNextChar()

Token PeekToken() { // �NToken���NowPeekToken��

  if ( uNowPeekToken.mName != "" ) {
    // �קK�@�}�l�٨SŪ�F��A��ERROR�A�[�WuNowPeekToken.mName != "" ������
    if ( uNowPeekToken.mName != uNowGetToken.mName
         || uNowPeekToken.mColumn != uNowGetToken.mColumn
         || uNowPeekToken.mLine != uNowGetToken.mLine ) {
      // �p�G�S��GetToken�A�h��Peek�sToken
      return uNowPeekToken ;
    } // if 
  } // if


  if ( uErrorLeftTokenTable.size() > 0 ) {
    // ��W����Token���X
    // cout << ulastline << "  " << uLineNum << endl ; 
    if ( usetline == true && ulastline != 0 ) {
    // �b�C�����O���Ĥ@��Token�]�wLineNum
    // �o��Error or �s���O�N�|�p�� 
    // 1.�W���̫᪺Line�ƻP�{�b��Line�Ƥ@�ˡA�N��P�@�榳��ӫ��O�Aline�]��1 
    // 2.���@�˫h�{�blinenum-�W��linenum�۴�
      if ( ulastline == uLineNum ) uLineNum = 1 ; 
      else uLineNum = uLineNum - ulastline ;
      usetline = false ;
    } // if
    // �p��uLineNum  
    Token token = uErrorLeftTokenTable.front() ;
    uErrorLeftTokenTable.erase( uErrorLeftTokenTable.begin() ) ;
    token.mColumn = uColumnNum ;
    token.mLine = uLineNum ;
    uNowPeekToken = token ; // ��Token�s��Buffer��
    return token ;
  } // if
  
  // ---------�H�W���T�{uBeJudgeTokenTable�O�_�٦��F��-----------
  // ��ڤW��GetToken
  // �|�D�ʸ��LWhiteSpace
  // �N���X�Ӫ�Token���Ȧs��Buffer��
  char next = '\0' ;
  char input = SkipWhiteSpaceCommentChar() ;
  // -----------------���L�ťաBComment------------------ 
  Token token ;  // Token�AID���O

  if ( input == '(' || input == ')' || input == '[' || input == ']' || input == '{' || input == '}'
       || input == '+' || input == '-' || input == '*' || input == '/' || input == '%' || input == '^'
       || input == '>' || input == '<' || input == '=' || input == '!' || input == '&' || input == '|'
       || input == ';' || input == ',' || input == '?' || input == ':' ) {
    // ----------�Ÿ����O(SYMBOL)--------------
    token.mType = SYMBOL ;  // �Y�O�S�w�Ÿ����O�A�ݦA���]�w

    if ( input == '<' ) {
      next = cin.peek() ;
      if ( next == '=' ) {       // '<='�BLE
        GetNextChar( next ) ;
        token.mName = "<=" ;
        token.mType = LE ;
      } // if
      else if ( next == '<' ) {  // '<<'�BLS
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
      if ( next == '=' ) {       // '>='�BGE
        GetNextChar( next ) ;
        token.mName = ">=" ;
        token.mType = GE ;
      } // if
      else if ( next == '>' ) {  // '>>'�BRS
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
      if ( next == '=' ) {        // '=='�BEQ
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
      if ( next == '=' ) {         // '!='�BNEQ
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
      if ( next == '&' ) {      // '&&'�BAND
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
      if ( next == '|' ) {      // '||'�BOR
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
      if ( next == '=' ) {       // '+='�BPE
        GetNextChar( next ) ;
        token.mName = "+=" ;
        token.mType = PE ;
      } // if
      else if ( next == '+' ) {   // '++'�BPP
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
      if ( next == '=' ) {      // '-='�BME
        GetNextChar( next ) ;
        token.mName = "-=" ;
        token.mType = ME ;
      } // if
      else if ( next == '-' ) {   // '--'�BMM
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
      if ( next == '=' ) {      // '*='�BTE
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
      if ( next == '=' ) {      // '/='�BDE
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
      if ( next == '=' ) {      // '%='�BRE
        GetNextChar( next ) ;
        token.mName = "%=" ;
        token.mType = RE ;
      } // if
      else {
        token.mName = "%" ;     // '%'
      } // else
    } // else if
    else {               // ��L�Ÿ�
      token.mName = input ;
    } // else

  } // if
  else if ( ( input >= '0' && input <= '9' ) || input == '.' || input == '\'' || input == '"' ) {
    token.mType = CONSTANT ;
    // -----------------Constant---------------------
    token.mName = input ; // �s�J

    if ( input == '.' ) {
    // EX: .aa�B.123�B.1.2�A�I�}�Y
      input = cin.peek() ;
      if ( input < '0' || input > '9' ) {
        // �p���I�᭱���O�Ʀr�AERROR
        UnrecognizedToken( '.' ) ;
      } // if
      else {
        while ( input >= '0' && input <= '9' ) {
        // �I��D�Ʀr���X
          scanf( "%c", &input ) ;
          KeepTrack( input ) ;
          token.mName = token.mName + input ;
          input = cin.peek() ; // �d�ݤU�ӬO�_���Ʀr
        } // while
      } // else
    } // if
    else if ( input == '\'' || input == '"' ) {
    // EX: 'a'�B"Hi, there"      '�B"�}�Y
      if ( input == '\'' ) {
        scanf( "%c", &input ) ;
        if ( input != '\'' ) {
          token.mName = token.mName + input ;  // 'a'
          scanf( "%c", &input ) ;
          KeepTrack( input ) ;
          if ( input != '\'' ) {
          // '�᭱�ĤG�Ӥ��O'�AERROR!
            UnrecognizedToken( '\'' ) ;
          } // if
        } // if
        else {
        // ''�B�S���F��BERROR
          // �d��Unexpected�P�_
        } // else
      } // if
      else {
        input = cin.peek() ;
        while ( input != '"'  ) {     // �I��"���X
          scanf( "%c", &input ) ;
          KeepTrack( input ) ;
          token.mName = token.mName + input ;
          input = cin.peek() ;
        } // while

        scanf( "%c", &input ) ;
        KeepTrack( input ) ;
        token.mName = token.mName + input ;
        token.mType = IDENT ; // "abc" �A�OIDENT Token 
      } // else
    } // else if 
    else {
    // EX: 12.1�B123�A�Ʀr�}�Y
      input = cin.peek() ;
      while ( ( input >= '0' && input <= '9' ) ||  input == '.'  ) {
        // �I��D�Ʀr�Bdot���X
        // Dot�ƶq���D�d��Unexpected�P�_
        scanf( "%c", &input ) ;
        KeepTrack( input ) ;
        token.mName = token.mName + input;
        input = cin.peek() ;     // ���P�_�U�@��Char�O�_���T�bŪ
      } // while
    } // else
  } // else if 
  else if ( ( input >= 'A' && input <= 'Z' ) || ( input >= 'a' && input <= 'z' ) ) {
    token.mType = IDENT ;
    // -----------------------IDENT-------------------------
    // �^��r���}�Y EX: ab123
    token.mName = input ; // �s�J
    input = cin.peek() ;
    while ( ( input >= 'A' && input <= 'Z' ) || ( input >= 'a' && input <= 'z' )
            || ( input >= '0' && input <= '9' ) || input == '_' ) {
      // Ū�줣�O�^��B�Ʀr�B���u����!!
      input = getchar() ;
      KeepTrack( input ) ;
      token.mName = token.mName + input;
      input = cin.peek() ;  // ���P�_�U�@��Char�O�_���T�bŪ
    } // while
  } // else if
  else {
  // Unexpect Error �w�� ���~��Token
  // �N���~��TokenŪ�X��
    token.mName = input ;
    while ( input != ' ' || input != '\t' || input != '\n' || input != EOF ) {
      token.mName = token.mName + next ;
      input = getchar() ;
    } // while
    // Error�Brecognized��Token�AUnexpectedToken
    uErrorToken = token.mName ;
    string error = "UnexpectedToken" ;
    throw error ;
  } // else

  // ------------------�H�U���P�_Unexpected �M Undefined------------------
  if ( !UnexpectedToken( token.mType, token.mName ) ) {
    // Token���H�W����k
  } // if

  token.mColumn = uColumnNum ;
  token.mLine = uLineNum ;
  uNowPeekToken = token ; // ��Token�s��Buffer��
  return token ;

} // PeekToken()

Token GetToken() {

  uNowGetToken = uNowPeekToken ; // Peek��Token���NowGetToken��
  int position = uNowGetToken.mName.find_first_of( "\"" ) ; 
  if ( uNowGetToken.mType == IDENT && uDontcheckDefined == false 
       && uNowGetToken.mName != "cout" && uNowGetToken.mName != "cin" && uNowGetToken.mName != "Done"
       && uNowGetToken.mName != "ListAllVariables" && uNowGetToken.mName != "ListAllFunctions"
       && uNowGetToken.mName != "ListVariable" && uNowGetToken.mName != "ListFunction"
       && position < 0 ) {
    // "abccd"�A�r�ꤣ�ݭn�Q�P�_ 
    // ���O�Q�w�q���A�P�_���LDefined�L
    UndefinedIdentifier( uNowGetToken.mName )  ;
  } // if

  uBeJudgeTokenTable.push_back( uNowGetToken ) ; // ��Get�쪺Token�O���_��
  return uNowGetToken ;
} // GetToken()

void SetNeedDefinedObject( Token token, Funct_id_type type, bool Local ) {
// �N�i��|�Q�w�q���F����Ȧs�_�� 
// �p�G�OLocal == true �N��SetNeedDefinedObject�OFunction�A���F�Ĥ@��token��L�����ϰ��ܼ�
  if ( type == FUNCTIONTYPE && !Local ) {
  // Function
    uNeedDefinedObject.mFunct_Local_Id.push_back( token ) ;
  } // if
  else if ( type == IDENTTYPE && Local ) {
  // Function �� �ϰ��ܼ�
    uNeedDefinedObject.mFunct_Local_Id.push_back( token ) ;
  } // else if
  else if ( type == IDENTTYPE && !Local ) {
  // �����ܼ�
    uNeedDefinedObject.mGlobal_Id.push_back( token ) ;
  } // else if  
  else cout << "Store error" << endl ;

} // SetNeedDefinedObject()

void StoreTrueDefine() {
// ��k���T�A��n�Q�w�q���F����Table��
  bool exist = false ; // �����O�_���w�q
  int address = 0 ; // �O���Q�w�q����m
  if ( uNeedDefinedObject.mType == FUNCTIONTYPE ) {   
  // Function�B�ϰ��ܼ�
    uDetail = Reform() ; // �NFunction���e�Ʀn�A�A���mDetail 
    for ( int i = 0 ; i < uHadDefined_Function_Table.size() ; i++  ) {
      string a = uNeedDefinedObject.mFunct_Local_Id.at( 0 ).mName ;
      if ( a == uHadDefined_Function_Table.at( i ).mFunctionName ) {
        address = i ; // �����w�s�bFunction����m 
        exist = true ;
      } // if 
    } // for
    // -------�P�_Function�O�_�w�Q�w�q�L�A���h���w�q----------
    if ( exist ) {
    // �Q�w�q�LFunction 
      string a = "> New definition of " + uNeedDefinedObject.mFunct_Local_Id.at( 0 ).mName ;
      cout << a + "() entered ..." << endl ;
      a = uNeedDefinedObject.mFunct_Local_Id.at( 0 ).mName ;
      uHadDefined_Function_Table.at( address ).mDetail = uDetail ; // �s���e
      uHadDefined_Function_Table.at( address ).mLocal_Id.clear() ; // �N�ϰ��ܼƪ��ƶq�M��
      for ( int i = 1 ; i < uNeedDefinedObject.mFunct_Local_Id.size() ; i++ ) {
      // �s�ϰ��ܼ�
        a = uNeedDefinedObject.mFunct_Local_Id.at( i ).mName ;
        uHadDefined_Function_Table.at( address ).mLocal_Id.push_back( a ) ;
      } // for
      // ---------------�ϰ��ܼƱƧ�-----------------------
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
      // ---------------�ϰ��ܼƱƧ�--------------------
    } // if
    else {
      // �S���Q�w�q�L
      StoreFunctionType storeFu ;
      storeFu.mFunctionName = uNeedDefinedObject.mFunct_Local_Id.at( 0 ).mName ; // �sFunction�W�r
      storeFu.mDetail = uDetail ; // �s���e 
      cout << "> Definition of " + storeFu.mFunctionName + "() entered ..." << endl ;
      for ( int i = 1 ; i < uNeedDefinedObject.mFunct_Local_Id.size() ; i++ ) {
      // �s�ϰ��ܼơA�Ĥ@�ӬOFunction�A�᭱�~�O�ϰ��ܼ� 
        storeFu.mLocal_Id.push_back( uNeedDefinedObject.mFunct_Local_Id.at( i ).mName ) ;
      } // for
      // ---------------�ϰ��ܼƱƧ�-----------------------
      for ( int i = 0 ; i < storeFu.mLocal_Id.size() ; i++ ) {
        for ( int j = i  ; j < storeFu.mLocal_Id.size() ; j++ ) {
          if ( strcmp( storeFu.mLocal_Id.at( i ).c_str(), storeFu.mLocal_Id.at( j ).c_str() ) > 0 ) {
            string a = storeFu.mLocal_Id.at( j ) ; 
            storeFu.mLocal_Id.at( j ) = storeFu.mLocal_Id.at( i ) ;
            storeFu.mLocal_Id.at( i ) = a ;
          } // if 
        } // for
      } // for
      // ---------------�ϰ��ܼƱƧ�--------------------
      uHadDefined_Function_Table.push_back( storeFu ) ; // �sFunction 
    } // else
    // -------------Function�Ƨ�-----------------------
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
    // -------------Function�Ƨ�-----------------------
  } // if 
  else if ( uNeedDefinedObject.mType == IDENTTYPE ) {
    // �����ܼ�
    cout << "> " ;
    int detailnum = 0 ;
    for ( int j = 0 ; j < uNeedDefinedObject.mGlobal_Id.size() ; j++ ) {
      // ���L�ݭn�Q�w�q�������ܼơA�@�Ӥ@�ӳ]�w
      int address = 0 ; // �����n��i�hmDetail����m 
      exist = false ;  // �Y���Q�w�q�L�A�ݭ��w�q�Aexist�]��true  
      for ( int i = 0 ; i < uHadDefined_Id_Table.size() ; i++  ) {
        string a = uNeedDefinedObject.mGlobal_Id.at( j ).mName ;
        if ( uHadDefined_Id_Table.at( i ).mName == a ) {
          address = i ; // �����Q�w�q�L��ID�bTable�̪���m
          exist = true ;
        } // if
      } // for

      string typeDetail = uBeJudgeTokenTable.at( 0 ).mName ;
      int k = detailnum + 1 ; 
      while ( k < uBeJudgeTokenTable.size() && uBeJudgeTokenTable.at( k ).mName != ";"
              && uBeJudgeTokenTable.at( k ).mName != "," ) {
        // �����w�q�覡�AListVariable�|�Ψ�
        // �I��,�B;�h���X 
        // detailnum�O�����U�ӥ����ܼƪ���m
        if ( uBeJudgeTokenTable.at( k ).mName == "[" || uBeJudgeTokenTable.at( k ).mName == ","  ) 
          typeDetail = typeDetail + uBeJudgeTokenTable.at( k ).mName ;
        else 
          typeDetail = typeDetail + " " + uBeJudgeTokenTable.at( k ).mName ;

        k++ ;
      } // while

      detailnum = k ; // k�X�j��N����m�O;�B,�A�]�wdetailnum�U���A��k+1����
      typeDetail = typeDetail + " ;" ; // �b�̫�[�W���� 

      if ( exist ) {
        // �Q�w�q�L
        string a = uHadDefined_Id_Table.at( address ).mName ;
        uHadDefined_Id_Table.at( address ).mDetail = typeDetail ; // �����w�q�覡�AListVariable�|�Ψ�
        cout << "New definition of " + a + " entered ..." << endl ; 
      } // if
      else {
      // �S���Q�w�q�L
        Token storeId ;
        storeId.mName = uNeedDefinedObject.mGlobal_Id.at( j ).mName ; // �s����W�r
        storeId.mDetail = typeDetail ; // �����w�q�覡�AListVariable�|�Ψ�
        cout << "Definition of " + storeId.mName + " entered ..." << endl ;
        uHadDefined_Id_Table.push_back( storeId ) ; // �sFunction 
      } // else

    } // for

    // ----------------------�����ܼƱƧ�-----------------------
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
    // ------------------------�����ܼƱƧ�----------------------
  } // else if

} // StoreTrueDefine()
// ----------------------GetToken-------------------------

// ----------------------Grammer-------------------------
bool User_input( bool &Done ) {
// user_input : ( definition | statement ) { definition | statement }
  while ( 1 ) {
    // ����^��A���줣�ŦX��k
    if ( Definition() ) {
      // �w�q���T�A�NuNeedDefinedObject���uHadDefinedTokenTable��
      StoreTrueDefine() ; 
      return true ;
    } // if
    else if ( Statement() ) {
      cout << "> " ; 
      if ( uBeJudgeTokenTable.at( 0 ).mName == "Done" && uBeJudgeTokenTable.at( 1 ).mName == "("
           && uBeJudgeTokenTable.at( 2 ).mName == ")" ) {
        // Ū����Done()�A����
        Done = true ;
        return true ;
      } // if
      else {
        if ( uBeJudgeTokenTable.at( 0 ).mName == "ListAllVariables" 
             && uBeJudgeTokenTable.at( 1 ).mName == "("
             && uBeJudgeTokenTable.at( 2 ).mName == ")" ) {
          // Ū����ListAllVariables()
          // �L�X�����ܼƦW�r
          for ( int i = 0 ; i < uHadDefined_Id_Table.size() ; i++ ) {
            cout << uHadDefined_Id_Table.at( i ).mName << endl ;
          } // for 
        } // if
        else if ( uBeJudgeTokenTable.at( 0 ).mName == "ListAllFunctions" 
                  && uBeJudgeTokenTable.at( 1 ).mName == "("
                  && uBeJudgeTokenTable.at( 2 ).mName == ")" ) {
          // Ū����ListAllFunctions()
          // �L�X�Ҧ�Function�W�r�A�B�ƧǹL
          for ( int i = 0 ; i < uHadDefined_Function_Table.size() ; i++ ) {
            cout << uHadDefined_Function_Table.at( i ).mFunctionName << "()" << endl ;
          } // for
        } // else if
        else if ( uBeJudgeTokenTable.at( 0 ).mName == "ListVariable" 
                  && uBeJudgeTokenTable.at( 1 ).mName == "(" ) {
          // Ū����ListVariable("") 
          // �L�X�����ܼơA�W�r�b�ĤT��
          int length = uBeJudgeTokenTable.at( 2 ).mName.length()  ;
          string funtname = uBeJudgeTokenTable.at( 2 ).mName.substr( 1, length - 2 ) ;
          // ��""����
          for ( int i = 0 ; i < uHadDefined_Id_Table.size() ; i++ ) {
            if ( funtname == uHadDefined_Id_Table.at( i ).mName ) {
              cout << uHadDefined_Id_Table.at( i ).mDetail << endl ;
            } // if  
          } // for
        } // else if
        else if ( uBeJudgeTokenTable.at( 0 ).mName == "ListFunction" 
                  && uBeJudgeTokenTable.at( 1 ).mName == "("
                  && uBeJudgeTokenTable.at( 3 ).mName == ")" ) {
          // Ū����ListFunction( "" )�AFunction�W�r�b��2��
          // �L�XFunction���e
          int length = uBeJudgeTokenTable.at( 2 ).mName.length() ;
          string funtname = uBeJudgeTokenTable.at( 2 ).mName.substr( 1, length - 2 ) ;
          // ��""���� 
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
// definition : VOID Identifier function_definition_without_ID      VOID�}�Y�OFunction 
//             | type_specifier Identifier function_definition_or_declarators
//               ��L�}�Y���FFunction�H�~�A�٥i��OID�FString ad[100] ; 
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

    uErrorToken = uNowPeekToken.mName ;   // �@�w��Defintion�o���~ 
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
      // Function or �����ܼ� 
      if ( Function_definition_or_declarators( token ) ) return true ;
    } // if

    uErrorToken = uNowPeekToken.mName ;   // �@�w��Defintion�o���~ 
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
// �Ƕi��firsttoken�O�]�����T�w���OFunction�٬O�����ܼ�
// function_definition_or_declarators : function_definition_without_ID
                                     // | rest_of_declarators
  if ( Function_definition_without_id( firsttoken ) ) {
    // Function
    uNeedDefinedObject.mType = FUNCTIONTYPE ;
    return true ;
  } // if
  else {
    // �����ܼơA�ѩ����]�w��Function�A�ݭn�令Ident���O
    uNeedDefinedObject.mType = IDENTTYPE ;
    SetNeedDefinedObject( firsttoken, IDENTTYPE, false ) ;
    // �N�n�s��Function or ID �Ȧs�_�� ( �n�s�J��Token, ���O, �O�_���ϰ��ܼ� )
    if ( Rest_of_declarators() ) return true ;
  } // else 

  return false ;
} // Function_definition_or_declarators()

bool Rest_of_declarators() {
// rest_of_declarators : [ '[' Constant ']' ] { ',' Identifier [ '[' Constant ']' ] } ';'
// �ϰ��ܼơB�����ܼƳ��|�Ψ즹Function 
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

    // �|�]��o��N��!�A��[CONSTANT]���U��while�S����Peek
    // �G�o��nPeek
    // �p�G�S���i��[CONSTANT]�A�W���N��Peek�L
  } // if

  token = PeekToken() ;
  while ( token.mName == "," ) {
    token = GetToken() ;
    token = PeekToken() ;
    if ( token.mType == IDENT ) {
      uDontcheckDefined = true ; 
      token = GetToken() ;
      uDontcheckDefined = false ;
      // �ϰ��ܼ� or �����ܼ�
      // �N�n�s��Function or ID �Ȧs�_�� ( �n�s�J��Token, ���O, �O�_���ϰ��ܼ� )
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
    uNeedDefinedObject.mType = FUNCTIONTYPE ; // Function���O���w�q 
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
// �o�䬰�ϰ��ܼ� 
  Token token = PeekToken() ;
  if ( Type_specifier( token ) ) {
    token = GetToken() ;
    token = PeekToken() ; // ���i��O& or IDENT 
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
      SetNeedDefinedObject( token, IDENTTYPE, true ) ; // �N�n�s��Function or ID �Ȧs�_�� ( �n�s�J��Token, ���O, �O�_���ϰ��ܼ� )
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
      token = PeekToken() ;    // ���i��O& or IDENT 
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
        // �N�n�s��Function or ID �Ȧs�_�� ( �n�s�J��Token, ���O, �O�_���ϰ��ܼ� )
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
// �o��OFunction�̪���k
// Declaration ���ϰ��ܼ� 
  Token token = PeekToken() ;
  if ( token.mName == "{" ) {
    uNeedDefinedObject.mType = FUNCTIONTYPE ; 
    token = GetToken() ;
    bool out = false ;
    while ( !out ) {
      if ( Declaration() || Statement() ) {
      } // if
      else out = true ; // ������Declaration�BStatement 
    } // while

    token = PeekToken() ;
    if ( token.mName == "}" ) {
      token = GetToken() ;
      return true ;
    } // if
    else {
      // �D�`������Return false ����ӷN��i�ण�ODefintion�A�]�i��O��k���~
      uErrorToken = token.mName ;      // �ĤG��'.'���᪺�r��ǤW�h
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
      // �ϰ��ܼ� 
      SetNeedDefinedObject( token, IDENTTYPE, true ) ; // �N�n�s��Function or ID �Ȧs�_�� ( �n�s�J��Token, ���O, �O�_���ϰ��ܼ� )
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
// �o���P�_ '['�B'('  �A�]��Romce_and_romloe�N�ⳣ�S���]�OTrue 
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

    return true ; // while�� �ݥ�!�P�_�A���諸�ܫhreturn false
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
//  rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp // �Yromce_and_romloe
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
  // bool out = false ; // ���X�j���
  Token token = PeekToken() ;
  while ( token.mName == "*" || token.mName == "/" || token.mName == "%" ) {
    token = GetToken() ;
    if ( !Unary_exp() ) return false ; // ��ӱ���P�ɵo��
    token = PeekToken() ;
  } // while

  return true ; // �z�פW���|�]��o�A����k�n�D�o�g
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
    usetline = true ; // �]�w�@���]��false
    uDetail = "" ;
    uColumnNum = 0 ; 
    uNowGetToken.mName = "" ;
    uNowPeekToken.mName = "" ;
    uBeJudgeTokenTable.clear() ;
    uNeedDefinedObject.mGlobal_Id.clear() ; // �Ȧs�ݭn�Q�w�q�������ܼ� 
    uNeedDefinedObject.mFunct_Local_Id.clear() ; // �Ȧs�ݭn�Q�w�q��Function�ΰϰ��ܼ� 
    uErrorToken = "" ; // �o�Ϳ��~�A�ΨӰO�����~Token
    try {
      if ( User_input( done ) ) {
        if ( uNowPeekToken.mName != uNowGetToken.mName
             || uNowPeekToken.mLine != uNowPeekToken.mLine 
             || uNowPeekToken.mColumn != uNowPeekToken.mColumn ) { 
          // ���hPeek���S��Get�A�h��Peek���F��d��U��
          uErrorLeftTokenTable.push_back( uNowPeekToken ) ;
          // �]�w�W��grammer�̫��m�B���̫�Peek��Token��m
          ulastline = uBeJudgeTokenTable.back().mLine ;
        } // if
        else ulastline = uLineNum ; // ���`�����A�S���hPeek

      } // if
      else {
        // �o����~�O��k�W�����~
        cout << "> Line " << uLineNum << " : unexpected token : '" << uNowPeekToken.mName << "'" << endl ;
        CleanErrorLine();
        ulastline = uLineNum ; // Error!�M�����B�N��S���hPeek
      } // else

    } // try
    catch( string message ) {
      // ���XUnrecognizedToken
      if ( message == "UnexpectedToken" ) {
        // Token�����~ 
        cout << "> Line " << uLineNum << " : unexpected token : '" << uErrorToken << "'" << endl ; 
        CleanErrorLine();
      } // if

      ulastline = uLineNum ; // Error!�M�����B�N��S���hPeek
    } // catch

  } // while

  cout << "Our-C exited ..." ;

} // main()



