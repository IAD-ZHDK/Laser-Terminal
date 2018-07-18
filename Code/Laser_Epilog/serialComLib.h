/* ----------------------------------------------------------------------------
 *  Serial Communication Lib
 * ----------------------------------------------------------------------------
 *  serialComLib.h								
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 08/07/2009
 * ----------------------------------------------------------------------------
 */
 
#ifndef _SERIALCOMLIB_H_
#define _SERIALCOMLIB_H_

#define DATAHEADER1  0x1a
#define DATAHEADER2  0xd0

class SerialComLib
{
public:

   enum Errors{
    Ok                  =   0,
    Error_Command       =   1,
    Error_Checksum      =   2,
    Error_Timeout       =   3,
    Error_DataHeader    =   4 ,    
    Error_Data          =   5,
    Error_DataSize      =   6,
    Error_NoData        =   7,
   };
   
   SerialComLib();
 
   bool open(long baudRate = 57600);
   
   bool available();
   void writeCmd(unsigned char cmdId, unsigned char dataLen, unsigned char* data);
   unsigned char readCmd(unsigned char* cmdId, unsigned char* dataLen, unsigned char* data,unsigned char dataLenMax);
   
   bool isOpen() { return _open; }
  
   static unsigned char readBlocked(unsigned char length,unsigned char* readValue,unsigned char readCount = 10,unsigned char delayMS = 1);

   // helper func.
   static void ConvShortToStr(unsigned short value,unsigned char* buf)
   {
     buf[0] = 0xff & value;
     buf[1] = (0xff00 & value) >> 8;
   }
   
   static void ConvStrToShort(unsigned char* buf, unsigned short* value)
   {
     *value = buf[0] + (buf[1] << 8);
   }
   
protected:

  bool findDataHeader();
  unsigned char   checksum(unsigned char len,unsigned char* data);
  bool            testChecksum(unsigned char len,unsigned char* data,unsigned char checksum);
  
  bool            _open;
  unsigned char   _byte;
};

#endif // _SERIALCOMLIB_H_
