#include <sys/types.h>
#include <stdio.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "bulb.c"
#include "../thirdparty/nugget/common/kernel/pcdrv.h"


#define MAX_OBJECTS

#define SCREENXRES 320
#define SCREENYRES 240
#define CENTERX SCREENXRES/2
#define CENTERY SCREENYRES/2
#define DITHER 1

#define OTLEN 12
#define OTENTRIES 1<<OTLEN //4096
#define PACKETMAX 2048

GsOT myOT[2];
GsOT_TAG myOT_TAG[2][OTENTRIES];
PACKET myPacketArea[2][PACKETMAX*24];
int myActiveBuff = 0;

GsDOBJ2 Objects[MAX_OBJECTS] = {0};
int ObjectCount = 0;

GsF_LIGHT pslt[1];

// General 8KB Buffer
char buffer[8192]; 
u_long buffer_offset = 0;

struct {
	int		x,y,z;
	int		pan,til,rol;
	VECTOR	pos;
	SVECTOR rot;
	GsRVIEW2 view;
	GsCOORDINATE2 coord2;
} Camera = {0};

struct {
    int x,xv;
    int y,yv;
    int z,zv;
    int pan,panv;
    int til,tilv;
} Player = {0};

int main();
void PutObject(VECTOR pos, SVECTOR rot, GsDOBJ2 *obj);
int LinkModel(u_long *tmd, GsDOBJ2 *obj);
void init();
void PrepDisplay();
void Display();

void init(){
    SVECTOR VScale={0};

    ResetGraph(0);

    GsInitGraph(SCREENXRES, SCREENYRES, GsINTER|GsOFSGPU, DITHER, 0);
    GsDefDispBuff(0, 0, 0, 0);

    myOT[0].length = OTLEN;
    myOT[1].length = OTLEN;
    myOT[0].org = myOT_TAG[0];
    myOT[1].org = myOT_TAG[1];

    GsClearOt(0, 0, &myOT[0]);
    GsClearOt(0, 0, &myOT[1]);

    FntLoad(960, 0);
    FntOpen(-CENTERX, -CENTERY, SCREENXRES, SCREENYRES, 0, 512);

    GsInit3D();
    GsSetProjection(CENTERX);

    GsInitCoordinate2(WORLD, &Camera.coord2);

    GsSetAmbient(ONE/4, ONE/4, ONE/4);

    GsSetLightMode(0);

    PadInit(0);
    PCinit();
}

void PrepDisplay(){
    myActiveBuff = GsGetActiveBuff();
    GsSetWorkBase((PACKET*)myPacketArea[myActiveBuff]);
    GsClearOt(0, 0, &myOT[myActiveBuff]);
}

void Display(){
    FntFlush(-1);

    VSync(0);
    GsSwapDispBuff();
    GsSortClear(10, 10, 10, &myOT[myActiveBuff]);
    GsDrawOt(&myOT[myActiveBuff]);
}

int LinkModel(u_long *tmd, GsDOBJ2 *obj){
    u_long *dop;
    int i, NumObj;

    dop = tmd;

    dop++;
    GsMapModelingData(dop);

    dop++;
    NumObj = *dop;

    dop++;
    for(i = 0; i < NumObj; i++){
        GsLinkObject4((u_long)dop, &obj[i], i);
        obj[i].attribute = (1<<6);
    }

    return NumObj;
}

void PutObject(VECTOR pos, SVECTOR rot, GsDOBJ2 *obj){
    MATRIX lmtx, omtx;
    GsCOORDINATE2 coord;

    coord = Camera.coord2;

    RotMatrix(&rot, &omtx);
    TransMatrix(&omtx, &pos);
    CompMatrixLV(&Camera.coord2.coord, &omtx, &coord.coord);
    coord.flg = 0;

    obj->coord2 = &coord;

    GsGetLws(obj->coord2, &lmtx, &omtx);
    GsSetLightMatrix(&lmtx);
    GsSetLsMatrix(&omtx);

    GsSortObject4(obj, &myOT[myActiveBuff], 14 - OTLEN, getScratchAddr(0));
}

void CalculateCamera(){
    VECTOR vec;
    GsVIEW2 view;

    view.view = Camera.coord2.coord;
    view.super = WORLD;

    RotMatrix(&Camera.rot, &view.view);
    ApplyMatrixLV(&view.view, &Camera.pos, &vec);
    TransMatrix(&view.view, &vec);

    GsSetView2(&view);
}

unsigned char *LoadFromPC(char *filename)
{
    int fd = PCopen(filename, 0, 0);
    if(fd < 0)
    {
        printf("Error opening file\n");
    }
    int size = PClseek(fd, 0, 2);
    PClseek(fd, 0, 0);
    int bytes_read = PCread(fd, &buffer[buffer_offset], size);
    unsigned char *ret_ptr = &buffer[buffer_offset];
    if(bytes_read < 0)
    {
        printf("Error reading file\n");
    }
    buffer_offset += bytes_read;

    return ret_ptr;

}

int main()
{
    int PadStatus;
    VECTOR obj_pos={0};
    SVECTOR obj_rot={0};

    VECTOR bulb_pos={0};
    SVECTOR bulb_rot={0};

    obj_pos.vz = -800;
	obj_pos.vy = -400;

    bulb_pos.vz = -800;
    bulb_pos.vy = -400;

    Player.x = ONE*-640;
	Player.y = ONE*510;
	Player.z = ONE*800;
	
	Player.pan = -ONE/4;
	Player.til = -245;

    Camera.pos.vx = Player.x/ONE;
    Camera.pos.vy = Player.y/ONE;
    Camera.pos.vz = Player.z/ONE;
    Camera.rot.vy = -Player.pan;
    Camera.rot.vx = -Player.til;
	

    init(); 
    unsigned char *cube_tmd;
    cube_tmd = LoadFromPC("CUBE.TMD");
    ObjectCount += LinkModel((u_long *)cube_tmd, &Objects[ObjectCount]);
    //ObjectCount += LinkModel((u_long *)tmd_bulb, &Objects[ObjectCount]);

    Objects[0].attribute = 0; // re-enable lighting

    while(1)
    {
        PadStatus = PadRead(0);

        if (PadStatus & PADLup)		obj_pos.vy -= 6;
		if (PadStatus & PADLdown)	obj_pos.vy += 6;
		if (PadStatus & PADLleft)	obj_pos.vz -= 6;
		if (PadStatus & PADLright)	obj_pos.vz += 6;
        if (PadStatus & PADL1)		obj_pos.vx -= 6;
        if (PadStatus & PADR1)		obj_pos.vx += 6;

        if (PadStatus & PADL2)		Player.pan -= 6;
        if (PadStatus & PADR2)		Player.pan += 6;

        if (PadStatus & PADRup) Player.x -= 6;
        if (PadStatus & PADRdown) Player.x += 6;
        if (PadStatus & PADRleft) Player.z -= 6;
        if (PadStatus & PADRright) Player.z += 6;


        if (Player.pan > -50 && Player.pan < 50)
        {
            printf("Player pan: %d\n", Player.pan);
        }

        Camera.pos.vx = Player.x/ONE;
		Camera.pos.vy = Player.y/ONE;
		Camera.pos.vz = Player.z/ONE;
		Camera.rot.vy = -Player.pan;
		Camera.rot.vx = -Player.til;

        PrepDisplay();
        CalculateCamera();
        PutObject(obj_pos, obj_rot, &Objects[0]);
       // PutObject(bulb_pos, bulb_rot, &Objects[1]);
        Display();
    }
}