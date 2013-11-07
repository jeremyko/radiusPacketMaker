#include <iomanip> 
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <ctime>
#include <time.h>

using namespace std;

const int TYPE_FILED_LEN   =1 ;
const int LENGTH_FIELD_LEN =1 ;

char  gBuffer [4096];
int   gBufferIndex = 0;
int   gTotalWriteLen = 0;

///////////////////////////////////////////////////////////////////////////////
void ConvertStringToHexaByteAndSetIntoWriteBuffer( int nLen, string& tempStr) ;
bool ConvertIntToHexaByteAndSetIntoWriteBuffer( int nFieldLen,  string& val );
void ConvertHexStringToHexaByteAndSetIntoWriteBuffer( int nLen,  string& tempStr);
char* Trimwhitespace(char *str);
size_t Trimwhitespace(char* out, size_t len, const char* str);
void CharToHex(const char s, char* out );
string ToHex(const string& s, bool upper_case );
bool SaveFile ( char* output_path);

///////////////////////////////////////////////////////////////////////////////
string ToHex(const string& s, bool upper_case )
{    
    ostringstream ret;

    for (string::size_type i = 0; i < s.length(); ++i)
    {
        ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << (int)s[i];
    }

    return ret.str();
}

///////////////////////////////////////////////////////////////////////////////
void CharToHex(const char s, char* out )
{   
    char szTemp[10];
    memset(&szTemp, 0x00, sizeof(szTemp));
    sprintf_s(szTemp, "0x%02X\n", s);
    //printf("-- %s\n",szTemp);
    strcpy(out, szTemp  );    
}

///////////////////////////////////////////////////////////////////////////////
bool SaveFile ( char* output_path)
{
    cout << "gBufferIndex:" << gBufferIndex << "\n";
    cout << "gTotalWriteLen:" << gTotalWriteLen << "\n";

    //set header length
    char output[100];
    memset(output, 0x00, sizeof(output));
    sprintf(output,"%04x",  gTotalWriteLen ); //radius header, length is 2 byte, buffer index starts from 2
    char*p = output ;
    char szStr [3];
    unsigned int nHexValue = 0; 
    int nHeaderLengthIndex = 2;
    for(int i=0; i < 2 ; i++)
    {        
        memset(szStr, 0x00, sizeof(szStr));
        strncpy( szStr, p, 2 );

        nHexValue = 0; 
        std::stringstream ss;
        ss << std::hex << szStr;
        ss >>  nHexValue;

        gBuffer[nHeaderLengthIndex]  = nHexValue     ;                 
        nHeaderLengthIndex++;     
        p+=2;
    }
        
    //write
    ofstream udrFile;
    
    udrFile.open (output_path, std::ofstream::binary );    
        
    udrFile.write (gBuffer, gTotalWriteLen  ); 
    
    udrFile.close();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool ConvertIntToHexaByteAndSetIntoWriteBuffer( int nFieldLen,  string& val )
{       
    //string hexstring =  ToHex(val, false );  

    char output[100];
    memset(output, 0x00, sizeof(output));
    //if( atoi(val.c_str() ) > 255 )
    if( nFieldLen == 1 )
    {
        sprintf(output,"%02x",atoi(val.c_str()) );        
    }
    if( nFieldLen == 2 )
    {
        sprintf(output,"%04x",atoi(val.c_str()) );
    }
    else if( nFieldLen == 4 )
    {
        sprintf(output,"%08x",atoi(val.c_str()) );
    }
    else
    {
        sprintf(output,"%02x",atoi(val.c_str()) );
    }

    //printf("%s\n",output);

    int nLenStr = strlen(output);

    char*p = output ;
    char szStr [3];
    unsigned int nHexValue = 0; 
    for(int i=0; i < nFieldLen ; i++)
    {        
        memset(szStr, 0x00, sizeof(szStr));
        strncpy( szStr, p, 2 );

        nHexValue = 0; 
        std::stringstream ss;
        ss << std::hex << szStr;
        ss >>  nHexValue;

        gBuffer[gBufferIndex]  = nHexValue     ;                 
        gBufferIndex++;     
        p+=2;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void ConvertStringToHexaByteAndSetIntoWriteBuffer( int nLen,  string& tempStr)
{
    for (int i = 0; i < tempStr.length(); i++)
    {     
        // String to Hex
        char hexstring[100];
        memset(&hexstring, 0x00, sizeof(hexstring));
        CharToHex(tempStr[i],hexstring);     

        unsigned int nHexValue = 0; 
        std::stringstream ss;
        ss << std::hex << hexstring;
        ss >>  nHexValue;

        gBuffer[gBufferIndex]  = nHexValue      ;                 
        gBufferIndex++;     
    }    
}

///////////////////////////////////////////////////////////////////////////////
void ConvertHexStringToHexaByteAndSetIntoWriteBuffer( int nLen,  string& tempStr)
{
    // 16진수 값이 직접 들어있다...
    char* p = (char*)tempStr.c_str();
    for(int i = 0; i < nLen - 2; i++) //-2 : except for : type, length 
    {
        char szStr [3];
        memset(szStr, 0x00, sizeof(szStr));
        strncpy( szStr, p, 2 );

        unsigned int nHexValue = 0; 
        std::stringstream ss;
        ss << std::hex << szStr;
        ss >>  nHexValue;
        gBuffer[gBufferIndex]  = nHexValue      ;                 
        gBufferIndex++;

        p +=2;
    }             
}

///////////////////////////////////////////////////////////////////////////////
char* Trimwhitespace(char *str)
{
    char *end;

    // Trim leading space
    while(isspace(*str)) 
    {
        str++;
    }

    if(*str == 0)  // All spaces
    {
        return str;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end))
    {
        end--;
    }

    // Write new null terminator
    *(end+1) = 0;

    return str;
}

///////////////////////////////////////////////////////////////////////////////
size_t Trimwhitespace(char* out, size_t len, const char* str)
{
    if(len == 0)
    {
        return 0;
    }

    const char *end;
    size_t out_size;

    // Trim leading space
    while(isspace(*str)) 
    {
        str++;
    }

    if(*str == 0)  // All spaces
    {
        *out = 0;
        return 1;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) 
    {
        end--;
    }
    end++;

    // Set output size to minimum of trimmed string length and buffer size minus 1
    out_size = (end - str) < len-1 ? (end - str) : len-1;

    // Copy trimmed string and add null terminator
    memcpy(out, str, out_size);
    out[out_size] = 0;

    return out_size;
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{    
    //init
    gBufferIndex = 0;
    gTotalWriteLen = 0;
    memset( &gBuffer, 0x00, sizeof(gBuffer));    
    
    if(argc != 3 )
    {
        printf(" usage:\n");
        printf(" %s [input_data_file] [output_udr_file]\n", argv[0] );
    }
    
    string temp = "";
    
    //read file    
    std::ifstream infile(argv[1]); 
    
    vector<string> vecOfHeader;
    vector<string> vecOfLine;
    std::string line;

    while (std::getline(infile, line))
    {
        if( line == "" ) //this is blank
        {
            continue;
        }

        char szReadLine [255];
        Trimwhitespace(szReadLine, sizeof(szReadLine), line.c_str() );

        if( szReadLine[0] == '#' ) //this is comment
        {
            continue;
        }

        if( 0 == strncmp( szReadLine, "header", 6)  ) //this is header data
        {
            vecOfHeader.push_back(line);
        }
        else
        {
            vecOfLine.push_back(line);
        }        
    }

    infile.close();

    //////////////////////////////////////////////////////////////////////////
    // tokenize line and fill buffer
    // header -> 1+1+2+16 = 20
    /*
     0                   1                   2                   3
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    | Code          | Identifier    | Length                        |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                                                               |
    | Request Authenticator   (16 byte)                             |
    |                                                               |
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    | Attributes ...
    +-+-+-+-+-+-+-+-+-+-+-+-+-
    */
    for(std::vector<string>::iterator it = vecOfHeader.begin(); it != vecOfHeader.end(); ++it) 
    {
        cout << "header: " << *it << "\n";       
        stringstream tempStringStream(*it);
        string tempStr;
        vector<string> vecOfElement;

        while (getline(tempStringStream, tempStr, '|')) 
        {
            vecOfElement.push_back(tempStr);
        }

        int nIndex = 0;
        for(std::vector<string>::iterator it = vecOfElement.begin(); it != vecOfElement.end(); ++it)         
        {
            string strElement = *(it);            
            /*
            header_code|1|04
            header_Identifier|1|55
            header_length|2|400
            header_authenticator|16|1f437ed89f17abd4c34adb76b1f1300f
            */
            if( nIndex == 2 ) //need just value
            {                
                if( vecOfElement[0] == "header_authenticator" )
                {
                    //16 byte , 이건 16진수 값이 직접 들어있다...
                    char* p = (char*)strElement.c_str();
                    for(int i = 0; i < 16; i++)
                    {
                        char szStr [3];
                        memset(szStr, 0x00, sizeof(szStr));
                        strncpy( szStr, p, 2 );

                        unsigned int nHexValue = 0; 
                        std::stringstream ss;
                        ss << std::hex << szStr;
                        ss >>  nHexValue;
                        gBuffer[gBufferIndex]  = nHexValue      ;                 
                        gBufferIndex++;

                        p +=2;
                    }                    
                    gTotalWriteLen +=16;
                }
                else
                {
                    // vecOfElement[1] --> length
                    int len= atoi( vecOfElement[1].c_str() );
                    ConvertIntToHexaByteAndSetIntoWriteBuffer( atoi( vecOfElement[1].c_str() ) , tempStr );
                    gTotalWriteLen +=len;
                }                
            } 
            nIndex++;
        }
    }
        
    //////////////////////////////////////////////////////////////////////////
    for(std::vector<string>::iterator it = vecOfLine.begin(); it != vecOfLine.end(); ++it) 
    {        
        //tokenize
        stringstream tempStringStream(*it);
        string tempStr;        
        bool bVendorSpecific = false;
        bool bHeaderData = false;

        vector<string> vecOfElement;
        while (getline(tempStringStream, tempStr, '|')) 
        {
            vecOfElement.push_back(tempStr);            
        }

        int nIndex = 0;
        for(std::vector<string>::iterator it = vecOfElement.begin(); it != vecOfElement.end(); ++it) 
        {
            string strElement = *(it);

            //--------------------------------
            if( nIndex == 0 )
            {
                if( strElement == "26" ) 
                {
                    cout << "Type: Vendor specific!" << "\n";        
                    bVendorSpecific = true;
                    /*
                    * vendor specific
                     0                   1                   2                   3
                     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                    | Type          | Length        | Vendor-Id
                    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                    Vendor-Id (cont)                | Vendor type   | Vendor length |
                    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                    | Attribute-Specific...
                    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
                    */
                    // 26|23|10415|1|17|450050209966672                    
                }
                else
                { 
                    //6|6|2|numeric
                    bVendorSpecific = false;        
                }

                ConvertIntToHexaByteAndSetIntoWriteBuffer( TYPE_FILED_LEN, strElement);
                gTotalWriteLen +=TYPE_FILED_LEN;                
            }    
            //--------------------------------
            else if( nIndex == 1 )
            {                
                ConvertIntToHexaByteAndSetIntoWriteBuffer( LENGTH_FIELD_LEN, strElement  );
                gTotalWriteLen +=LENGTH_FIELD_LEN;                
            }               
            //--------------------------------
            else if( nIndex == 2 )
            {                
                if( bVendorSpecific ) 
                {   //vendor-id, 4byte                 
                    ConvertIntToHexaByteAndSetIntoWriteBuffer( 4 ,  strElement  );  
                    gTotalWriteLen +=4;                    
                }
                else
                {                             
                    int len = atoi( vecOfElement[1].c_str() ) ;

                    if( vecOfElement[3] == "string" )
                    {
                        ConvertStringToHexaByteAndSetIntoWriteBuffer ( len -2   , strElement);
                    }
                    else if( vecOfElement[3] == "numeric" )
                    {
                        ConvertIntToHexaByteAndSetIntoWriteBuffer( len -2  ,  strElement  );   
                    }
                    else if( vecOfElement[3] == "hexa" )
                    {
                        ConvertHexStringToHexaByteAndSetIntoWriteBuffer ( len   , strElement);
                    }                                       
                    else
                    {
                    }

                    gTotalWriteLen +=len-2;                    
                }
            }   
            //-------------------------------- vendor specific only
            if( bVendorSpecific ) 
            { 
                // 26|23|10415|1|17|450050209966672|string
                if( nIndex == 3 )
                { 
                    //vendor-type
                    ConvertIntToHexaByteAndSetIntoWriteBuffer( TYPE_FILED_LEN, strElement);
                    gTotalWriteLen +=TYPE_FILED_LEN;                    
                }
                else if( nIndex == 4 )
                { 
                    //Vendor length
                    ConvertIntToHexaByteAndSetIntoWriteBuffer( LENGTH_FIELD_LEN, strElement  );
                    gTotalWriteLen +=LENGTH_FIELD_LEN;                    
                }
                else if( nIndex == 5 )
                { 
                    //Attribute-Specific -- string
                    int len = atoi( vecOfElement[4].c_str() ) ;
                    // # format : type|length|value|value_type 
                    //# - length include type|length+value
                    //# - value_type : one of following string value
                    //#   "string", "numeric", "hexa"
                    if( vecOfElement[6] == "string" )
                    {
                        ConvertStringToHexaByteAndSetIntoWriteBuffer ( len -2   , strElement);
                    }
                    else if( vecOfElement[6] == "numeric" )
                    {
                        ConvertIntToHexaByteAndSetIntoWriteBuffer( len - 2, strElement  );
                    }
                    else if( vecOfElement[6] == "hexa" )
                    {
                        ConvertHexStringToHexaByteAndSetIntoWriteBuffer ( len   , strElement);
                    }
                    gTotalWriteLen +=len-2;                     
                }
            }

            nIndex++;
        } //for       
    }

    //write file    
    SaveFile (argv[2]);

    return 0;
}


