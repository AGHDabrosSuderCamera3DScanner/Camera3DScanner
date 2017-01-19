#include "mbed.h"
#include "OV7670.h"
#include "Clock.h"
#include "UIPUdp.h"
#include "UIPEthernet.h"

Serial pc( USBTX, USBRX );
/*7670Camera Camera(
    PB_8,  //SCL
    PB_9,  //SDA
    PA_5,  //VSYNC
    PA_6,  //HREF
    PC_7,  //D7
    PA_9,  //D6
    PA_8,  //D5
    PB_10, //D4
    PB_4,  //D3
    PB_5,  //D2
    PB_3,  //D1
    PA_10  //D0
);*/
//I2C i2c( PB_9, PB_8 );
//PwmOut BoardClk( PA_11 );

volatile bool VSyncRise = false;
volatile bool VSyncFall = false;
volatile bool HRefRise = false;



DigitalOut led( LED2 );
PortIn port( PortC, 0xFF ); // PC_0 - PC_7

// mosi, miso, sck, cs
UIPEthernetClass UIPEthernet( PB_15, PB_14, PB_13, PB_1 );
UIPUDP UdpClient;
uint8_t mac[] = { 0x12, 0x23, 0x34, 0x45, 0x56, 0x67 };
uint8_t ip[] = { 168, 192, 167, 3 };

int main()
{
    // initializing pc serial connection
    pc.baud( 115200 );
    pc.format();
    pc.printf( "starting\r\n" );

    wait( 1 );

    UIPEthernet.begin( mac, IPAddress( ip ) );
    pc.printf( "UIPEthernet initialized\r\n" );

    int result = 0;
    UdpClient.beginPacket( IPAddress( 168, 192, 33, 3 ), 35000 );

    result = UdpClient.write( reinterpret_cast<const uint8_t*>( "dupa" ), 4 );
    pc.printf( "Packet written: %d\r\n", result );
    
    result = UdpClient.endPacket();
    pc.printf( "Packet sent: %s\r\n", result ? "true" : "false" );

    while( 1 )
    {
        UIPEthernet.maintain();
        wait( 1 );
    }

    /*
    // initializing camera
    if( Camera.Init( "RGB", 640, 480 ) == 0 )
    {
        while( 1 )
        {
            pc.printf( "Init failed!\r\n" );
            wait( 0.5 );
        }
    }
    wait( 1 );

    while( 1 )
    {
        VSyncFall = false;
        VSyncRise = false;
        HRefRise = false;

        int count = 0;
       // BoardClk.Interval_us( 1 );

        // wait for VSync fall
        while( !VSyncFall )
        {
           // BoardClk.Cycle();
            count++;
        }

        VSyncFall = false;
        pc.printf( "# of Tpclk: %d\r\n", count );
        count = 0;

        // wait for VSync rise
        while( !VSyncRise )
        {
           // BoardClk.Cycle();
            count++;
        }

        VSyncRise = false;
        pc.printf( "# of Tpclk: %d\r\n", count );
        count = 0;

        pc.printf( "\r\nready for image!\r\n" );
        Camera.EnableHRefRiseHandler();

        // wait for first HRef rise
        HRefRise = false;
        while( !HRefRise )
        {
          //  BoardClk.Cycle();
            count++;
        }

        HRefRise = false;
        Camera.DisableHRefRiseHandler();
        pc.printf( "# of Tpclk: %d\r\n", count );
        count = 0;
        Camera.DisableVSyncFallHandler();
        Camera.DisableVSyncRiseHandler();

        // get image
        //BoardClk.Interval_us( 1 );

       // pc.printf( "%d\r\n", BoardClk.Get() );

        //BoardClk.Switch();
       // for( int i = 0; i < 240; i++ )
        uint16_t data = 0;
        uint16_t Row[ 784 ];
        int i, j;
        while(1)
        {
            memset( Row, 0, 784 * sizeof( uint16_t ) );

            // get line
            for( j = 0; j < 784; j++ )
            {
                
               // BoardClk.Set_NoWait(0);
                data = Camera.ReadByte() * 256;
                wait_us( 1 );
               // BoardClk.Set_NoWait( 1 );
                data += Camera.ReadByte();
                //Row[ j ] = ReadPixel();
                Row[ j ] = data;
            }

            for( j = 0; j < 784; j++ )
            {
                pc.printf("%0.4X ", Row[j]);
            }
            pc.printf( "\r\n" );

        }

        while( 1 ) wait_ms( 1 );
    }
    */

    return 0;
}