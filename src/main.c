
#include <sys/types.h>
#include <stdio.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <libapi.h>
#include <malloc.h>
#include <libcd.h>
#include "../thirdparty/nugget/common/kernel/pcdrv.h"

#define VMODE 0                 // Video Mode : 0 : NTSC, 1: PAL
#define SCREENXRES 320          // Screen width
#define SCREENYRES 240          // Screen height
#define CENTERX SCREENXRES/2    // Center of screen on x 
#define CENTERY SCREENYRES/2    // Center of screen on y
#define MARGINX 0                // margins for text display
#define MARGINY 32
#define FONTSIZE 8 * 7             // Text Field Height
#define OTLEN 8                    // Ordering Table Length 
DISPENV disp[2];                   // Double buffered DISPENV and DRAWENV
DRAWENV draw[2];
u_long ot[2][OTLEN];               // double ordering table of length 8 * 32 = 256 bits / 32 bytes
char primbuff[2][32768];     // double primitive buffer of length 32768 * 8 =  262.144 bits / 32,768 Kbytes
char *nextpri = primbuff[0];       // pointer to the next primitive in primbuff. Initially, points to the first bit of primbuff[0]
short db = 0;                      // index of which buffer is used, values 0, 1


// For CD Access
// CD specifics
#define CD_SECTOR_SIZE 2048
// Converting bytes to sectors SECTOR_SIZE is defined in words, aka int
#define BtoS(len) ( ( len + CD_SECTOR_SIZE - 1 ) / CD_SECTOR_SIZE )

static unsigned char ramAddr[0x40000]; // 256KB buffer 
u_long * cdDataBuffer;    
// Value returned by CDread() - 1 is good, 0 is bad
int CDreadOK = 0;
// Value returned by CDsync() - Returns remaining sectors to load. 0 is good.
int CDreadResult = 0;      
  // Those are not strictly needed, but we'll use them to see the commands results.
// They could be replaced by a 0 in the various functions they're used with.
u_char cdCtrlResult[8];
// libcd's CD file structure contains size, location and filename
CdlFILE filePos = {0};

// TIM files will get objcopyed into the .rodata section of the ELF file
// objcopy generates these symobls automatically
// extern means that the symbols are defined elsewhere

extern ulong _binary_assets_tim_tex64_tim_start[];
extern ulong _binary_assets_tim_tex64_tim_end[];
extern ulong _binary_assets_tim_tex64_tim_size[];



#define CLUT_MASK 0x8

// General 8KB Buffer
char buffer[8192]; 

// Where the TIM texture will be loaded
TIM_IMAGE tex64;


void LoadTexture(ulong *tim_addr, TIM_IMAGE *tim_image)
{
    int error = OpenTIM(tim_addr);
    if(error)
    {
        printf("Error opening TIM file\n");
    }

    ReadTIM(tim_image);
    error = LoadImage(tim_image->prect, tim_image->paddr);
    if(error)
    {
        printf("Error loading image from tim image of file\n");
    }
    DrawSync(0);
    if(tim_image->mode & CLUT_MASK)
    {
        LoadImage(tim_image->crect, tim_image->caddr);
        DrawSync(0);
    }

}

// Loading texture functions

void LoadTextureFromPCFileServer(char *filename, TIM_IMAGE *tim_image)
{
    int err;

    int fd = PCopen(filename, 0, 0);
    if(err)
    {
        printf("Error opening file\n");
    }
    int size = PClseek(fd, 0, 2);
    PClseek(fd, 0, 0);
    int bytes_read = PCread(fd, buffer, size);
    if(bytes_read < 0)
    {
        printf("Error reading file\n");
    }

    LoadTexture((ulong *) buffer, &tex64);
}

void LoadTextureFromMemory(u_long *tim_addr, TIM_IMAGE *tim_image)
{
    LoadTexture(tim_addr, tim_image);
}


void LoadTextureFromCD(char *filename, TIM_IMAGE *tim_image)
{
    printf("Loading texture from CD\n");
    // CD Stuff //
    CdSearchFile(&filePos, filename);

    cdDataBuffer = malloc( BtoS(filePos.size) * CD_SECTOR_SIZE );
    CdControl(CdlSetloc, (u_char *)&filePos, cdCtrlResult);

    // read data and load it to data buffer
    CDreadOK = CdRead( (int)BtoS(filePos.size), (u_long *)cdDataBuffer, CdlModeSpeed);
    
    // wait for operation to complete
    CDreadResult = CdReadSync( 0, 0);

    LoadTexture(cdDataBuffer, tim_image);
    
}

// Initialization

void init(void)
{
    ResetGraph(0);
    // Initialize and setup the GTE
    InitGeom();
    SetGeomOffset(CENTERX,CENTERY);
    SetGeomScreen(CENTERX);
    SetDefDispEnv(&disp[0], 0, 0, SCREENXRES, SCREENYRES);
    SetDefDispEnv(&disp[1], 0, SCREENYRES, SCREENXRES, SCREENYRES);
    SetDefDrawEnv(&draw[0], 0, SCREENYRES, SCREENXRES, SCREENYRES);
    SetDefDrawEnv(&draw[1], 0, 0, SCREENXRES, SCREENYRES);
    if (VMODE) {
        SetVideoMode(MODE_PAL);
        disp[0].disp.y = disp[1].disp.y = 8;
    }
    SetDispMask(1);                 
    setRGB0(&draw[0], 50, 50, 50);
    setRGB0(&draw[1], 50, 50, 50);
    draw[0].isbg = 1;
    draw[1].isbg = 1;
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    FntLoad(960, 0);
    FntOpen(MARGINX, SCREENYRES - MARGINY - FONTSIZE, SCREENXRES - MARGINX * 2, FONTSIZE, 0, 280 );

    int err;

    // pcrdv init
    // err = PCinit();
    // if(err)
    // {
    //     printf("Error initializing PC Fileserver\n");
    // }

    // cd-rom init
    err = CdInit();
    if(err == 0)
    {
        printf("Error initializing CD-ROM\n");
    }

    InitHeap((u_long *)ramAddr, sizeof(ramAddr));

    //LoadTextureFromMemory(_binary_assets_tim_tex64_tim_start, &tex64);
// #ifdef CD_ROM_BUILD
    LoadTextureFromCD("\\TIM\\TEX64.TIM;1", &tex64);
// #else 
    //LoadTextureFromPCFileServer("tim/tex64.tim", &tex64);
// #endif

}

// Display

void display(void)
{
    DrawSync(0);
    VSync(0);
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    DrawOTag(&ot[db][OTLEN - 1]);
    db = !db;
    nextpri = primbuff[db];
}

// Main

int main(void)
{
    // sprite stuff
    SPRT *sprt_tex64;
    DR_TPAGE *tpage_tex64;

    // rotating rectangle stuff
    SVECTOR RotVector = {0, 0, 0};                  // Initialize rotation vector {x, y, z}
    VECTOR  MovVector = {0, 0, CENTERX, 0};         // Initialize translation vector {x, y, z}
    VECTOR  ScaleVector = {ONE, ONE, ONE};           // ONE is define as 4096 in libgte.h
    SVECTOR VertPos[4] = {                          // Set initial vertices position relative to 0,0 - see here : https://psx.arthus.net/docs/poly_f4.jpg
            {-32, -32, 1 },                         // Vert 1 
            {-32,  32, 1 },                         // Vert 2
            { 32, -32, 1 },                         // Vert 3
            { 32,  32, 1  }                         // Vert 4
        };                                          
    MATRIX PolyMatrix = {0};    
    POLY_F4 *poly = {0};                            // pointer to a POLY_F4 
               
    long polydepth;
    long polyflag;
    long OTz;
    init();

    // sprite stuff
   // LoadTexture(_binary_assets_tim_tex64_tim_start, &tex64);
    while (1)
    {
        ClearOTagR(ot[db], OTLEN);
        // poly = (POLY_F4 *)nextpri;                    // Set poly to point to  the address of the next primitiv in the buffer
        // // Set transform matrices for this polygon
        // RotMatrix(&RotVector, &PolyMatrix);           // Apply rotation matrix
        // TransMatrix(&PolyMatrix, &MovVector);
        // ScaleMatrix(&PolyMatrix, &ScaleVector);         // Apply translation matrix   
        // SetRotMatrix(&PolyMatrix);                    // Set default rotation matrix
        // SetTransMatrix(&PolyMatrix);                  // Set default transformation matrix
        // setPolyF4(poly);                              // Initialize poly as a POLY_F4 
        // setRGB0(poly, 255, 0, 255);                   // Set poly color
        // // RotTransPers using 4 calls
        // //~ OTz = RotTransPers(&VertPos[0], (long*)&poly->x0, &polydepth, &polyflag);
        // //~ RotTransPers(&VertPos[1], (long*)&poly->x1, &polydepth, &polyflag);
        // //~ RotTransPers(&VertPos[2], (long*)&poly->x2, &polydepth, &polyflag);
        // //~ RotTransPers(&VertPos[3], (long*)&poly->x3, &polydepth, &polyflag);
        // // RotTransPers4 equivalent 
        // OTz = RotTransPers4(
        //             &VertPos[0],      &VertPos[1],      &VertPos[2],      &VertPos[3],
        //             (long*)&poly->x0, (long*)&poly->x1, (long*)&poly->x2, (long*)&poly->x3,
        //             &polydepth,
        //             &polyflag
        //             );                                // Perform coordinate and perspective transformation for 4 vertices
        // RotVector.vy += 4;
        // RotVector.vz += 4;                              // Apply rotation on Z-axis. On PSX, the Z-axis is pointing away from the screen.  
        // addPrim(ot[db], poly);                         // add poly to the Ordering table
        // nextpri += sizeof(POLY_F4);                    // increment nextpri address with size of a POLY_F4 struct 
        // FntPrint("Hello Poly !");                   
        // FntFlush(-1);

        //spriter stuff
        sprt_tex64 = (SPRT *)nextpri;
        setSprt(sprt_tex64);
        setRGB0(sprt_tex64, 128, 128, 128);
        setClut(sprt_tex64, tex64.crect->x, tex64.crect->y);
        setXY0(sprt_tex64, 0, 0);
        setWH(sprt_tex64, 64, 64);
        addPrim(ot[db], sprt_tex64);
        nextpri += sizeof(SPRT);
        tpage_tex64 = (DR_TPAGE *)nextpri;
        setDrawTPage(tpage_tex64, 0, 1, 
            getTPage(tex64.mode&0x3, 0, tex64.prect->x, tex64.prect->y)
        );
        addPrim(ot[db], tpage_tex64);
        nextpri += sizeof(DR_TPAGE);
        display();
    }
    return 0;
}
