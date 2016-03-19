/*
 * RFID.cpp - RFID : Class Implement
 * Creator: sketchpink
 * Time: 2015.10.1
 */

/******************************************************************************
 * 包含文件
 ******************************************************************************/
#include <Arduino.h>

#include <RFID.h>

/******************************************************************************
 * 构造 RFID
 * int chipSelectPin RFID /ENABLE pin
 ******************************************************************************/
RFID::RFID(int chipSelectPin, int NRSTPD)
{
  _chipSelectPin = chipSelectPin;
  _NRSTPD = NRSTPD;

  pinMode(_chipSelectPin,OUTPUT);     // 设置管脚_chipSelectPin为输出并连接到模块使能口
  digitalWrite(_chipSelectPin, LOW);


  pinMode(_NRSTPD,OUTPUT);            // 设置管脚NRSTPD为输出，非重置或掉电
  digitalWrite(_NRSTPD, HIGH);
  
  for(int i = 0; i < 5; i++)	serNum[i] = 0;	
}

/******************************************************************************
 * 用户 API
 ******************************************************************************/

/******************************************************************************
 * 函 数 名：isCard
 * 功能描述：寻卡：在阅读器功率范围内找到效卡片
 * 输入参数：无
 * 返 回 值：成功返回ture 失败返回false
 ******************************************************************************/
bool RFID::isCard()
{
  unsigned char status;
  unsigned char str[MAX_LEN];

  status = MFRC522Request(PICC_REQIDL, str);	//str位置获取了若干字节返回数据
  if (status == MI_OK)
    return true;
  else
    return false;
}//str内容并未使用，退栈/析构

/******************************************************************************
 * 函 数 名：readCardSerial
 * 功能描述：返回卡的序列号 4字节，调用防冲突完成取序列号
 * 输入参数：无
 * 返 回 值：成功返回ture 失败返回false
 ******************************************************************************/
bool RFID::readCardSerial(){

  unsigned char status;
  unsigned char str[MAX_LEN];
  
  // 防冲撞，返回卡的序列号 4字节，存入serNum中
  status = anticoll(str);		//防冲突检测包含取序列号操作
    
  if (status == MI_OK){
    memcpy(serNum, str, 5);		//str最终被退栈析构，公有成员serNum保存数据
	return true;
  }
  else
    return false;
}

/******************************************************************************
 * 函 数 名：init
 * 功能描述：初始化RC522
 * 输入参数：无
 * 返 回 值：无
 ******************************************************************************/
void RFID::init()
{
  digitalWrite(_NRSTPD,HIGH);

  reset();

  //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
  writeMFRC522(TModeReg, 0x8D);   //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
  writeMFRC522(TPrescalerReg, 0x3E);  //TModeReg[3..0] + TPrescalerReg
  writeMFRC522(TReloadRegL, 30);
  writeMFRC522(TReloadRegH, 0);
  writeMFRC522(TxAutoReg, 0x40);    //100%ASK
  writeMFRC522(ModeReg, 0x3D);    // CRC valor inicial de 0x6363

  //ClearBitMask(Status2Reg, 0x08); //MFCrypto1On=0
  //writeMFRC522(RxSelReg, 0x86);   //RxWait = RxSelReg[5..0]
  //writeMFRC522(RFCfgReg, 0x7F);     //RxGain = 48dB

  antennaOn();    //打开天线
}

/******************************************************************************
 * 函 数 名：reset
 * 功能描述：复位RC522
 * 输入参数：无
 * 返 回 值：无
 ******************************************************************************/
void RFID::reset()
{
  writeMFRC522(CommandReg, PCD_RESETPHASE);
}

/******************************************************************************
 * 函 数 名：writeMFRC522
 * 功能描述：向MFRC522的某一寄存器写一个字节数据
 * 输入参数：addr--寄存器地址；val--要写入的值
 * 返 回 值：无
 ******************************************************************************/
void RFID::writeMFRC522(unsigned char addr, unsigned char val)
{
  digitalWrite(_chipSelectPin, LOW);

  //地址格式：0XXXXXX0
  SPI.transfer((addr<<1)&0x7E);
  SPI.transfer(val);

  digitalWrite(_chipSelectPin, HIGH);
}

/******************************************************************************
 * 函 数 名：readMFRC522
 * 功能描述：从MFRC522的某一寄存器读一个字节数据
 * 输入参数：addr--寄存器地址
 * 返 回 值：返回读取到的一个字节数据
 ******************************************************************************/
unsigned char RFID::readMFRC522(unsigned char addr)
{
  unsigned char val;
  digitalWrite(_chipSelectPin, LOW);
  SPI.transfer(((addr<<1)&0x7E) | 0x80);
  val =SPI.transfer(0x00);
  digitalWrite(_chipSelectPin, HIGH);
  return val;
}

/******************************************************************************
 * 函 数 名：setBitMask
 * 功能描述：置RC522寄存器位
 * 输入参数：reg--寄存器地址;mask--置位值
 * 返 回 值：无
 ******************************************************************************/
void RFID::setBitMask(unsigned char reg, unsigned char mask)
{
  unsigned char tmp;
  tmp = readMFRC522(reg);			//取寄存器原值并置掩码
  writeMFRC522(reg, tmp | mask);  // set bit mask, 掩码处置1
}

/******************************************************************************
 * 函 数 名：clearBitMask
 * 功能描述：清RC522寄存器位
 * 输入参数：reg--寄存器地址;mask--清位值
 * 返 回 值：无
 ******************************************************************************/
void RFID::clearBitMask(unsigned char reg, unsigned char mask)
{
  unsigned char tmp;
  tmp = readMFRC522(reg);			//取寄存器原值并置掩码
  writeMFRC522(reg, tmp & (~mask));  // clear bit mask, 掩码处清0
}

/******************************************************************************
 * 函 数 名：antennaOn
 * 功能描述：开启天线,每次启动或关闭天险发射之间应至少有1ms的间隔
 * 输入参数：无
 * 返 回 值：无
 ******************************************************************************/
void RFID::antennaOn(void)
{
  unsigned char temp;

  temp = readMFRC522(TxControlReg);
  if (!(temp & 0x03))
  {
    setBitMask(TxControlReg, 0x03);
  }
}

/******************************************************************************
 * 函 数 名：antennaOff
 * 功能描述：关闭天线,每次启动或关闭天险发射之间应至少有1ms的间隔
 * 输入参数：无
 * 返 回 值：无
 ******************************************************************************/
void RFID::antennaOff(void)
{
  unsigned char temp;

  temp = readMFRC522(TxControlReg);
  if (!(temp & 0x03))
  {
    clearBitMask(TxControlReg, 0x03);
  }
}

/******************************************************************************
 * 函 数 名：calculateCRC
 * 功能描述：用MF522计算CRC
 * 输入参数：pIndata--要读数CRC的数据，len--数据长度，pOutData--计算的CRC结果
 * 返 回 值：无
 ******************************************************************************/
void RFID::calculateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
  unsigned char i, n;

  clearBitMask(DivIrqReg, 0x04);      //CRCIrq = 0
  setBitMask(FIFOLevelReg, 0x80);     //清FIFO指针
  //Write_MFRC522(CommandReg, PCD_IDLE);

  //向FIFO中写入数据
  for (i=0; i<len; i++)
    writeMFRC522(FIFODataReg, *(pIndata+i));
  writeMFRC522(CommandReg, PCD_CALCCRC);

  //等待CRC计算完成
  i = 0xFF; 		//255倒计时
  do
  {
    n = readMFRC522(DivIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x04));      //CRCIrq = 1

  //读取CRC计算结果
  pOutData[0] = readMFRC522(CRCResultRegL);
  pOutData[1] = readMFRC522(CRCResultRegM);
}

/******************************************************************************
 * 函 数 名：MFRC522ToCard
 * 功能描述：RC522和ISO14443卡通讯
 * 输入参数：command--MF522命令字，
 *           sendData--通过RC522发送到卡片的数据,
 *                     sendLen--发送的数据长度
 *                     backData--接收到的卡片返回数据，
 *                     backLen--返回数据的位长度
 * 返 回 值：成功返回MI_OK
 ******************************************************************************/
unsigned char RFID::MFRC522ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned int *backLen)
{
  unsigned char status = MI_ERR;
  unsigned char irqEn = 0x00;
  unsigned char waitIRq = 0x00;
  unsigned char lastBits;
  unsigned char n;				//
  unsigned int i;

  switch (command)
  {
    case PCD_AUTHENT:   //认证卡密
    {
      irqEn = 0x12;		// 0001 0010
      waitIRq = 0x10;	// 0001	0000
      break;
    }
    case PCD_TRANSCEIVE:  //发送FIFO中数据
    {
      irqEn = 0x77;		// 0111 0111	为什么不写0xF7?
      waitIRq = 0x30;	// 0011 0000 
      break;
    }
    default:
      break;
  }

  writeMFRC522(CommIEnReg, irqEn|0x80); //允许中断请求 1000 0000 
  clearBitMask(CommIrqReg, 0x80);       //清除所有中断请求位
  setBitMask(FIFOLevelReg, 0x80);       //FlushBuffer=1, FIFO初始化

  writeMFRC522(CommandReg, PCD_IDLE);   //无动作，取消当前命令

  //内存向MFRC522 FIFO中写入数据,不超过16字节,其中第一个字节为卡片命令字PICC_READ、PICC_WRITE,第二字节为块地址
  for (i=0; i<sendLen; i++)
    writeMFRC522(FIFODataReg, sendData[i]);

  //执行命令
  writeMFRC522(CommandReg, command);	//传入命令字：准备交互数据或认证
  if (command == PCD_TRANSCEIVE)
    setBitMask(BitFramingReg, 0x80);    //StartSend=1,transmission of data starts  开始交互数据

  //等待接收数据完成,等待中断信号
  i = 2000; 							//i根据时钟频率调整，操作M1卡最大等待时间25ms
  do
  {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq		7类中断
    n = readMFRC522(CommIrqReg);		//轮询CommIrqReg,任意一类中断发生，n非0
    i--;
  }
  while ((i!=0) && !(n&0x01) && !(n&waitIRq));	//i倒计时结束、TimerValueReg超时结束、5 4位发生中断结束(期望)

  clearBitMask(BitFramingReg, 0x80);      //StartSend=0

  if (i != 0)		//检查是否由于超时结束，保证非第一类结束
  {
    if(!(readMFRC522(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr 非此若干种类错误，否则MI_ERR
    {
      status = MI_OK;					//成功！
      if (n & irqEn & 0x01)
        status = MI_NOTAGERR;     //??

      if (command == PCD_TRANSCEIVE)	//若做的是数据交互，则做数据保存的善后处理
      {
        n = readMFRC522(FIFOLevelReg);	//字节数
        lastBits = readMFRC522(ControlReg) & 0x07;
        if (lastBits)					//计数，位
          *backLen = (n-1)*8 + lastBits;	//末尾若干bit不足1字节
        else
          *backLen = n*8;					//字节整数倍

        if (n == 0)							//两种越界情况
          n = 1;
        if (n > MAX_LEN)
          n = MAX_LEN;

        //读取FIFO中接收到的数据，FIFO是真正的交换区，其他变量为临时交换区
        for (i=0; i<n; i++)
          backData[i] = readMFRC522(FIFODataReg);
      }
    }
    else
      status = MI_ERR;
  }

  //SetBitMask(ControlReg,0x80);           //timer stops
  //Write_MFRC522(CommandReg, PCD_IDLE);

  return status;
}


/******************************************************************************
 * 函 数 名：MFRC522Request
 * 功能描述：寻卡，读取卡类型号
 * 输入参数：reqMode--寻卡方式，
 *           TagType--返回卡片类型
 *                    0x4400 = Mifare_UltraLight
 *                    0x0400 = Mifare_One(S50)
 *                    0x0200 = Mifare_One(S70)
 *                    0x0800 = Mifare_Pro(X)
 *                    0x4403 = Mifare_DESFire
 * 返 回 值：成功返回MI_OK
 ******************************************************************************/
unsigned char RFID::MFRC522Request(unsigned char reqMode, unsigned char *TagType)
{
  unsigned char status;
  unsigned int backBits;      //接收到的数据bit数

  writeMFRC522(BitFramingReg, 0x07);    //TxLastBists = BitFramingReg[2..0] ???

  //TagType放置发送数据(1Byte)，同时TagType位置保存返回数据(backbits bit),此区域可看做数据交换区
  TagType[0] = reqMode;
  status = MFRC522ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
  //检查状态，backbits不超过16位?
  if ((status != MI_OK) || (backBits != 0x10))
    status = MI_ERR;

  return status;
}

/******************************************************************************
 * 函 数 名：anticoll
 * 功能描述：防冲突检测：多张卡片同时出现在广播方位内时，只选取一张卡片，并读取
			选中卡片的卡序列号
 * 输入参数：serNum--返回4字节卡序列号,第5字节为校验字节
 * 返 回 值：成功返回MI_OK
 ******************************************************************************/
unsigned char RFID::anticoll(unsigned char *serNum)
{
  unsigned char status;
  unsigned char i;
  unsigned char serNumCheck = 0;		// 0000 0000 
  unsigned int unLen;

  //ClearBitMask(Status2Reg, 0x08);   //TempSensclear
  //ClearBitMask(CollReg,0x80);     //ValuesAfterColl
  writeMFRC522(BitFramingReg, 0x00);    //TxLastBists = BitFramingReg[2..0]

  serNum[0] = PICC_ANTICOLL;
  serNum[1] = 0x20;
  
  //防冲突的核心内容为完整一次卡片通信，
  status = MFRC522ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

  if (status == MI_OK)
  {
    //校验卡序列号
    for (i=0; i<4; i++)				//本卡4字节序列号，1字节校验
      serNumCheck ^= serNum[i];
    if (serNumCheck != serNum[i])	//5个字节全部做异或校验(serNumCheck),结果与serNum[4]校验字节应一致
      status = MI_ERR;
  }

  //SetBitMask(CollReg, 0x80);    //ValuesAfterColl=1

  return status;		//serNum成员保存返回
}

/******************************************************************************
 * 函 数 名：auth
 * 功能描述：验证卡片密码
 * 输入参数：authMode--密码验证模式
 *                     0x60 = 验证A密钥
 *                     0x61 = 验证B密钥
 *           BlockAddr--块地址
 *           Sectorkey--扇区密码
 *           serNum--卡片序列号，4字节
 * 返 回 值：成功返回MI_OK
 ******************************************************************************/
unsigned char RFID::auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum)
{
  unsigned char status;
  unsigned int recvBits;
  unsigned char i;
  unsigned char buff[12];

  //验证指令+块地址＋扇区密码＋卡序列号
  buff[0] = authMode;
  buff[1] = BlockAddr;
  for (i=0; i<6; i++)
    buff[i+2] = *(Sectorkey+i);
  for (i=0; i<4; i++)
    buff[i+8] = *(serNum+i);
    
  status = MFRC522ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
  if ((status != MI_OK) || (!(readMFRC522(Status2Reg) & 0x08)))
    status = MI_ERR;

  return status;
}

/******************************************************************************
 * 函 数 名：read
 * 功能描述：读块数据
 * 输入参数：blockAddr--块地址;recvData--读出的块数据
 * 返 回 值：成功返回MI_OK
 ******************************************************************************/
unsigned char RFID::read(unsigned char blockAddr, unsigned char *recvData)
{
  unsigned char status;
  unsigned int unLen;

  recvData[0] = PICC_READ;		//第一字节为卡片命令字
  recvData[1] = blockAddr;		//第二字节为块地址
  calculateCRC(recvData,2, &recvData[2]);
  status = MFRC522ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

  if ((status != MI_OK) || (unLen != 0x90))
    status = MI_ERR;

  return status;
}

/******************************************************************************
 * 函 数 名：write
 * 功能描述：写块数据
 * 输入参数：blockAddr--块地址;writeData--向块写16字节数据
 * 返 回 值：成功返回MI_OK
 ******************************************************************************/
unsigned char RFID::write(unsigned char blockAddr, unsigned char *writeData)
{
  unsigned char status;
  unsigned int recvBits;
  unsigned char i;
  unsigned char buff[18];		//16 + 2 

  buff[0] = PICC_WRITE;			//第一字节为卡片命令字
  buff[1] = blockAddr;			//第二字节为块地址
  calculateCRC(buff, 2, &buff[2]);
  status = MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

  if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    status = MI_ERR;

  if (status == MI_OK)		//第一次toCard确认写操作，第二次toCard完成数据写入
  {
    for (i=0; i<16; i++)    //?FIFO?16Byte?? Datos a la FIFO 16Byte escribir
      buff[i] = *(writeData+i);	//buff加载数据，传给toCard
      
    calculateCRC(buff, 16, &buff[16]);
    status = MFRC522ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);	//数据写入

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
      status = MI_ERR;
  }

  return status;
}

/******************************************************************************
 * 函 数 名：selectTag
 * 功能描述：选卡，读取卡存储器容量，锁定卡片，防止多次读写
 * 输入参数：serNum--传入卡序列号
 * 返 回 值：成功返回卡容量
 ******************************************************************************/
unsigned char RFID::selectTag(unsigned char *serNum)
{
  unsigned char i;
  unsigned char status;
  unsigned char size;
  unsigned int recvBits;
  unsigned char buffer[9];

  //ClearBitMask(Status2Reg, 0x08);                        //MFCrypto1On=0

  buffer[0] = PICC_SElECTTAG;
  buffer[1] = 0x70;

  for (i=0; i<5; i++)
    buffer[i+2] = *(serNum+i);

  calculateCRC(buffer, 7, &buffer[7]);
  
  status = MFRC522ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
  if ((status == MI_OK) && (recvBits == 0x18))
    size = buffer[0];
  else
    size = 0;
  return size;
}

/******************************************************************************
 * 函 数 名：Halt
 * 功能描述：命令卡片进入休眠状态
 * 输入参数：无
 * 返 回 值：无
 ******************************************************************************/
void RFID::halt()
{
  unsigned char status;
  unsigned int unLen;
  unsigned char buff[4];

  buff[0] = PICC_HALT;
  buff[1] = 0;
  calculateCRC(buff, 2, &buff[2]);

  status = MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}