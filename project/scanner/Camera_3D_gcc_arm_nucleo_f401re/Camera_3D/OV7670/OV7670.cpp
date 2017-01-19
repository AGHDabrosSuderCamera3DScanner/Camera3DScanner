#include "OV7670.h"

extern Serial pc;
extern bool VSyncRise;
extern bool VSyncFall;
extern bool HRefRise;

OV7670Camera::OV7670Camera(
    PinName SCL,
    PinName SDA,
    PinName VSYNC,
    PinName HREF,
    PinName D7,
    PinName D6,
    PinName D5,
    PinName D4,
    PinName D3,
    PinName D2,
    PinName D1,
    PinName D0
)   : m_I2C(SDA, SCL)
    , m_VSync( VSYNC )
    , m_HRef( HREF )
    , m_Data( D7, D6, D5, D4, D3, D2, D1, D0 )
    , m_RowCount( 0 )
    , m_PixelCount( 0 )
{
    wait_us(500);
    m_I2C.stop();
    m_I2C.frequency( OV7670_I2CFREQ );
}

OV7670Camera::~OV7670Camera( void )
{
}

// write to camera
void OV7670Camera::WriteReg(int addr,int data)
{
    m_I2C.start();
    wait_us(OV7670_WRITEWAIT);
    m_I2C.write(OV7670_WRITE);
    wait_us(OV7670_WRITEWAIT);
    m_I2C.write(addr);
    wait_us(OV7670_WRITEWAIT);
    m_I2C.write(data);
    wait_us(OV7670_WRITEWAIT);
    m_I2C.stop();
    wait_us(OV7670_WRITEWAIT);
}

// read from camera
int OV7670Camera::ReadReg(int addr)
{
    int data = 0;
    
    wait_us(OV7670_WRITEWAIT);
    m_I2C.start();
    wait_us(OV7670_WRITEWAIT); 
    m_I2C.write(OV7670_WRITE);
    wait_us(OV7670_WRITEWAIT);
    m_I2C.write(addr);
    wait_us(OV7670_WRITEWAIT); 
    m_I2C.stop();
    wait_us(OV7670_WRITEWAIT); 

    m_I2C.start();
    m_I2C.write(OV7670_READ);
    wait_us(OV7670_WRITEWAIT);
    data = m_I2C.read(OV7670_NOACK);
    wait_us(OV7670_WRITEWAIT);
    m_I2C.stop();  
    wait_us(OV7670_WRITEWAIT); 
    return data;
}

void OV7670Camera::Reset(void) 
{    
    WriteReg(0x12, 0x80);   // RESET CAMERA
    wait_ms(200);
}

int OV7670Camera::Init( const char * format, int x, int y )
{
    // 160*120
    // 320*240
    // 640*480  
    
    int n = x * y;
    
    //printf( "REG_PID: %0.8X\r\n", ReadReg(REG_PID));
    int pid = ReadReg(REG_PID);
    pc.printf("pid: %0.2X\r\n", pid);
       
    if ( pid != 0x76)           // check id camera
    {
        return 0;
    }    
    
    Reset();                                 // Resets all registers to default values
    Reset();                                 // Resets all registers to default values
    
    WriteReg(REG_RGB444, 0x00);              // Disable RGB444
    WriteReg(REG_COM10, 0x02);               // 0x02   VSYNC negative (http://nasulica.homelinux.org/?p=959)
    WriteReg(REG_MVFP, 0x27);                // mirror image 

    WriteReg(REG_CLKRC, 0x80);               // prescaler x1     
    WriteReg(DBLV, 0x0a);                    // bypass PLL 
        
    WriteReg(REG_COM11, 0x0A) ;
    WriteReg(REG_TSLB, 0x04);                // 0D = UYVY  04 = YUYV     
    WriteReg(REG_COM13, 0x88);               // connect to REG_TSLB
    
        
    if((strcmp("BAW", format) == 0) || (strcmp("YUV", format) == 0)|| (strcmp("RAW", format) == 0))     // YUV
    {
        WriteReg(REG_COM7, 0x00);           // YUV
        WriteReg(REG_COM17, 0x00);          // color bar disable
        WriteReg(REG_COM3, 0x04);
        WriteReg(REG_COM15, 0xC0);          // Set normal rgb with Full range

    }else
    if((strcmp("RGB", format) == 0))                // RGB565
    {                                       
        WriteReg(REG_COM7, 0x04);           // RGB + color bar disable 
        WriteReg(REG_RGB444, 0x00);         // Disable RGB444
        WriteReg(REG_COM15, 0x10);          // Set rgb565 with Full range    0xD0
        WriteReg(REG_COM3, 0x04);
       // 
    }
      
    //WriteReg(REG_CLKRC, 0xA0);              // prescaler x
    WriteReg(0x70, 0x3A);                   // Scaling Xsc
    WriteReg(0x71, 0x35);                   // Scaling Ysc
    //WriteReg(0xA2, 0x02);                   // pixel clock delay

    if(n == 19200)              // 160*120
    {
        WriteReg(REG_COM14, 0x1a);          // divide by 4
        WriteReg(0x72, 0x22);               // downsample by 4
        WriteReg(0x73, 0xf2);               // divide by 4
        WriteReg(REG_HREF, 0xa4);
        WriteReg(REG_HSTART, 0x16);
        WriteReg(REG_HSTOP, 0x04);
        WriteReg(REG_VREF, 0x0a);   
        WriteReg(REG_VSTART, 0x02);
        WriteReg(REG_VSTOP, 0x7a);        
             
        WriteReg(0x7a, 0x20);
        WriteReg(0x7b, 0x1c);
        WriteReg(0x7c, 0x28);
        WriteReg(0x7d, 0x3c);
        WriteReg(0x7e, 0x5a);
        WriteReg(0x7f, 0x68);
        WriteReg(0x80, 0x76);
        WriteReg(0x81, 0x80);
        WriteReg(0x82, 0x88);
        WriteReg(0x83, 0x8f);
        WriteReg(0x84, 0x96);
        WriteReg(0x85, 0xa3);
        WriteReg(0x86, 0xaf);
        WriteReg(0x87, 0xc4);
        WriteReg(0x88, 0xd7);
        WriteReg(0x89, 0xe8);
        
        WriteReg(0x13, 0xe0);
        WriteReg(0x00, 0x00);
        WriteReg(0x10, 0x00);
        WriteReg(0x0d, 0x40);
        WriteReg(0x14, 0x18);
        WriteReg(0xa5, 0x05);
        WriteReg(0xab, 0x07);
        WriteReg(0x24, 0x95);
        WriteReg(0x25, 0x33);
        WriteReg(0x26, 0xe3);
        WriteReg(0x9f, 0x78);
        WriteReg(0xa0, 0x68);
        WriteReg(0xa1, 0x03);
        WriteReg(0xa6, 0xd8);
        WriteReg(0xa7, 0xd8);
        WriteReg(0xa8, 0xf0);
        WriteReg(0xa9, 0x90);
        WriteReg(0xaa, 0x94);
        WriteReg(0x13, 0xe5);
        
        WriteReg(0x0e, 0x61);
        WriteReg(0x0f, 0x4b);
        WriteReg(0x16, 0x02);

        WriteReg(0x21, 0x02);
        WriteReg(0x22, 0x91);
        WriteReg(0x29, 0x07);
        WriteReg(0x33, 0x0b);
        WriteReg(0x35, 0x0b);
        WriteReg(0x37, 0x1d);
        WriteReg(0x38, 0x71);
        WriteReg(0x39, 0x2a);
        WriteReg(0x3c, 0x78);
        WriteReg(0x4d, 0x40);
        WriteReg(0x4e, 0x20);
        WriteReg(0x69, 0x00);

        WriteReg(0x74, 0x10);
        WriteReg(0x8d, 0x4f);
        WriteReg(0x8e, 0x00);
        WriteReg(0x8f, 0x00);
        WriteReg(0x90, 0x00);
        WriteReg(0x91, 0x00);
        WriteReg(0x92, 0x00);

        WriteReg(0x96, 0x00);
        WriteReg(0x9a, 0x80);
        WriteReg(0xb0, 0x84);
        WriteReg(0xb1, 0x0c);
        WriteReg(0xb2, 0x0e);
        WriteReg(0xb3, 0x82);
        WriteReg(0xb8, 0x0a);
        
        WriteReg(0x43, 0x0a);
        WriteReg(0x44, 0xf0);
        WriteReg(0x45, 0x34);
        WriteReg(0x46, 0x58);
        WriteReg(0x47, 0x28);
        WriteReg(0x48, 0x3a);
        WriteReg(0x59, 0x88);
        WriteReg(0x5a, 0x88);
        WriteReg(0x5b, 0x44);
        WriteReg(0x5c, 0x67);
        WriteReg(0x5d, 0x49);
        WriteReg(0x5e, 0x0e);
        WriteReg(0x64, 0x04);
        WriteReg(0x65, 0x20);
        WriteReg(0x66, 0x05);
        WriteReg(0x94, 0x04);
        WriteReg(0x95, 0x08);

        WriteReg(0x6c, 0x0a);
        WriteReg(0x6d, 0x55);
        WriteReg(0x6e, 0x11);
        WriteReg(0x6f, 0x9f);
        WriteReg(0x6a, 0x40);
        WriteReg(0x01, 0x40);
        WriteReg(0x02, 0x40);
        WriteReg(0x13, 0xe7);
        WriteReg(0x15, 0x02);

        WriteReg(0x4f, 0x80);
        WriteReg(0x50, 0x80);
        WriteReg(0x51, 0x00);
        WriteReg(0x52, 0x22);
        WriteReg(0x53, 0x5e);
        WriteReg(0x54, 0x80);
        WriteReg(0x58, 0x9e);
        
        WriteReg(0x41, 0x08);
        WriteReg(0x3f, 0x00);
        WriteReg(0x75, 0x05);
        WriteReg(0x76, 0xe1);
        WriteReg(0x4c, 0x00);
        WriteReg(0x77, 0x01);
        WriteReg(0x3d, 0xc1);
        WriteReg(0x4b, 0x09);
        WriteReg(0xc9, 0x60);
        WriteReg(0x41, 0x38);
        WriteReg(0x56, 0x40);
        
        WriteReg(0x34, 0x11);
        WriteReg(0x3b, 0x02);
        WriteReg(0xa4, 0x88);
        WriteReg(0x96, 0x00);
        WriteReg(0x97, 0x30);
        WriteReg(0x98, 0x20);
        WriteReg(0x99, 0x30);
        WriteReg(0x9a, 0x84);
        WriteReg(0x9b, 0x29);
        WriteReg(0x9c, 0x03);
        WriteReg(0x9d, 0x4c);
        WriteReg(0x9e, 0x3f);
        WriteReg(0x78, 0x04);
        
        WriteReg(0x79, 0x01);
        WriteReg(0xc8, 0xf0);
        WriteReg(0x79, 0x0f);
        WriteReg(0xc8, 0x00);
        WriteReg(0x79, 0x10);
        WriteReg(0xc8, 0x7e);
        WriteReg(0x79, 0x0a);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x0b);
        WriteReg(0xc8, 0x01);
        WriteReg(0x79, 0x0c);
        WriteReg(0xc8, 0x0f);
        WriteReg(0x79, 0x0d);
        WriteReg(0xc8, 0x20);
        WriteReg(0x79, 0x09);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x02);
        WriteReg(0xc8, 0xc0);
        WriteReg(0x79, 0x03);
        WriteReg(0xc8, 0x40);
        WriteReg(0x79, 0x05);
        WriteReg(0xc8, 0x30);
        WriteReg(0x79, 0x26);
        WriteReg(0x09, 0x03);
        WriteReg(0x3b, 0x42);
        
        WriteReg(0xff, 0xff);   /* END MARKER */ 
               
    }    
    if(n == 76800)              // 320*240
    {
        WriteReg(REG_COM14, 0x19);         
        WriteReg(0x72, 0x11);
        WriteReg(0x73, 0xf1);
        WriteReg(REG_HREF, 0x24); 
        WriteReg(REG_HSTART, 0x16);
        WriteReg(REG_HSTOP, 0x04);
        WriteReg(REG_VREF, 0x0a);           
        WriteReg(REG_VSTART,0x02);
        WriteReg(REG_VSTOP, 0x7a);  
        
        WriteReg(0x7a, 0x20);
        WriteReg(0x7b, 0x1c);
        WriteReg(0x7c, 0x28);
        WriteReg(0x7d, 0x3c);
        WriteReg(0x7e, 0x55);
        WriteReg(0x7f, 0x68);
        WriteReg(0x80, 0x76);
        WriteReg(0x81, 0x80);
        WriteReg(0x82, 0x88);
        WriteReg(0x83, 0x8f);
        WriteReg(0x84, 0x96);
        WriteReg(0x85, 0xa3);
        WriteReg(0x86, 0xaf);
        WriteReg(0x87, 0xc4);
        WriteReg(0x88, 0xd7);
        WriteReg(0x89, 0xe8);
        
        WriteReg(0x13, 0xe0);
        WriteReg(0x00, 0x00);
        WriteReg(0x10, 0x00);
        WriteReg(0x0d, 0x00);
        WriteReg(0x14, 0x28);
        WriteReg(0xa5, 0x05);
        WriteReg(0xab, 0x07);
        WriteReg(0x24, 0x75);
        WriteReg(0x25, 0x63);
        WriteReg(0x26, 0xA5);
        WriteReg(0x9f, 0x78);
        WriteReg(0xa0, 0x68);
        WriteReg(0xa1, 0x03);
        WriteReg(0xa6, 0xdf);
        WriteReg(0xa7, 0xdf);
        WriteReg(0xa8, 0xf0);
        WriteReg(0xa9, 0x90);
        WriteReg(0xaa, 0x94);      
        WriteReg(0x13, 0xe5);
        
        WriteReg(0x0e, 0x61);
        WriteReg(0x0f, 0x4b);
        WriteReg(0x16, 0x02);
        WriteReg(0x21, 0x02);
        WriteReg(0x22, 0x91);
        WriteReg(0x29, 0x07);
        WriteReg(0x33, 0x0b);
        WriteReg(0x35, 0x0b);
        WriteReg(0x37, 0x1d);
        WriteReg(0x38, 0x71);
        WriteReg(0x39, 0x2a);
        WriteReg(0x3c, 0x78);
        WriteReg(0x4d, 0x40);
        WriteReg(0x4e, 0x20);
        WriteReg(0x69, 0x00);
        WriteReg(0x6b, 0x00);
        WriteReg(0x74, 0x19);
        WriteReg(0x8d, 0x4f);
        WriteReg(0x8e, 0x00);
        WriteReg(0x8f, 0x00);
        WriteReg(0x90, 0x00);
        WriteReg(0x91, 0x00);
        WriteReg(0x92, 0x00);
        WriteReg(0x96, 0x00);
        WriteReg(0x9a, 0x80);
        WriteReg(0xb0, 0x84);
        WriteReg(0xb1, 0x0c);
        WriteReg(0xb2, 0x0e);
        WriteReg(0xb3, 0x82);
        WriteReg(0xb8, 0x0a);
        WriteReg(0x43, 0x14);
        WriteReg(0x44, 0xf0);
        WriteReg(0x45, 0x34);
        WriteReg(0x46, 0x58);
        WriteReg(0x47, 0x28);
        WriteReg(0x48, 0x3a);
        WriteReg(0x59, 0x88);
        WriteReg(0x5a, 0x88);
        WriteReg(0x5b, 0x44);
        WriteReg(0x5c, 0x67);
        WriteReg(0x5d, 0x49);
        WriteReg(0x5e, 0x0e);
        WriteReg(0x64, 0x04);
        WriteReg(0x65, 0x20);
        WriteReg(0x66, 0x05);
        WriteReg(0x94, 0x04);
        WriteReg(0x95, 0x08);
        WriteReg(0x6c, 0x0a);
        WriteReg(0x6d, 0x55);
        WriteReg(0x6e, 0x11);
        WriteReg(0x6f, 0x9f);
        WriteReg(0x6a, 0x40);
        WriteReg(0x01, 0x40);
        WriteReg(0x02, 0x40);
        WriteReg(0x13, 0xe7);
        WriteReg(0x15, 0x02);
        WriteReg(0x4f, 0x80);
        WriteReg(0x50, 0x80);
        WriteReg(0x51, 0x00);
        WriteReg(0x52, 0x22);
        WriteReg(0x53, 0x5e);
        WriteReg(0x54, 0x80);
        WriteReg(0x58, 0x9e);   
        WriteReg(0x41, 0x08);
        WriteReg(0x3f, 0x00);
        WriteReg(0x75, 0x05);
        WriteReg(0x76, 0xe1);
        WriteReg(0x4c, 0x00);
        WriteReg(0x77, 0x01);
        WriteReg(0x3d, 0xc2);  
        WriteReg(0x4b, 0x09);
        WriteReg(0xc9, 0x60);
        WriteReg(0x41, 0x38);
        WriteReg(0x56, 0x40);
        WriteReg(0x34, 0x11);
        WriteReg(0x3b, 0x02);
        WriteReg(0xa4, 0x89);
        WriteReg(0x96, 0x00);
        WriteReg(0x97, 0x30);
        WriteReg(0x98, 0x20);
        WriteReg(0x99, 0x30);
        WriteReg(0x9a, 0x84);
        WriteReg(0x9b, 0x29);
        WriteReg(0x9c, 0x03);
        WriteReg(0x9d, 0x4c);
        WriteReg(0x9e, 0x3f);
        WriteReg(0x78, 0x04);   
        WriteReg(0x79, 0x01);
        WriteReg(0xc8, 0xf0);
        WriteReg(0x79, 0x0f);
        WriteReg(0xc8, 0x00);
        WriteReg(0x79, 0x10);
        WriteReg(0xc8, 0x7e);
        WriteReg(0x79, 0x0a);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x0b);
        WriteReg(0xc8, 0x01);
        WriteReg(0x79, 0x0c);
        WriteReg(0xc8, 0x0f);
        WriteReg(0x79, 0x0d);
        WriteReg(0xc8, 0x20);
        WriteReg(0x79, 0x09);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x02);
        WriteReg(0xc8, 0xc0);
        WriteReg(0x79, 0x03);
        WriteReg(0xc8, 0x40);
        WriteReg(0x79, 0x05);
        WriteReg(0xc8, 0x30);
        WriteReg(0x79, 0x26);
        WriteReg(0x09, 0x03);
        WriteReg(0x3b, 0x42);
        
        WriteReg(0xff, 0xff);   /* END MARKER */ 
        
    }
    if(n == 307200)             // 640*480    
    { 
        WriteReg(REG_CLKRC, 0x40);  // no clock prescaler
        //WriteReg(REG_CLKRC, 0x01);     
        WriteReg(REG_TSLB,  0x04);    
        WriteReg(REG_COM7, 0x01);        
        WriteReg(DBLV, 0x4a); 
        WriteReg(REG_COM3, 0);        
        WriteReg(REG_COM14, 0);
        
        WriteReg(REG_HSTART, 0x13);   
        WriteReg(REG_HSTOP, 0x01);
        WriteReg(REG_HREF, 0xb6);     
        WriteReg(REG_VSTART, 0x02);
        WriteReg(REG_VSTOP, 0x7a);    
        WriteReg(REG_VREF, 0x0a);
        WriteReg(0x72, 0x11);         
        WriteReg(0x73, 0xf0);  
        
        /* Gamma curve values */
        WriteReg(0x7a, 0x20);         
        WriteReg(0x7b, 0x10);
        WriteReg(0x7c, 0x1e);         
        WriteReg(0x7d, 0x35);
        WriteReg(0x7e, 0x5a);         
        WriteReg(0x7f, 0x69);
        WriteReg(0x80, 0x76);         
        WriteReg(0x81, 0x80);
        WriteReg(0x82, 0x88);         
        WriteReg(0x83, 0x8f);
        WriteReg(0x84, 0x96);         
        WriteReg(0x85, 0xa3);
        WriteReg(0x86, 0xaf);         
        WriteReg(0x87, 0xc4);
        WriteReg(0x88, 0xd7);         
        WriteReg(0x89, 0xe8);
        
        /* AGC and AEC parameters.  Note we start by disabling those features,
        then turn them only after tweaking the values. */
        WriteReg(0x13, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT);
        WriteReg(0x00, 0);        
        WriteReg(0x10, 0);
        WriteReg(0x0d, 0x40); 
        WriteReg(0x14, 0x18); 
        WriteReg(0xa5, 0x05);  
        WriteReg(0xab, 0x07);
        WriteReg(0x24, 0x95);      
        WriteReg(0x25, 0x33);
        WriteReg(0x26, 0xe3);      
        WriteReg(0x9f, 0x78);
        WriteReg(0xa0, 0x68);   
        WriteReg(0xa1, 0x03); 
        WriteReg(0xa6, 0xd8);   
        WriteReg(0xa7, 0xd8);
        WriteReg(0xa8, 0xf0);   
        WriteReg(0xa9, 0x90);
        WriteReg(0xaa, 0x94);
        WriteReg(0x13, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC);  
        
        /* Almost all of these are magic "reserved" values.  */    
        WriteReg(0x0e, 0x61);     
        WriteReg(0x0f, 0x4b);
        WriteReg(0x16, 0x02);        
        WriteReg(0x1e, 0x27);
        WriteReg(0x21, 0x02);         
        WriteReg(0x22, 0x91);
        WriteReg(0x29, 0x07);         
        WriteReg(0x33, 0x0b);
        WriteReg(0x35, 0x0b);         
        WriteReg(0x37, 0x1d);
        WriteReg(0x38, 0x71);         
        WriteReg(0x39, 0x2a);
        WriteReg(0x3c, 0x78);    
        WriteReg(0x4d, 0x40);
        WriteReg(0x4e, 0x20);         
        WriteReg(0x69, 0);
        WriteReg(0x6b, 0x0a);         
        WriteReg(0x74, 0x10);
        WriteReg(0x8d, 0x4f);         
        WriteReg(0x8e, 0);
        WriteReg(0x8f, 0);            
        WriteReg(0x90, 0);
        WriteReg(0x91, 0);            
        WriteReg(0x96, 0);
        WriteReg(0x9a, 0);          
        WriteReg(0xb0, 0x84);
        WriteReg(0xb1, 0x0c);         
        WriteReg(0xb2, 0x0e);
        WriteReg(0xb3, 0x82);         
        WriteReg(0xb8, 0x0a);
        
        /* More reserved magic, some of which tweaks white balance */
        WriteReg(0x43, 0x0a);         
        WriteReg(0x44, 0xf0);
        WriteReg(0x45, 0x34);         
        WriteReg(0x46, 0x58);
        WriteReg(0x47, 0x28);         
        WriteReg(0x48, 0x3a);
        WriteReg(0x59, 0x88);         
        WriteReg(0x5a, 0x88);
        WriteReg(0x5b, 0x44);         
        WriteReg(0x5c, 0x67);
        WriteReg(0x5d, 0x49);         
        WriteReg(0x5e, 0x0e);
        WriteReg(0x6c, 0x0a);         
        WriteReg(0x6d, 0x55);
        WriteReg(0x6e, 0x11);         
        WriteReg(0x6f, 0x9f); 
        WriteReg(0x6a, 0x40);         
        WriteReg(0x01, 0x40);
        WriteReg(0x02, 0x60);
        WriteReg(0x13, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC|COM8_AWB);  
        
        /* Matrix coefficients */
        WriteReg(0x4f, 0x80);         
        WriteReg(0x50, 0x80);
        WriteReg(0x51, 0);            
        WriteReg(0x52, 0x22);
        WriteReg(0x53, 0x5e);         
        WriteReg(0x54, 0x80);
        WriteReg(0x58, 0x9e);
        
        WriteReg(0x41, 0x08);   
        WriteReg(0x3f, 0);
        WriteReg(0x75, 0x05);         
        WriteReg(0x76, 0xe1);
        WriteReg(0x4c, 0);            
        WriteReg(0x77, 0x01);
        WriteReg(0x3d, 0xc3);    
        WriteReg(0x4b, 0x09);
        WriteReg(0xc9, 0x60);         
        WriteReg(0x41, 0x38);
        WriteReg(0x56, 0x40);
        
        WriteReg(0x34, 0x11);         
        WriteReg(0x3b, COM11_EXP|COM11_HZAUTO);
        WriteReg(0xa4, 0x88);         
        WriteReg(0x96, 0);
        WriteReg(0x97, 0x30);         
        WriteReg(0x98, 0x20);
        WriteReg(0x99, 0x30);         
        WriteReg(0x9a, 0x84);
        WriteReg(0x9b, 0x29);         
        WriteReg(0x9c, 0x03);
        WriteReg(0x9d, 0x4c);         
        WriteReg(0x9e, 0x3f);
        WriteReg(0x78, 0x04);
        
        /* Extra-weird stuff.  Some sort of multiplexor register */
        WriteReg(0x79, 0x01);         
        WriteReg(0xc8, 0xf0);
        WriteReg(0x79, 0x0f);         
        WriteReg(0xc8, 0x00);
        WriteReg(0x79, 0x10);         
        WriteReg(0xc8, 0x7e);
        WriteReg(0x79, 0x0a);         
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x0b);         
        WriteReg(0xc8, 0x01);
        WriteReg(0x79, 0x0c);         
        WriteReg(0xc8, 0x0f);
        WriteReg(0x79, 0x0d);         
        WriteReg(0xc8, 0x20);
        WriteReg(0x79, 0x09);         
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x02);         
        WriteReg(0xc8, 0xc0);
        WriteReg(0x79, 0x03);         
        WriteReg(0xc8, 0x40);
        WriteReg(0x79, 0x05);         
        WriteReg(0xc8, 0x30);
        WriteReg(0x79, 0x26);
        
        WriteReg(0xff, 0xff); /* END MARKER */                 
    }
    
    m_VSync.rise( this, &OV7670Camera::VSyncRiseHandler );
    m_VSync.fall( this, &OV7670Camera::VSyncFallHandler );
       
    return 1; 
}

char OV7670Camera::ReadByte( void )
{
    return static_cast<char>( m_Data.read() );
}

void OV7670Camera::VSyncRiseHandler( void )
{
    VSyncRise = true;
    pc.printf( "VSync::Rise\n" );
}

void OV7670Camera::VSyncFallHandler( void )
{
    VSyncFall = true;
    pc.printf( "VSync::Fall\n" );
}

void OV7670Camera::HRefRiseHandler( void )
{
    HRefRise = true;
    pc.printf( "HRef::Rise\n" );
}

void OV7670Camera::EnableHRefRiseHandler( void )
{
    m_HRef.rise( this, &OV7670Camera::HRefRiseHandler );
}

void OV7670Camera::DisableHRefRiseHandler( void )
{
    m_HRef.rise( NULL );
}