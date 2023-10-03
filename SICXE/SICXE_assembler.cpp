#include <string>
#include <iostream>
#include <fstream>   // ifstream, ofstream
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std; 

struct instr {
    string ins ;
    int format ;
    string opcode ;
};

struct regstr {
    string reg ;
    string opcode ;
};

struct table {
    string name ;
    string address ;
};

struct type {
    string hex ;
    string bin ;
} ;

vector<string> errorline ;
vector<string> address ;
vector<string> linebuffer ;
vector<vector<string>> tokenbuffer ;
vector<string> machinecode ;
string infilename ;
vector<type> number = {
{"0", "0000"},
{"1", "0001"},
{"2", "0010"},
{"3", "0011"},
{"4", "0100"},
{"5", "0101"},
{"6", "0110"},
{"7", "0111"},
{"8", "1000"},
{"9", "1001"},
{"A", "1010"},
{"B", "1011"},
{"C", "1100"},
{"D", "1101"},
{"E", "1110"},
{"F", "1111"},
} ; // hex&binary 

/* 
vector<string> table1 ; // instruction
vector<string> table2 ; // pseudo and extra
vector<string> table3 ; // register
vector<string> table4 ; // delimiter
vector<table> SYMTAB ; // symbol、SYMTAB
vector<table> LITTAB ; // literal
*/

// content placed start from 1 
vector<instr>  table1 = {
{      "",  0,   "" },     
{   "ADD", 34, "18" }, 
{  "ADDF", 34, "58" },
{  "ADDR",  2, "90" },
{   "AND", 34, "40" },
{ "CLEAR",  2, "B4" },
{  "COMP", 34, "28" },
{ "COMPF", 34, "88" },
{ "COMPR",  2, "A0" },
{   "DIV", 34, "24" },
{  "DIVF", 34, "64" },
{  "DIVR",  2, "9C" },
{   "FIX",  1, "C4" },
{ "FLOAT",  1, "C0" },
{   "HIO",  1, "F4" },
{     "J", 34, "3C" },
{   "JEQ", 34, "30" },
{   "JGT", 34, "34" },
{   "JLT", 34, "38" },
{  "JSUB", 34, "48" },
{   "LDA", 34, "00" },
{   "LDB", 34, "68" },
{  "LDCH", 34, "50" },
{   "LDF", 34, "70" },
{   "LDL", 34, "08" },
{   "LDS", 34, "6C" },
{   "LDT", 34, "74" },
{   "LDX", 34, "04" },
{   "LDS", 34, "D0" },
{   "MUL", 34, "20" },
{  "MULF", 34, "60" },
{  "MULR",  2, "98" },
{  "NORM",  1, "C8" },
{    "OR", 34, "44" },
{    "RD", 34, "D8" },
{   "RMO",  2, "AC" },
{  "RSUB", 34, "4C" },
{"SHIFTL",  2, "A4" },
{"SHIFTR",  2, "A8" },
{   "SIO",  1, "F0" },
{   "SSK", 34, "EC" },
{   "STA", 34, "0C" },
{   "STB", 34, "78" },
{  "STCH", 34, "54" },
{   "STF", 34, "80" },
{   "STI", 34, "D4" },
{   "STL", 34, "14" },
{   "STS", 34, "7C" },
{  "STSW", 34, "E8" },
{   "STT", 34, "84" },
{   "STX", 34, "10" },
{   "SUB", 34, "1C" },
{  "SUBF", 34, "5C" },
{  "SUBR",  2, "94" },
{   "SVC",  2, "B0" },
{    "TD", 34, "E0" },
{   "TIO",  1, "F8" },
{   "TIX", 34, "2C" },
{  "TIXR",  2, "B8" },
{    "WD", 34, "DC" }
}; // instruction
vector<string> table2 = {   
"",
"START",
"END",
"BYTE",
"WORD",
"RESB",
"RESW",
"EQU",
"BASE",
"LTORG", 
"ORG",
} ; // pseudo and extra
vector<regstr> table3 = { 
{  "",  "" },
{ "A", "0" },
{ "X", "1" },
{ "L", "2" },
{ "B", "3" },
{ "S", "4" },
{ "T", "5" },
{ "F", "6" },
{ "PC", "8" },
{ "SW", "9" }
}; // register
vector<string> table4 = { "", ",", "+", "-", "*", "/", ":", ";", "?", "'", ".", "=", "#", "@" }; // delimiter
// hashtable  range: 0~99
vector<table> SYMTAB ; // symbol、SYMTAB
vector<table> LITTAB ; // literal

bool IsWhiteSpace( char a ) {
    if ( a == '\n' || a == '\t' || a == ' ' ) {
        return true ;
    } // if()
    else return false ;
}  // IsWhiteSpace()

bool IsDelimiter( char a ) {
    bool ans = false ;
    for ( int i = 1 ; i < table4.size() ; i++ ) {
        if ( a == table4[i][0] )  {
            ans = true ;
            break ;
        } // if ()
    } // for()

    return ans ;
} // IsDelimiter()

bool IsDelimiter( string a ) {
    bool ans = false ;
    for ( int i = 1 ; i < table4.size() ; i++ ) {
        if ( a.compare( table4[i]) == 0 )  {
            ans = true ;
            break ;
        } // if ()
    } // for()

    return ans ;
} // IsDelimiter()

bool IsInstruction( string &a, int &index ) {
    bool ans = false ;
    bool allalpha = true ;
    string temp = a ;
    // transfer small alpha to big alpha
    for (  int i = 0 ; i < a.size() && allalpha ; i++ ) {
        if ( a[i] >= 'A' && a[i] <= 'Z' ) ;
        else if ( a[i] >= 'a' && a[i] <= 'z' ) {
            a[i] = a[i] - 32 ;
        } // else if 
        else allalpha = false ; // appear not alpha char
    } // for() 

    if ( allalpha ) {
        for ( int i = 1 ; i < table1.size() ; i++ ) {
            if ( a.size() == table1[i].ins.size() ) {
                if ( a.compare(table1[i].ins) == 0 ) {
                    ans = true ;
                    index = i ;
                    break ;
                } // if()
            } // if () 
        } // for()
    } // if 

    if ( ! ans ) a = temp ;
    return ans ;
} // IsInstruction()

bool IsPseudo( string &a ) {
    bool ans = false ;
    bool allalpha = true ;
    string temp = a ;
    // transfer small alpha to big alpha
    for ( int i = 0 ; i < a.size() && allalpha ; i++ ) {
        if ( a[i] >= 'A' && a[i] <= 'Z' ) ;
        else if ( a[i] >= 'a' && a[i] <= 'z' ) {
            a[i] = a[i] - 32 ;
        } // else if 
        else allalpha = false ; // appear not alpha char
    } // for() 

    if ( allalpha ) {
        for ( int i = 1 ; i < table2.size() ; i++ ) {
            if ( a.size() == table2[i].size() ) {
                if ( a.compare(table2[i]) == 0 ) {
                    ans = true ;
                    break ;
                } // if()
            } // if () 
        } // for()
    } // if()

    if ( ! ans ) a = temp ;
    return ans ;
} // IsPseudo()

bool IsRegister( string &a, int &index ) {
    bool ans = false ;
    bool allalpha = true ;
    string temp = a ;
    // transfer small alpha to big alpha
    for ( int i = 0 ; i < a.size() && allalpha ; i++ ) {
        if ( a[i] >= 'A' && a[i] <= 'Z' ) ;
        else if ( a[i] >= 'a' && a[i] <= 'z' ) {
            a[i] = a[i] - 32 ;
        } // else if 
        else allalpha = false ; // appear not alpha char
    } // for() 

    if ( allalpha ) {
        for ( int i = 1 ; i < table3.size() ; i++ ) {
            if ( a.size() == table3[i].reg.size() ) {
                if ( a.compare(table3[i].reg ) == 0 ) {
                    ans = true ;
                    index = i ;
                    break ;
                } // if()
            } // if () 
        } // for()
    } // if()

    if ( ! ans ) a = temp ;
    return ans ;
} // IsRegister()

bool IsNumber( string a ) {
    bool ans = true ;
    for ( int i = 0 ; i < a.size() ; i++ ) {
        if ( a[i] >= '0' && a[i] <= '9' ) ;
        else {
            ans = false ;
            break ;
        } // else()
    } // for

    return ans ;
} // IsNumber()

bool IsHexnumber( string a ) {
    bool ans = true ;
    for ( int i = 0 ; i < a.size() ; i++ ) {
        if ( a[i] >= '0' && a[i] <= '9' ) ;
        else if ( a[i] >= 'A' && a[i] <= 'F' ) ;
        else if ( a[i] >= 'a' && a[i] <= 'f' ) {
            a[i] = a[i] - 32 ;
        } // 
        else {
            ans = false ;
            break ;
        } // else()
    } // for

    return ans ;
} // IsHexnumber()

bool IsSymbol( string a, int &index ) {
    bool ans = false ;
    for ( int k = 0 ; k < SYMTAB.size() ; k++ ) {
        if ( a.compare( SYMTAB[k].name ) == 0 ) {
            index = k ;
            ans = true ;
            break ;
        } // if
    } // for()

    return ans ;
} // IsSymbol()

int gethvalue( string a ) { // hash value
    int ascii = 0 ;
    for ( int i = 0 ; i < a.size() ; i++ ) {
        ascii = ascii + a[i] ;
    } // for()

    int hvalue = ascii % 100 ;
    return hvalue ;
} // gethvalue()

bool saveIntotable( string s, vector<table> &table, string address, bool reloction  ) {
    int hvalue = gethvalue(s) ;
    bool save = false ;
    bool existed = false ;

    for ( int i = hvalue ; ! save ; ) {

        if ( s.compare( table[i].name ) == 0 ) {
            existed = true ;
            if ( reloction ) {
                table[i].address = address ;
            } // 
            save = true ;
        } // if ()
        else if ( table[i].name.compare( "\0" ) == 0 ) {
            table[i].name = s ;
            table[i].address = address ;
            save = true ;
        } // if()
        else { // liner hashing
            if ( i == table.size() - 1 ) i = 0 ;
            else i++ ;
        } // else 
    } // for()

    return existed ;
} // saveIntotable()

bool Readfile( vector<string> &buffer ) {
    cout << "Pleasae enter filename (ex: abc_input.txt ): " ; 
    cin >> infilename ;
    ifstream infile(infilename.c_str(), ios::in ) ; // open the file

    if ( ! infile.is_open() ) { // file can't open
        cout << "sorry~ This file doesn't exist. Please try again !!" << endl ;
        return false ; 
    } // if 

    string line ; // information of a line
    while ( getline(infile, line) ) {
        buffer.push_back(line);
    } // while
   
    infile.close() ; // close the file
    return true ;
} // Readfile()

void outputfile() {
    fstream outfile ;
    string outfilename ;
    outfilename = infilename.substr(0, infilename.find("_")) ;
    outfilename = outfilename + "_output2.txt" ;
    outfile.open( outfilename.c_str(), ios:: out ) ;
    outfile << setw(5)  << left << setfill(' ') << "Line" ; 
    outfile << setw(15) << left << setfill(' ') << "Location"  ;
    outfile << setw(15) << left << setfill(' ') << "Source code" ;
    outfile << setw(15) << left << setfill(' ') << "Object code" << endl ;
    outfile << "-----------------------------------------------------------------------------" << endl ;
    int line = 5 ;
    for ( int i = 0 ; i < address.size() ; i++, line = line + 5 ) {
        outfile << setw(5)  << left << line << setw(8) << left << setfill(' ')  << address[i]  ;
        outfile << setw(30) << left << linebuffer[i] ;
        outfile << "\t" << machinecode[i] << endl ;
    } // for 

    outfile.close() ;
} // outputfile

void GetToken( vector<vector<string>> &token, vector<string> line  ) { 

    int start, end ; // for token
    string s ;

    for ( int i = 0 ; i < line.size() ; i++ ) {

        for ( int j = 0 ; j < line[i].size() ; j++ ) {
            
            if ( IsDelimiter( line[i][j] ) ) {
                s = s.assign( line[i], j, 1 ) ;
                token[i].push_back(s) ;
            } // if() 

            else if ( ! IsWhiteSpace( line[i][j] ) ) { 
                for ( start = j ; ! IsWhiteSpace( line[i][j] ) && ! IsDelimiter( line[i][j] ) ;) {
                    end = j ;
                    j++ ;
                } // for()
                j-- ;
                
                s = s.assign( line[i], start, end - start + 1 ) ;
                token[i].push_back(s) ;
                    
            } // else if()

        } // for()

    } // for()

} // GetToken()

int HexstrToDecint( string a ) {
    int ans ;
    stringstream ss;
    ss << a;
    ss >> hex >> ans;
    return ans;
} // HexToDec 

int decstrTodecint( string a ) {
    int num ;
    stringstream ss ;
    ss << a ;
    ss >> num ;
    return num ;
} // decstrTodecint()

void decinttohexstr( int a, string &hexstr ) {
    stringstream ss ;
    ss << hex << a ;
    ss >> hexstr ;
    // 小寫轉大寫
    for ( int i = 0 ; i < hexstr.size() ; i++ ) {
        if ( hexstr[i] >= 'a' && hexstr[i] <= 'z' ) {
            hexstr[i] = hexstr[i] - 32 ;
        } // else if 
    } // for()
} // decinttohexstr

void hexbintrans( string &a , int hexbin ) {
    string temp = "\0" ;
    string t ;
    if ( hexbin == 1 ) { // hex to bin 
        for ( int i = 0 ; i < a.size() ; i++ ) {
            t = t.assign( a , i , 1 ) ; 
            for ( int j = 0 ; j < number.size() ; j++ ) {
                if ( t.compare( number[j].hex ) == 0 ) {
                    temp = temp + number[j].bin ;
                    break ;  
                } // if 
            } // for
        } // for 
        a = temp ;
    } // if 

    else if ( hexbin == 2 ) { // bin to hex 
        for ( int i = 0 ; i < a.size() ; i = i+4 ) {
            t = t.assign( a , i , 4 ) ; 
            for ( int j = 0 ; j < number.size() ; j++ ) {
                if ( t.compare( number[j].bin ) == 0 ) {
                    temp = temp + number[j].hex ;
                    break ;  
                } // if 
            } // for
        } // for 
        a = temp ;
    } // else if 

} // hexbintrans

void hexcalculate( string a, string b, string &ans ) {
    // 輸入a, b為16進位字串，輸出ans為16進位字串
    // int maxsize = a.size() ;
    if ( a.size() > b.size() ) {
        for ( int i = a.size() - b.size() ; i >= 0 ; i-- ) {
            b = "0" + b ; 
        } // for
    } // if 
    else if ( a.size() < b.size() ) {
        // maxsize = b.size() ;
        for ( int i = b.size() - a.size() ; i >= 0 ; i-- ) {
            a = "0" + a ; 
        } // for
    } // else if 

    stringstream s1, s2 ;
    int op1, op2 ;
    s1 << a ;          // 輸入16進位的字串
    s1 >> hex >> op1 ; // 輸出16進位轉成10進位的值
    
    s2 << b ;
    s2 >> hex >> op2 ;

    op1 = op1 - op2 ;
    if (  op1 < 0 ) {
        op1 = -op1 ; // 先看正數的部分
        decinttohexstr( op1, ans ) ; // 10進位的數字轉成16進位的字串
        hexbintrans( ans , 1 ) ; // 16進位的字串轉成2進位的字串
        for ( int i = 0 ; i < ans.size() ; i++ ) { // 1's compliment
            if ( ans[i] == '1') ans[i] = '0' ;
            else if ( ans[i] == '0') ans[i] = '1' ;
        } // for()

        hexbintrans( ans , 2 ) ; // 2進位的字串轉成16進位的字串
        int twocompliment = HexstrToDecint(ans) + 1 ; // 16進位的字串轉成10進位的數字然後加1
        decinttohexstr( twocompliment, ans ) ; // 10進位的數字轉成16進位的字串        
        while ( ans.size() < 3 ) {
            ans = "F" + ans ;
        } // while

    } // if
    else {
    
        decinttohexstr( op1, ans ) ; // 10進位的數字轉成16進位的字串
        while ( ans.size() < 3 ) {
            ans = "0" + ans ;
        } // while
    } // 

} // hexsub()

void XElocation( vector<string> &address, vector<vector<string>> tokenbuffer  ) {
    bool relocation = false ;
    // start address 把address補齊4位
    address[0] = tokenbuffer[0][2] ;
    while( address[0].size() < 4 ) {
        address[0] = "0" + address[0] ;
    } // while

    address[1] = address[0] ;
    bool error = false ;
    int index = 0 ;
    int LOCCTR = HexstrToDecint( tokenbuffer[0][2] ) ; // 將16進位字串轉成10進位數字
    string temp ;
    int num = 0 ;

    for ( int i = 1 ; i < tokenbuffer.size() ; i++ ) {
        error = false ;
        for  ( int j = 0 ; j < tokenbuffer[i].size() ; j++ ) {
            index = 0 ;
            temp = tokenbuffer[i][j] ;
            // cout << temp << endl ;
            // cout << temp << endl ;

            if ( temp.compare( "+" ) == 0 ) LOCCTR = LOCCTR + 1 ;
            else if ( temp.compare( "." ) == 0 ) break ;
   
            if ( IsPseudo(temp) ) {

                if ( temp.compare( "WORD" ) == 0 ) 
                    LOCCTR = LOCCTR + 3 ;

                else if ( temp.compare( "BYTE" ) == 0 ) {

                    if ( tokenbuffer[i][j+1].compare( "X" ) == 0  ) 
                        LOCCTR = LOCCTR + tokenbuffer[i][j+3].size() / 2 ;

                    else if ( tokenbuffer[i][j+1].compare( "C" ) == 0  ) 
                        LOCCTR = LOCCTR + tokenbuffer[i][j+3].size() ;

                } // else if 

                else if ( temp.compare( "RESW" ) == 0 ) {

                    num = decstrTodecint( tokenbuffer[i][j+1] ) ;
                    LOCCTR = LOCCTR + num * 3 ;
                    
                } // else if ()

                else if ( temp.compare( "RESB" ) == 0 ) {

                    num = decstrTodecint( tokenbuffer[i][j+1] ) ;
                    LOCCTR = LOCCTR + num ;

                } // else if ()

                else if ( temp.compare( "LTORG" ) == 0 || temp.compare( "END" ) == 0 ) { // literal table
                    for ( int k = 0 ; k < LITTAB.size() ; k++ ) {
                        if ( LITTAB[k].name.compare( "\0" ) != 0 ) {
                            if ( LITTAB[k].address.compare( "\0" ) == 0 ) {
                                int length = 0 ;
                                
                                length = LITTAB[k].name.size() ;
                                if ( LITTAB[k].name[0] == 'X' || LITTAB[k].name[0] == 'x' ) {
                                    length = ( length - 3 ) / 2 ;
                                } // if
                                else if ( LITTAB[k].name[0] == 'C' || LITTAB[k].name[0] == 'c' ) {
                                    length = length - 3  ;
                                } // else if  
                                else {
                                    length = 3 ;
                                } // else

                                LOCCTR = LOCCTR + length ;
                                decinttohexstr( LOCCTR, temp ) ; // 將10進位數字轉成16進位字串
                                //把address補齊4位
                                while( temp.size() < 4 ) {
                                    temp = "0" + temp ;
                                } // while
                                LITTAB[k].address = temp ;

                            } // if  
                        }  // if 

                    } // for
                } // else  

                else if ( temp.compare( "ORG" ) == 0 ) {
                    j = j+1 ;
                    int tokenmum = tokenbuffer[i].size() - j ; 
                    if ( tokenmum == 1 ) {
                        if ( IsNumber( tokenbuffer[i][j] ) ) { // 數字
                            LOCCTR = decstrTodecint( tokenbuffer[i][j] ) ;
                            
                        } // 
                        else if ( IsSymbol( tokenbuffer[i][j], index ) ) { // symbol
                            LOCCTR = HexstrToDecint( SYMTAB[index].address ) ;
                            cout << "LO : " << LOCCTR << endl ;
                        } // else if 
                    } // if 
                    else { // 數字 lable加減乘除的
                        
                    } // 
                    break ;

                } // else if 

            } // else if() 

            else if ( IsInstruction(temp, index) ) {

                if ( table1[index].format == 1 ) LOCCTR = LOCCTR + 1 ;
                else if ( table1[index].format ==  2  ) LOCCTR = LOCCTR + 2 ;
                else LOCCTR = LOCCTR + 3 ;

            } // if
            
            else if ( temp.compare( "=" ) == 0 ) { // literal

                // 把等號後面的token全部連在一起
                string t = "\0" ;
                for ( j = j+1 ; j < tokenbuffer[i].size() ; j++ ) {
                    t = t + tokenbuffer[i][j] ;
                } // for()

                // literal 放入 literal table
                table literal ;
                literal.name = t ;
                // 避免duplicate literal 
                bool existed = false ;
                for ( int m = 0 ; m < LITTAB.size() ; m++ ) {
                    if ( t.compare( LITTAB[m].name ) == 0 ) {
                        existed = true ;
                        break ;  
                    } // if 
                } // for 
                if ( ! existed ) LITTAB.push_back( literal ) ;

            } // else if 
            else if ( IsDelimiter( temp ) ) ;
            else if ( IsNumber(temp) ) ;
            else if ( IsRegister(temp, index) ) ; 
            else { // symbol
                if ( j == 0 ) {
                    if ( saveIntotable( temp, SYMTAB, address[i], relocation ) ) {
                        cout << "error : dupilcate define   " << endl ;
                        error = true ;
                        errorline[i] = "error" ;
                        break ;
                    } // if 
                } // if 
            } // else 
        } // for
        
        if ( i+1 < tokenbuffer.size() && ! error ) {
            string hexstr ;
            decinttohexstr( LOCCTR, hexstr ) ; // 將10進位數字轉成16進位字串

            //把address補齊4位
            while( hexstr.size() < 4 ) {
                hexstr = "0" + hexstr ;
            } // 
            address[i+1] = hexstr ;

        } // if 
        else if ( i+1 < tokenbuffer.size() && error ) {
            address[i+1] = address[i] ;
            address[i] = "\0" ;
        } // else if

    } // for 

} // XElocation()

void XErelocation( int line, int startaddress ) {
    
    bool relocation = true ;
    int LOCCTR = startaddress ;
    int index ;
    string temp ;
    int num = 0 ;

    for ( int i = line ; i < tokenbuffer.size() ; i++ ) {
        if ( errorline[i].compare( "error" ) != 0 ) {
            for  ( int j = 0 ; j < tokenbuffer[i].size() ; j++ ) {
                index = 0 ;
                temp = tokenbuffer[i][j] ;

                if ( temp.compare( "+" ) == 0 ) LOCCTR = LOCCTR + 1 ;
                else if ( temp.compare( "." ) == 0 ) break ;
    
                if ( IsPseudo(temp) ) {

                    if ( temp.compare( "WORD" ) == 0 ) 
                        LOCCTR = LOCCTR + 3 ;

                    else if ( temp.compare( "BYTE" ) == 0 ) {

                        if ( tokenbuffer[i][j+1].compare( "X" ) == 0  ) 
                            LOCCTR = LOCCTR + tokenbuffer[i][j+3].size() / 2 ;

                        else if ( tokenbuffer[i][j+1].compare( "C" ) == 0  ) 
                            LOCCTR = LOCCTR + tokenbuffer[i][j+3].size() ;

                    } // else if 

                    else if ( temp.compare( "RESW" ) == 0 ) {

                        num = decstrTodecint( tokenbuffer[i][j+1] ) ;
                        LOCCTR = LOCCTR + num * 3 ;
                        
                    } // else if ()

                    else if ( temp.compare( "RESB" ) == 0 ) {

                        num = decstrTodecint( tokenbuffer[i][j+1] ) ;
                        LOCCTR = LOCCTR + num ;

                    } // else if ()

                    else if ( temp.compare( "LTORG" ) == 0 || temp.compare( "END" ) == 0 ) { // literal table
                        for ( int k = 0 ; k < LITTAB.size() ; k++ ) {
                            if ( LITTAB[k].name.compare( "\0" ) != 0 ) {
                                int ltaddress = HexstrToDecint( LITTAB[k].address ) ;
                                if ( ltaddress > startaddress ) {
                                    int length = 0 ;
                                    length = LITTAB[k].name.size() ;
                                    if ( LITTAB[k].name[0] == 'X' || LITTAB[k].name[0] == 'x' ) {
                                        length = ( length - 3 ) / 2 ;
                                    } // if
                                    else if ( LITTAB[k].name[0] == 'C' || LITTAB[k].name[0] == 'c' ) {
                                        length = length - 3  ;
                                    } // else if  
                                    else {
                                        length = 3 ;
                                    } // else

                                    LOCCTR = LOCCTR + length ;
                                    decinttohexstr( LOCCTR, temp ) ; // 將10進位數字轉成16進位字串
                                    //把address補齊4位
                                    while( temp.size() < 4 ) {
                                        temp = "0" + temp ;
                                    } // while
                                    LITTAB[k].address = temp ;

                                } // if  
                            }  // if 

                        } // for
                    } // else  

                    else if ( temp.compare( "ORG" ) == 0 ) {
                        j = j+1 ;
                        int tokenmum = tokenbuffer[i].size() - j ; 
                        if ( tokenmum == 1 ) {
                            if ( IsNumber( tokenbuffer[i][j] ) ) { // 數字
                                LOCCTR = decstrTodecint( tokenbuffer[i][j] ) ;
                            } // 
                            else if ( IsSymbol( tokenbuffer[i][j], index ) ) { // symbol
                                LOCCTR = HexstrToDecint( SYMTAB[index].address ) ;
                            } // else if 
                        } // if 
                        else { // 數字 lable加減乘除的
                            
                        } // 
                        break ;

                    } // else if 

                } // else if() 

                else if ( IsInstruction(temp, index) ) {

                    if ( table1[index].format == 1 ) LOCCTR = LOCCTR + 1 ;
                    else if ( table1[index].format ==  2  ) LOCCTR = LOCCTR + 2 ;
                    else LOCCTR = LOCCTR + 3 ;

                } // if
                
                else if ( temp.compare( "=" ) == 0 ) { // literal

                    // 把等號後面的token全部連在一起
                    string t = "\0" ;
                    for ( j = j+1 ; j < tokenbuffer[i].size() ; j++ ) {
                        t = t + tokenbuffer[i][j] ;
                    } // for()

                    // literal 放入 literal table
                    table literal ;
                    literal.name = t ;
                    // 避免duplicate literal 
                    bool existed = false ;
                    for ( int m = 0 ; m < LITTAB.size() ; m++ ) {
                        if ( t.compare( LITTAB[m].name ) == 0 ) {
                            existed = true ;
                            break ;  
                        } // if 
                    } // for 
                    if ( ! existed ) LITTAB.push_back( literal ) ;

                } // else if 
                else if ( IsDelimiter( temp ) ) ;
                else if ( IsNumber(temp) ) ;
                else if ( IsRegister(temp, index) ) ; 
                else { // symbol
                    if ( j == 0 ) {
                        saveIntotable( temp, SYMTAB, address[i], relocation ) ;
                    } // if 
            
                } // else 
            } // for
        } // 

        if ( i+1 < tokenbuffer.size() && errorline[i].compare( "error" ) != 0 ) {
            string hexstr ;
            decinttohexstr( LOCCTR, hexstr ) ; // 將10進位數字轉成16進位字串

            //把address補齊4位
            while( hexstr.size() < 4 ) {
                hexstr = "0" + hexstr ;
            } // while
            address[i+1] = hexstr ;

        } // if 
        else if ( i+1 < tokenbuffer.size() && errorline[i].compare( "error" ) == 0 ) {
            address[i+1] = address[i] ;
            address[i] = "\0" ;
        } // else if

    } // for 
} // XErelocation()

void format2( vector<string> instr, int instrline, int instrindex ) {
    string temp = table1[instrindex].opcode ;
    int rindex = 0 ;
    for ( int i = 0 ; i < instr.size() ; i++ ) {

        if ( IsRegister( instr[i], rindex ) ) 
            temp = temp + table3[rindex].opcode ;

    } // for 

    if ( temp.size() == 3 ) temp = temp + "0" ;

    machinecode[instrline] = temp ;
} // format2()

bool format3( vector<string> instr, int instrline, int instrindex, string pc, string base ) {
    // cout << "instr : " << table1[instrindex].ins << endl ;
    bool status = true ; // 判斷使用 pc or base 有沒有 error -> false 
    string opcode = table1[instrindex].opcode ; // 取得指令的opcode
    string temp = "\0" ;
    hexbintrans( opcode, 1 ) ; // hex to bin
    
    string op6bit ;
    op6bit = op6bit.assign( opcode, 0 , 6 ) ; // 取前6bit

    string nixbpe = "110000" ;
    string disp = "\0" ;
    int index = 0 ;

    for ( int i = 0 ; i < instr.size() ; i++ ) {

        if ( instr[i].compare( "X" ) == 0 ) {
            nixbpe[2] = '1' ; // X = 1
        } // if()

        else if ( instr[i].compare( "#" ) == 0 ) { // immediate addressing 
            nixbpe[0] = '0' ; // n = 0 
            nixbpe[1] = '1' ; // i = 1
            i++ ;

            if ( IsNumber( instr[i] ) )  {
                int immediate = decstrTodecint( instr[i] ) ;
                decinttohexstr( immediate , disp ) ;
                while( disp.size() < 3 ) {
                    disp = "0" + disp ;
                } // while()
                hexbintrans( disp, 1 ) ; // hex to bin
            } // if 
            else if ( IsSymbol( instr[i], index ) ) {
                
                int offset = HexstrToDecint( SYMTAB[index].address ) -  HexstrToDecint( pc ) ;
                // cout << "offset" << offset << endl ;
                if ( offset >= -2048 && offset <= 2047 ) {
                    nixbpe[4] = '1' ; // p = 1
                    hexcalculate( SYMTAB[index].address, pc, disp ) ; // 用pc計算disp
                    while( disp.size() < 3 ) {
                        disp = "0" + disp ;
                    } // while()
                    hexbintrans( disp, 1 ) ; // hex to bin
                } // if 
                else if ( base.compare( "\0" ) != 0 ) { // 有base register可用
                    hexcalculate( SYMTAB[index].address, base, disp ) ; // 用base計算disp
                    nixbpe[3] = '1' ; // b = 1
                    while( disp.size() < 3 ) {
                        disp = "0" + disp ;
                    } // while()
                    hexbintrans( disp, 1 ) ; // hex to bin
                } // else if ()
                else {
                    cout << "error, no base register" << endl ;    
                    status = false ;
                } // else 

            } // else if  
            break ;
        } // if 

        else if ( instr[i].compare( "@" ) == 0 ) { // inderect addressing 
            i++ ;
            nixbpe[0] = '1' ; // n = 1 
            nixbpe[1] = '0' ; // i = 0

            if ( IsSymbol( instr[i], index ) ) {

                int offset = HexstrToDecint( SYMTAB[index].address ) -  HexstrToDecint( pc ) ;
                // cout << "offset" << offset << endl ;
                if ( offset >= -2048 && offset <= 2047 ) {
                    nixbpe[4] = '1' ; // p = 1
                    hexcalculate( SYMTAB[index].address, pc, disp ) ; // 用pc計算disp
                    while( disp.size() < 3 ) {
                        disp = "0" + disp ;
                    } // while()
                    hexbintrans( disp, 1 ) ; // hex to bin
                } // if 

                else if ( base.compare( "\0" ) != 0 ) { // 有base register可用
                    hexcalculate( SYMTAB[index].address, base, disp ) ; // 用base計算disp
                    nixbpe[3] = '1' ; // b = 1
                    while( disp.size() < 3 ) {
                        disp = "0" + disp ;
                    } // while()
                    hexbintrans( disp, 1 ) ; // hex to bin
                } // else if ()

                else {
                    cout << "error, no base register" << endl ;    
                    status = false ;
                } // else 
                    
            } // if 
            break ;
        } // else if

        else { // directing addressing or simple addressing  
            nixbpe[0] = '1' ; // n = 1 
            nixbpe[1] = '1' ; // i = 1
            int index = 0 ;
            
            if ( IsSymbol( instr[i], index ) ) {

                int offset = HexstrToDecint( SYMTAB[index].address ) - HexstrToDecint( pc ) ;
                // cout << "offset : " << offset << endl ;
                if ( offset >= -2048 && offset <= 2047 ) {
                    nixbpe[4] = '1' ; // p = 1
                    hexcalculate( SYMTAB[index].address, pc, disp ) ; // 用pc計算disp
                    cout << "disp : " << disp << endl ;
                    while( disp.size() < 3 ) {
                        disp = "0" + disp ;
                    } // while()
                    hexbintrans( disp, 1 ) ; // hex to bin
                } // if 

                else if ( base.compare( "\0" ) != 0 ) { // 有base register可用
                    hexcalculate( SYMTAB[index].address, base, disp ) ; // 用base計算disp
                    nixbpe[3] = '1' ; // b = 1
                    while( disp.size() < 3 ) {
                        disp = "0" + disp ;
                    } // while()
                    
                    hexbintrans( disp, 1 ) ; // hex to bin
                    
                } // else if ()

                else {
                    cout << "error, no base register" << endl ;    
                    status = false ;
                } // else 
                    
            } // if 
            else if ( IsHexnumber( instr[i] ) ) { // address ex: comp 0  0(hex)
                nixbpe[0] = '1' ; // n = 0 
                nixbpe[1] = '1' ; // i = 0
                disp = instr[i] ;
                while( disp.size() < 3 ) {
                    disp = "0" + disp ;
                } // while()
                hexbintrans( disp, 1 ) ; // hex to bin
            } // else
            
        } // else()
    } // for ()
    
    
    temp = op6bit + nixbpe + disp ;
    hexbintrans( temp, 2 ) ; // bin to hex
    if ( opcode.compare( "01001100" ) == 0 ) { // rsub
        while ( temp.size() < 6 ) { // rsub
            temp = temp + "0" ;
        } // while 
    } // if
    
    if ( status ) machinecode[instrline] = temp ;

    return status ;
} // format3()

void format4( vector<string> instr, int instrline, int instrindex ) {
    
    string opcode = table1[instrindex].opcode ; // 取得指令的opcode
    string temp = "\0" ;
    hexbintrans( opcode, 1 ) ; // hex to bin
    
    string op6bit ;
    op6bit = op6bit.assign( opcode, 0 , 6 ) ; // 取前6bit
   
    string address ;
    string nixbpe = "110001" ;

    for ( int i = 0 ; i < instr.size() ; i++ ) {

        if ( instr[i].compare( "X" ) == 0 ) {
            nixbpe[2] = '1' ; // X = 1
        } // if()

        else if ( instr[i].compare( "#" ) == 0 ) { // immediate addressing 
            nixbpe[0] = '0' ; // n = 0 
            nixbpe[1] = '1' ; // i = 1
            i++ ;
            int immediate = decstrTodecint( instr[i] ) ;
            decinttohexstr( immediate , address ) ;
            while( address.size() < 5 ) {
                address = "0" + address ;
            } // while()
            hexbintrans( address, 1 ) ; // hex to bin
        } // if 

        else if ( instr[i].compare( "@" ) == 0 ) { // inderect addressing 
            i++ ;
            nixbpe[0] = '1' ; // n = 1 
            nixbpe[1] = '0' ; // i = 0
            
            for ( int k = 0 ; k < SYMTAB.size() ; k++ ) {
                if ( instr[i].compare( SYMTAB[k].name) == 0 ) {
                    address = SYMTAB[k].address ;
                    while( address.size() < 5 ) {
                        address = "0" + address ;
                    } // while()
                    hexbintrans( address, 1 ) ; // hex to bin
                    break ;
                } // if
            } // for()

        } // else if

        else { // directing addressing or simple addressing  
            nixbpe[0] = '1' ; // n = 1 
            nixbpe[1] = '1' ; // i = 1
            int index = 0 ;
            
            if ( IsSymbol( instr[i], index ) ) {
                address = SYMTAB[index].address ;
                while( address.size() < 5 ) {
                    address = "0" + address ;
                } // while()
                hexbintrans( address, 1 ) ; // hex to bin
            } // else if 
            else if ( IsHexnumber( instr[i] ) ) { // address ex: comp 0  0(hex)
                nixbpe[0] = '1' ; // n = 0 
                nixbpe[1] = '1' ; // i = 0
                address = instr[i] ;
                while( address.size() < 5 ) {
                    address = "0" + address ;
                } // while()
                hexbintrans( address, 1 ) ; // hex to bin
            } // else

        } // else()

    } // for ()

    temp = op6bit + nixbpe + address ;
    hexbintrans( temp, 2 ) ; // bin to hex
    if ( opcode.compare( "01001100" ) == 0 ) { // rsub
        while ( temp.size() < 6 ) { // rsub
            temp = temp + "0" ;
        } // while 
    } // if
    machinecode[instrline] = temp ;
    
    
} // format4()

bool XEpass1() {
    bool exesuccess = true ;
    if ( Readfile( linebuffer ) ) {
        // 解決空白行的問題
        vector<string> temp ;
        for ( int i = 0 ; i < linebuffer.size() ; i++ ) {
            if ( linebuffer[i].size() != 0 ) temp.push_back( linebuffer[i] ) ;
        } // for 
        linebuffer.clear() ; 
        linebuffer = temp ;

        address.resize( linebuffer.size() ) ;
        errorline.resize( linebuffer.size() ) ;
        tokenbuffer.resize( linebuffer.size() ) ;

        GetToken( tokenbuffer, linebuffer ) ;
        XElocation( address, tokenbuffer ) ;
    } // 
    else exesuccess = false ;

    return exesuccess ;
} // pass1() 

bool checkundefined( vector<string> instr  ) {

    bool defined = true ; 
    int index = 0 ;
    for ( int i = 0 ; i < instr.size() ; i++ ) {
        if ( IsInstruction( instr[i], index ) );
        else if ( IsPseudo( instr[i] ) ) ;
        /*
        else if ( instr[i].compare( "=" ) == 0 ) {
            // 把等號後面的token全部連在一起
            string t = "\0" ;
            for ( i = i+1 ; i < instr[i].size() ; i++ ) {
                t = t + instr[i] ;
            } // for()
            bool find = false ;
            for ( int k = 0 ; k < LITTAB.size() && ! find ; k++ ) {
                if ( t.compare( LITTAB[k].name ) == 0 ) find = true ;
            } // for 
            if ( ! find ) defined = false ;
        } // else 
        */
        else if ( IsRegister( instr[i], index ) ) ;
        else if ( IsDelimiter( instr[i] ) ) ;
        else if ( IsNumber( instr[i] ) ) ;
        else if ( IsHexnumber( instr[i] ) ) ;
        else if ( IsSymbol( instr[i], index ) ) ;
        else if ( instr[i].compare( "." ) == 0 ) {
            cout << instr[i] << endl ;
            i = instr.size() ;
        } // if ()
        else if ( instr[i].compare( "END" ) == 0 ) i = instr.size() ;
        else {
            cout << "!!!!" << instr[i] << endl ;
            defined = false ;
        } //else 
    } // for 

    if ( ! defined ) {
        cout << "error : undefined symbol " << endl ;
        for ( int i = 0 ; i < instr.size() ; i++ ) {
            if ( IsSymbol( instr[i], index ) ) {
                SYMTAB[index].name = "\0" ;
                SYMTAB[index].address = "\0" ;
            } // if 
        } // for 
    } // if ()

    return defined ;
} // checkundefined

void XEpass2() {

    string pc = "\0" ; // 指向下一道指令的位址
    string base = "\0" ; // base register
    machinecode.resize( address.size() ) ;
    int index ; // 代表token在某個table的位子
    string temp ;

    for ( int i = 1 ; i < tokenbuffer.size() ; i++ ) {
        index = 0 ;
        // 設定PC
        if ( i + 1 < tokenbuffer.size() ) {
            bool get = false ;
            int g = 0 ;
            for ( g = i+1 ; g < tokenbuffer.size() && ! get ;  ) {
                if ( errorline[g].compare( "error" ) == 0 ) g++ ;
                else if ( tokenbuffer[g][0].compare( "." ) == 0 ) g++ ;
                // else if ( tokenbuffer[g][0].compare( "END" ) == 0 ) ;
                // else if ( tokenbuffer[g].size() > 1 && tokenbuffer[g][1].compare( "END" ) == 0 ) ;
                else get = true ;
            } // 

            if ( g < tokenbuffer.size() &&  get ) pc = address[g] ;
            else {
                cout << " error " << endl ; // 找不到pc 
            } // else 
        } // 
            
        // cout << "pc : " << pc << endl ;
        if ( errorline[i].compare( "error" ) != 0 ) {

            vector<string> instr;

            for ( int m = 0 ; m < tokenbuffer[i].size() ; m++ ) {
                instr.push_back( tokenbuffer[i][m] ) ;
            } // for 

            if ( checkundefined( instr )  ) {
    
                    for ( int j = 0 ; j < tokenbuffer[i].size() ; j++ ) {

                        temp = tokenbuffer[i][j] ;
                        // cout << "temp" << temp << endl ;
                        if ( temp.compare( "." ) == 0 ) {
                            address[i] = "\0" ;
                            break ;
                        } // 
                        else if ( IsInstruction(temp, index) ) {

                            if ( table1[index].format == 1 ) {
                                machinecode[i] = table1[index].opcode ;
                                break ;
                            } // if ()

                            else if ( table1[index].format == 2 ) {

                                vector<string> instr ;
                                for ( int k = j+1 ; k < tokenbuffer[i].size() ; k++ ) {
                                    instr.push_back( tokenbuffer[i][k] ) ;
                                } // for()

                                format2 ( instr, i, index ) ;
                                break ;

                            } // else if()

                            else if ( table1[index].format == 34 ) {
                                
                                bool find = false ;
                                // 看目前這行指令有沒有+出現
                                for ( int k = 0 ; k < tokenbuffer[i].size() && ! find ; k++ ) {
                                    if ( tokenbuffer[i][k].compare( "+" ) == 0 ) find = true ;
                                } // for()

                                vector<string> instr ;
                                for ( int k = j+1 ; k < tokenbuffer[i].size() ; k++ ) {
                                    instr.push_back( tokenbuffer[i][k] ) ;
                                } // for()
                                
                                if ( find ) {
                                    format4( instr, i, index ) ;
                                } // 
                                else {
                                    format3( instr, i, index, pc, base ) ;
                                } // else 

                                break ;
                            } // else if 
                        } // if 

                        else if ( IsPseudo( temp ) ) {
                            if ( temp.compare( "EQU" ) == 0 ) {
                                j++ ;
                                temp = tokenbuffer[i][j] ;
                                int tokenmum = tokenbuffer[i].size() - j ; 
                                if( tokenmum == 1 ) {
                                    if ( IsNumber( temp ) ) { // 數字

                                        int value = decstrTodecint( temp ) ;
                                        decinttohexstr( value, temp ) ;
                                        while( temp.size() < 4 ) {
                                            temp = "0" + temp ;
                                        } // while
                                        address[i] = temp ;

                                        // 修改這個token在symtab的address
                                        if ( IsSymbol( tokenbuffer[i][j-2], index ) ) { 
                                            SYMTAB[index].address = temp ;
                                        } // if 

                                    } // if 
                                    
                                    else if ( IsSymbol( temp , index ) ) { // symbol
                                        string addr = SYMTAB[index].address ;
                                        address[i] = addr ;
                                        // 修改這個token在symtab的address
                                        if ( IsSymbol( tokenbuffer[i][j-2], index ) ) { 
                                            SYMTAB[index].address = addr ;
                                        } // if 

                                    } // else if ()
                                } // if 
                                else { // LABEL +-*/

                                } // 
                            } // if 

                            else if ( temp.compare( "WORD" ) == 0 ) {
                                j = j + 1 ; // 取下一個TOKEN

                                if ( IsNumber( tokenbuffer[i][j] ) ) {
                                    int value = decstrTodecint( tokenbuffer[i][j] ) ;
                                    decinttohexstr( value, temp ) ;
                                    while( temp.size() < 6 ) {
                                        temp = "0" + temp ;
                                    } // 
                                    machinecode[i] = temp ;
                                } // if
                                else if ( tokenbuffer[i][j].compare( "X" ) == 0 || tokenbuffer[i][j].compare( "x" ) == 0 ) {
                                    j = j + 2 ;
                                    machinecode[i] = tokenbuffer[i][j] ;
                                } // if ()
                                else if ( tokenbuffer[i][j].compare( "C" ) == 0 || tokenbuffer[i][j].compare( "c" ) == 0 ) {
                                    j = j + 2 ;
                                    string s = "\0" ;
                                    for ( int k = 0 ; k < tokenbuffer[i][j].size()  ; k++ ) {
                                        int ascii = tokenbuffer[i][j][k] ; // 取得char的ascii
                                        string  hstrascii  ;
                                        decinttohexstr( ascii, hstrascii ) ;
                                        s = s + hstrascii ; 
                                    } // for 
                                    machinecode[i] = s ;
                                } // else if 

                            } // else if 

                            else if ( temp.compare( "BYTE" ) == 0 ) {

                                j = j + 1 ; // 取下一個TOKEN

                                if ( IsNumber( tokenbuffer[i][j] ) ) {
                                    int value = decstrTodecint( tokenbuffer[i][j] ) ;
                                    decinttohexstr( value, temp ) ;
                                    while( temp.size() < 6 ) {
                                        temp = "0" + temp ;
                                    } // 
                                    machinecode[i] = temp ;
                                } // if
                                else if ( tokenbuffer[i][j].compare( "X" ) == 0 || tokenbuffer[i][j].compare( "x" ) == 0 ) {
                                    j = j + 2 ;
                                    machinecode[i] = tokenbuffer[i][j] ;
                                } // if ()
                                else if ( tokenbuffer[i][j].compare( "C" ) == 0 || tokenbuffer[i][j].compare( "c" ) == 0 ) {
                                    j = j + 2 ;
                                    string s = "\0" ;
                                    for ( int k = 0 ; k < tokenbuffer[i][j].size()  ; k++ ) {
                                        int ascii = tokenbuffer[i][j][k] ; // 取得char的ascii
                                        string  hstrascii  ;
                                        decinttohexstr( ascii, hstrascii ) ;
                                        s = s + hstrascii ; 
                                    } // for 
                                    machinecode[i] = s ;
                                } // else if 
                                
                            } // else if 

                            else if ( temp.compare( "BASE" ) == 0 ) { 
                                address[i] = "\0" ; // base不需要ADDRESS
                                j = j + 1 ; 
                                if ( IsNumber( tokenbuffer[i][j]) ) {
                                    int basenum = decstrTodecint( tokenbuffer[i][j] ) ;
                                    decinttohexstr( basenum, temp ) ;
                                    while( temp.size() < 4 ) {
                                        temp = "0" + temp ; 
                                    } // while()

                                    base = temp ;
                                } // if ()
                                else if ( IsSymbol( tokenbuffer[i][j], index ) ) {
                                    base = SYMTAB[index].address ;
                                    cout << "base : " << base << endl ;
                                } // else if
                            
                            } // else if 

                            else if ( temp.compare( "END"  ) == 0 ) {
                                address[i] = "\0" ;
                            } // else if 

                        } // else if 

                    } // for ()
            } // if checkundefined
            else {
                
                int startaddress = HexstrToDecint( address[i] ) ;
                address[i+1] = address[i] ;
                XErelocation( i+1, startaddress ) ;
                address[i] = "\0" ;
                errorline[i] = "error" ;
                i = 1 ;

            } // 
        } // if  
    } // for ()

} // pass2()

void SIClocation( vector<string> &address, vector<vector<string>> tokenbuffer ) {
    bool relocation = false ;
    // start address 把address補齊4位
    address[0] = tokenbuffer[0][2] ;
    while( address[0].size() < 4 ) {
        address[0] = "0" + address[0] ;
    } // while

    address[1] = address[0] ;

    int index = 0 ;
    int LOCCTR = HexstrToDecint( tokenbuffer[0][2] ) ; // 將16進位字串轉成10進位數字
    string temp ;
    int num = 0 ;
     
    for ( int i = 1 ; i < tokenbuffer.size() ; i++ ) {

        for  ( int j = 0 ; j < tokenbuffer[i].size() ; j++ ) {
            index = 0 ;
            temp = tokenbuffer[i][j] ;
        
            if ( temp.compare( "." ) == 0 ) break ;
   
            else if ( IsPseudo(temp) ) {

                if ( temp.compare( "WORD" ) == 0 ) 
                    LOCCTR = LOCCTR + 3 ;

                else if ( temp.compare( "BYTE" ) == 0 ) {

                    if ( tokenbuffer[i][j+1].compare( "X" ) == 0  ) 
                        LOCCTR = LOCCTR + tokenbuffer[i][j+3].size() / 2 ;

                    else if ( tokenbuffer[i][j+1].compare( "C" ) == 0  ) 
                        LOCCTR = LOCCTR + tokenbuffer[i][j+3].size() ;

                } // else if 

                else if ( temp.compare( "RESW" ) == 0 ) {

                    num = decstrTodecint( tokenbuffer[i][j+1] ) ;
                    LOCCTR = LOCCTR + num * 3 ;
                    
                } // else if ()

                else if ( temp.compare( "RESB" ) == 0 ) {

                    num = decstrTodecint( tokenbuffer[i][j+1] ) ;
                    LOCCTR = LOCCTR + num ;

                } // else if ()

            } // else if() 

            else if ( IsInstruction(temp, index) ) {
                LOCCTR = LOCCTR + 3 ;
            } // if

            else if ( IsDelimiter( temp ) ) ;
            else if ( IsNumber(temp) ) ;
            else if ( IsRegister(temp, index) ) ; 
            else { // symbol
                if ( j == 0 ) {
                    saveIntotable( temp, SYMTAB, address[i], relocation ) ;
                } // if 
            } // else 

        } // for

        
        if ( i+1 < tokenbuffer.size() ) {
            decinttohexstr( LOCCTR, temp ) ; // 將10進位數字轉成16進位字串

            //把address補齊4位
            while( temp.size() < 4 ) {
                temp = "0" + temp ;
            } // 

            address[i+1] = temp ;

        } // if 
        
    } // for 
} // SIClocation()

bool SICpass1() {
    
    bool exesuccess = true ;
    if ( Readfile( linebuffer ) ) {

        // 解決空白行的問題
        vector<string> temp ;
        for ( int i = 0 ; i < linebuffer.size() ; i++ ) {
            if ( linebuffer[i].size() != 0 ) temp.push_back( linebuffer[i] ) ;
        } // for 
        linebuffer.clear() ; 
        linebuffer = temp ;

        address.resize( linebuffer.size() ) ;
        errorline.resize( linebuffer.size() ) ;
        tokenbuffer.resize( linebuffer.size() ) ;

        GetToken( tokenbuffer, linebuffer ) ;
        SIClocation( address, tokenbuffer ) ;

    } // if 
    else exesuccess = false ;

    return exesuccess ;
} // SICpass1()

void SICpass2() {
    string pc ; // 指向下一道指令的位址
    machinecode.resize( address.size() ) ;
    int index ; // 代表token在某個table的位子
    string temp ;

    for ( int i = 0 ; i < tokenbuffer.size() ; i++ ) {
        index = 0 ;
        // 設定PC
        if ( i + 1 < tokenbuffer.size() ) {
            bool get = false ;
            int g = 0 ;
            for ( g = i+1 ; g < tokenbuffer.size() && ! get ;  ) {
                if ( errorline[g].compare( "error" ) == 0 ) g++ ;
                else if ( tokenbuffer[g][0].compare( "." ) == 0 ) g++ ;
                // else if ( tokenbuffer[g][0].compare( "END" ) == 0 ) ;
                // else if ( tokenbuffer[g].size() > 1 && tokenbuffer[g][1].compare( "END" ) == 0 ) ;
                else get = true ;
            } // 

            if ( g < tokenbuffer.size() &&  get ) pc = address[g] ;
            else {
                cout << " error " << endl ; // 找不到pc 
            } // else 
        } // 

        if ( errorline[i].compare( "error" ) != 0 ) {
            for ( int j = 0 ; j < tokenbuffer[i].size() ; j++ ) {

                temp = tokenbuffer[i][j] ;
                
                if ( IsInstruction(temp, index) ) {
                    string opcode = table1[index].opcode ;
                    hexbintrans( opcode , 1 ) ; // hex to bin
                    string ad ;
                    string x = "0" ;
                    for ( ; j < tokenbuffer[i].size() ; j++ ) {
                        if ( IsSymbol( tokenbuffer[i][j], index ) ) {
                            temp = SYMTAB[index].address ;
                            while ( temp.size() < 3 ) {
                                temp = "0" + temp ;
                            } // while()
                            hexbintrans( temp, 1 ) ; // hex to bin
                            ad = ad.assign( temp, 1, temp.size() - 1 ) ;
                        } // if
                        else if ( tokenbuffer[i][j].compare( "X" ) == 0 || tokenbuffer[i][j].compare( "x" ) == 0 ) {
                            x = "1" ;
                        } // else if 
                    } // for()
                    
                    
                    temp = opcode + x + ad ;
                    hexbintrans( temp, 2 ) ; // bin to hex
                    if ( opcode.compare( "01001100" ) == 0 ) { // rsub
                        while ( temp.size() < 6 ) { // rsub
                            temp = temp + "0" ;
                        } // 
                    } // if
                    machinecode[i] = temp ;

                } // if 

                else if ( IsPseudo( temp ) ) {
                    if ( temp.compare( "EQU" ) == 0 ) {
                        j++ ;
                        temp = tokenbuffer[i][j] ;

                        if ( IsNumber( temp ) ) {

                            int value = decstrTodecint( temp ) ;
                            decinttohexstr( value, temp ) ;
                            while( temp.size() < 4 ) {
                                temp = "0" + temp ;
                            } // while
                            address[i] = temp ;

                            if ( IsSymbol( tokenbuffer[i][j-2], index ) ) {
                                SYMTAB[index].address = temp ;
                            } // if 
                        } // if 
                        else if ( IsSymbol( temp , index ) ) { // label && label +-*/ labbel


                        } // else if () 
                    } // if 

                    else if ( temp.compare( "WORD" ) == 0 ) {
                        j = j + 1 ; // 取下一個TOKEN

                        if ( IsNumber( tokenbuffer[i][j] ) ) {
                            int value = decstrTodecint( tokenbuffer[i][j] ) ;
                            decinttohexstr( value, temp ) ;
                            while( temp.size() < 6 ) {
                                temp = "0" + temp ;
                            } // 
                            machinecode[i] = temp ;
                        } // if
                        else if ( tokenbuffer[i][j].compare( "X" ) == 0 || tokenbuffer[i][j].compare( "x" ) == 0 ) {
                            j = j + 2 ;
                            machinecode[i] = tokenbuffer[i][j] ;
                        } // if ()
                        else if ( tokenbuffer[i][j].compare( "C" ) == 0 || tokenbuffer[i][j].compare( "c" ) == 0 ) {
                            j = j + 2 ;
                            string s = "\0" ;
                            for ( int k = 0 ; k < tokenbuffer[i][j].size()  ; k++ ) {
                                int ascii = tokenbuffer[i][j][k] ; // 取得char的ascii
                                string  hstrascii  ;
                                decinttohexstr( ascii, hstrascii ) ;
                                s = s + hstrascii ; 
                            } // for 
                            machinecode[i] = s ;
                        } // else if 

                    } // else if 

                    else if ( temp.compare( "BYTE" ) == 0 ) {

                        j = j + 1 ; // 取下一個TOKEN

                        if ( IsNumber( tokenbuffer[i][j] ) ) {
                            int value = decstrTodecint( tokenbuffer[i][j] ) ;
                            decinttohexstr( value, temp ) ;
                            while( temp.size() < 6 ) {
                                temp = "0" + temp ;
                            } // 
                            machinecode[i] = temp ;
                        } // if
                        else if ( tokenbuffer[i][j].compare( "X" ) == 0 || tokenbuffer[i][j].compare( "x" ) == 0 ) {
                            j = j + 2 ;
                            machinecode[i] = tokenbuffer[i][j] ;
                        } // if ()
                        else if ( tokenbuffer[i][j].compare( "C" ) == 0 || tokenbuffer[i][j].compare( "c" ) == 0 ) {
                            j = j + 2 ;
                            string s = "\0" ;
                            for ( int k = 0 ; k < tokenbuffer[i][j].size()  ; k++ ) {
                                int ascii = tokenbuffer[i][j][k] ; // 取得char的ascii
                                string  hstrascii  ;
                                decinttohexstr( ascii, hstrascii ) ;
                                s = s + hstrascii ; 
                            } // for 
                            machinecode[i] = s ;
                        } // else if 
                        
                    } // else if 

                    else if ( temp.compare( "END"  ) == 0 ) {
                        address[i] = "\0" ;
                    } // else if 

                } // else if 

                else if ( temp.compare( "." ) == 0 ) {
                    address[i] = "\0" ;
                    break ;
                } // 

            } // for ()
        } // if 
    } // for ()
} // SICpass2()

void initial() {
    SYMTAB.clear() ;
    LITTAB.clear() ;
    SYMTAB.resize(100) ;
    LITTAB.resize(100) ;    
    tokenbuffer.clear() ;
    linebuffer.clear() ;
    machinecode.clear() ;
    address.clear() ;
    errorline.clear() ;
} // initial() 

int main() {

    string option = "\0" ;  

    cout << "## CHOOSE WHICH ASSEMBLER BE USED ## " << endl ;
    cout << "## 1. SIC/XE                         " << endl ;
    cout << "## 2. SIC                            " << endl ;
    cout << "## 3. quit                           " << endl ;
    cout << "Pleasae enter 1~3 : " ;
    cin >> option ; 

    while( option.compare( "3" ) != 0 ) {
        initial() ; // 初始化table
        if ( option.compare( "1" ) == 0 ) {
            if ( XEpass1() ) {
                XEpass2() ;
                outputfile() ;
                cout << endl << "Finish ! Please look up objet txt." << endl << endl ;
            } // if 
        } // if 
        else if ( option.compare( "2" ) == 0  ) {
            if ( SICpass1() ) {
                SICpass2() ;
                outputfile() ;
                cout << endl << "Finish ! Please look up objet txt." << endl << endl ;
            } // if 
        } // if 

        system( "pause" ) ;
        cout << endl ;
        cout << "## CHOOSE WHICH ASSEMBLER BE USED ## " << endl ;
        cout << "## 1. SIC/XE                         " << endl ;
        cout << "## 2. SIC                            " << endl ;
        cout << "## 3. quit                           " << endl ;
        cout << "Pleasae enter 1~3 : " ;
        cin >> option ; 
    } // while()

} // main()
