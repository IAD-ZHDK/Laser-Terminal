/* ----------------------------------------------------------------------------
 *  Serial Communication Lib
 * ----------------------------------------------------------------------------
 *  serialComLib.cpp								
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 08/07/2009
 * ----------------------------------------------------------------------------
 */

#include "wiring.h"
#include "HardwareSerial.h"

#include "serialComLib.h"

#define   MIN_PACKET_LENGTH    5 // 2(header) + 1(commandId) + 1(length) + 1(checksum) = 5
#define   TRY_OUT_COUNT        20

SerialComLib::SerialComLib():
_open(false)
{
}

bool SerialComLib::open(long baudRate)
{
  if(_open)
    return true;
  
  // start the hardware uart
  Serial.begin(baudRate);
  _open = true;
  
  return _open;
}

bool SerialComLib::available()
{
  return (Serial.available() >= MIN_PACKET_LENGTH)? true : false;
}

void SerialComLib::writeCmd(unsigned char cmdId, unsigned char dataLen, unsigned char* data)
{
  // write header
  Serial.write(DATAHEADER1);
  Serial.write(DATAHEADER2);
  
  // write commandId
  Serial.write(cmdId);

  // write data
  Serial.write(dataLen);
  if(dataLen == 0)
    Serial.write(0xff);  // checksum
  else
  {
    Serial.write(data,dataLen);

    // calc the checksume and write it
    Serial.write(checksum(dataLen,data));
  }
}

unsigned char SerialComLib::readCmd(unsigned char* cmdId, unsigned char* dataLen, unsigned char* data,unsigned char dataLenMax)
{
  if(findDataHeader())
  {  // read the data
  
     // read commandId
     if(readBlocked(1,cmdId) != Ok)
       return Error_NoData;
     
     // read dataLen
     if(readBlocked(1,dataLen) != Ok)
       return Error_NoData;
     
     // read data
     if(*dataLen > dataLenMax)
       return Error_DataSize;
       
     if(*dataLen > 0)
     {
       if(readBlocked(*dataLen,data) != Ok)
         return Error_NoData;
     }
       
     // checksum 
     if(readBlocked(1,&_byte) != Ok)
       return Error_NoData;
     
    // if(testChecksum(*dataLen,data,_byte))
       return Ok;
  }
  else
    return Error_DataHeader;
}
  
unsigned char SerialComLib::readBlocked(unsigned char length,unsigned char* readValue,unsigned char readCount,unsigned char delayMS)
{
  while(readCount > 0)
  {
    if(Serial.available() >= length)
    {  // read out the data
      for(int i=0;i < length;i++)
        readValue[i] = Serial.read();
      return 0;
    }
    else
      // wait a little
      delay(delayMS); 
    --readCount;
  }
  
  return 1;
}
  
bool SerialComLib::findDataHeader()
{ 
  char count = TRY_OUT_COUNT;
  
  while(count > 0)
  {
    if(readBlocked(1,&_byte) == Ok && _byte == DATAHEADER1)
    {  // read dataHeader1
      if(readBlocked(1,&_byte) == Ok && _byte == DATAHEADER2)
        // read dataHeader2
        return true;         
    } 
    --count;
  }
  return false;
}

unsigned char SerialComLib::checksum(unsigned char len,unsigned char* data)
{
  unsigned short checkSum = 0;
  for(int i=0;i < len;i++)
    checkSum += data[i];
  return(0xff - (0xff & checkSum));
}

bool SerialComLib::testChecksum(unsigned char len,unsigned char* data,unsigned char checksumVal)
{
  return checksum(len,data) == checksumVal;
}

