#version 330 core

/*
 * -=[ P1X DEMO TOOL ]=-
 * by Krzysztof Krystian Jankowski
 *
 * (c) 2023.04
 * https://github.com/w84death/p1x-demo-tool
 * */

in vec2 TexCoord;

uniform float u_time;

out vec4 FragColor;

const float u_fft=1.0;

const float WORLD_MAX = 192.0;
const float WORLD_RES = 0.001;

const float MAT_GROUND = 1.0;
const float MAT_DARKBLUE = 2.0;
const float MAT_CITY = 3.0;
const float MAT_ASPHALT = 4.0;
const float MAT_CONCRETE = 5.0;
const float MAT_CHESS = 6.0;
const float MAT_METAL = 7.0;
const float MAT_CARS = 8.0;
const float MAT_CITY2 = 9.0;

const float T_SUNRISE=6.0;
const float T_INTRO=8.0;
const float T_STOP1=16.0;
const float T_STOP2=32.0;
const float T_END=50.0;

int SCENE = 0;
const int SCENE_INTRO = 1;
const int SCENE_HELI = 2;
const int SCENE_STREETLOOK = 3;
const int SCENE_END = 4;

const float FOG_DIST = 0.05;
const vec3 FOG_COLOR = vec3(.8,.8,.8);
const float CAM_LENS_ABERRATION = 0.002;
const float CAM_LENS_DISTOR_K1 = 0.0015;
const float CAM_LENS_DISTOR_K2 = 0.0005;
const float CAM_LENS_24mm = 1.8;
const float CAM_LENS_35mm = 3.5;
const float CAM_LENS_50mm = 7.0;
const float CAM_LENS_80mm = 10.0;

/*
 * SDF BRUSHES
 * https://iquilezles.org/articles/distfunctions/
 *
 * */
float sdSphere( vec3 p, float s){
    return length(p)-s;
}

float sdBox( vec3 p, vec3 b){
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdRoundBox( vec3 p, vec3 b, float r ){
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

/*
 * SDF OPs
 *
 * */
float opUnion( float d1, float d2) { return min(d1,d2); }

float opSubtraction( float d1, float d2) { return max(-d1,d2); }

float opIntersection( float d1, float d2) { return max(d1,d2); }

float opSmoothUnion( float d1, float d2, float k) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h); }

float opSmoothSubtraction( float d1, float d2, float k) {
    float h = clamp( 0.5 - 0.5*(d2+d1)/k, 0.0, 1.0 );
    return mix( d2, -d1, h ) + k*h*(1.0-h); }

float opSmoothIntersection( float d1, float d2, float k) {
    float h = clamp( 0.5 - 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) + k*h*(1.0-h); }


vec3 opCheapBend( in vec3 p, float bend )
{
    float k = sin(u_time)*bend; // or some other amount
    float c = cos(k*p.x);
    float s = sin(k*p.x);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xy,p.z);
    return q;
}

vec3 opRep( in vec3 p, in vec3 c)
{
    vec3 q = mod(p+0.5*c,c)-0.5*c;
    return q;
}

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {
    return outputMin + ((clamp(value, inputMin, inputMax) - inputMin) / (inputMax - inputMin)) * (outputMax - outputMin);
}

/*
 * FAKE RANDOM GENERATOR
 *
 * */
float rnd(vec2 pos){
    return fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453);
}

/*
 * COMBINE SDF WORLD
 *
 * */
vec2 sdfWorld(in vec3 pos){
    float m = MAT_GROUND;
    float ground = pos.y+0.1;


    vec3 qb = vec3(mod(abs(pos.x),20.0)-10.0,
                   pos.y,
                   mod(abs(pos.z),10.0)-5.0);

    float base = sdRoundBox(qb+vec3(.0,.5,.0),vec3(5.5,1.0,3.),0.1);

    vec2 bid = vec2(floor(abs(pos.x)/10.0)-5.0,
                   floor(abs(pos.z)/5.0)-2.5);

    vec3 qbBend = opCheapBend(qb,rnd(bid)*0.04);
    float bheight = 2.0+10.0*abs(sin(u_time+rnd(bid)*.25));

    vec3 qb1 = qbBend-vec3(3.1,0.,1.6);
    vec3 qb2 = qbBend-vec3(-3.1,0.,1.6);
    vec3 qb3 = qbBend-vec3(3.1,0.,-1.6);
    vec3 qb4 = qbBend-vec3(-3.1,0.,-1.6);

    float b1=sdRoundBox(qb1,vec3(2.0,bheight,1.),0.2);
    float b2=sdRoundBox(qb2,vec3(2.0,bheight,1.),0.2);
    float b3=sdRoundBox(qb3,vec3(2.0,bheight,1.),0.2);
    float b4=sdRoundBox(qb4,vec3(2.0,bheight,1.),0.2);

    float roofrnd = .1+rnd(bid)*.8;

    float r1 = sdRoundBox(qb1-vec3(-0.5+rnd(bid+vec2(.1)),bheight,-0.5+roofrnd),vec3(roofrnd,roofrnd*1.5,roofrnd),0.1);
    roofrnd = .1+rnd(bid+vec2(1.))*.8;
    float r2 = sdRoundBox(qb2-vec3(-0.5+rnd(bid+vec2(.1)),bheight,-0.5+roofrnd),vec3(roofrnd,roofrnd*1.5,roofrnd),0.1);
    roofrnd = .1+rnd(bid+vec2(2.))*.8;
    float r3 = sdRoundBox(qb3-vec3(-0.5+rnd(bid+vec2(.1)),bheight,-0.5+roofrnd),vec3(roofrnd,roofrnd*1.5,roofrnd),0.1);
    roofrnd = .1+rnd(bid+vec2(3.))*.8;
    float r4 = sdRoundBox(qb4-vec3(-0.5+rnd(bid+vec2(.1)),bheight,-0.5+roofrnd),vec3(roofrnd,roofrnd*1.5,roofrnd),0.1);

    float roofs_ = opUnion(r1,opUnion(r2,opUnion(r3,r4)));
    float block_ = opUnion(b1,opUnion(b2,opUnion(b3,b4)));

    block_ = opUnion(block_,base);
    block_ = opSmoothUnion(block_,roofs_,.25);

    if (block_<WORLD_RES) m=MAT_CITY;
    if (base<WORLD_RES) m=MAT_CITY2;
    if (roofs_<WORLD_RES) m=MAT_CONCRETE;

    vec3 qlamps = vec3(mod(abs(pos.x),5.0)-1.6,
                   pos.y,
                   mod(abs(pos.z),5.0)-2.5);

    float lampBase = sdBox(qlamps,vec3(0.05,.1,0.05));
    float lampLeg = sdRoundBox(qlamps,vec3(0.01,2.0,0.01),0.04);
    float lampLed = sdRoundBox(qlamps-vec3(0.2,2.0,0.0),vec3(0.3,0.05,0.02),0.04);
    float lamps_ = opSmoothUnion(opSmoothUnion(lampBase,lampLeg,0.1),lampLed,0.04);

    float pavementBase = sdBox(qb+vec3(.0,0.15,.0), vec3(8.5,0.1,4.5));
    float pavementCut = sdBox(qb, vec3(7.5,0.2,3.5));
    float pavement_ = opSubtraction(pavementCut,pavementBase);

    if (lamps_<WORLD_RES) m=MAT_CONCRETE;
    if (pavement_<WORLD_RES) m=MAT_CONCRETE;

    vec3 qcarsL = vec3(mod(-pos.x,20.0)-0.8,
                   pos.y,
                   mod(abs(pos.z+u_time*5.0),2.0)-1.);
    vec3 qcarsR = vec3(mod(pos.x,20.0)-0.8,
                   pos.y,
                   mod(abs(pos.z-u_time*5.0),2.0)-1.);
    vec2 carLid = vec2(floor(abs(pos.x)/20.0)-0.8,
                   floor(abs(pos.z+u_time*5.0)/2.0)-1.);
    vec2 carRid = vec2(floor(abs(pos.x)/20.0)-0.8,
                   floor(abs(pos.z-u_time*5.0)/2.0)-1.);

    float rndL = rnd(carLid)*.1;
    float rndR = rnd(carRid)*.1;
    float carsL = opSubtraction(
        sdBox(qcarsL-vec3(.0,-.25,0.), vec3(1,.2,1.)),
        sdRoundBox(qcarsL-vec3(.0,-.1+rndL*4.,0.), vec3(.12,.1+rndL*4.,.3+rndL),0.2)
    );
    float carsR = opSubtraction(
        sdBox(qcarsR-vec3(.0,-.25,0.), vec3(1.,.2,1.)),
        sdRoundBox(qcarsR-vec3(.0,-.1+rndR*4.,0.), vec3(.12,.1+rndR*4.,.3+rndR),0.2)
    );

    float carSpd = u_time*5.0;
    vec3 qcars2L = vec3(mod(abs(pos.x+carSpd),2.0)-1.,
                   pos.y-5.2,
                   mod(-pos.z,10.0)-0.6);
    vec3 qcars2R = vec3(mod(abs(pos.x-carSpd),2.0)-1.,
                   pos.y-5.2,
                   mod(pos.z,10.0)-0.6);

    vec2 car2Lid = vec2(floor(abs(pos.x+carSpd)/2.0)-1.,
                   floor(abs(pos.z)/10.0)-.8);
    vec2 car2Rid = vec2(floor(abs(pos.x-carSpd)/2.0)-1.,
                   floor(abs(pos.z)/10.0)-.8);

    float rnd2L = rnd(car2Lid)*.1;
    float rnd2R = rnd(car2Rid)*.1;

    float cars2L = opSubtraction(
        sdBox(qcars2L-vec3(.0,-.2,0.), vec3(1.,.4,1.)),
        sdRoundBox(qcars2L-vec3(.0,-.1+rnd2L*4.,0.), vec3(.1+rnd2L*4.,.3+rnd2L,.12),0.2)
    );
    float cars2R = opSubtraction(
        sdBox(qcars2R-vec3(.0,-.2,0.), vec3(1.,.4,1.)),
        sdRoundBox(qcars2R-vec3(.0,-.1+rnd2R*4.,0.), vec3(.1+rnd2R*4.,.3+rnd2R,.12),0.2)
    );

    float cars_ = opUnion(
        opUnion(carsL,carsR),
        opUnion(cars2L,cars2R)
    );

    if(cars_<WORLD_RES) m=MAT_CARS;

    vec3 qhiRoad = vec3(mod(pos.x,5.0)-2.5,
                   pos.y-5.0,
                   mod(abs(pos.z-5.),10.0)-5.);

    float hiRoad = opUnion(
        opUnion(
            sdBox(qhiRoad-vec3(.0,.0,.0),vec3(2.5,.1,1.4)),
            sdBox(qhiRoad-vec3(.0,-.15,.0),vec3(2.5,.1,1.))),
        opSubtraction(
            sdRoundBox(qhiRoad-vec3(.0,.3,.0),vec3(2.5,.6,1.),.3),
            sdRoundBox(qhiRoad-vec3(.0,.3,.0),vec3(2.5,.5,1.6),.1)
        )
    );
    float hiBase = opSubtraction(
        sdRoundBox(qhiRoad-vec3(.0,-3.2,.0),vec3(.2,2.6,.8),.2),
        sdRoundBox(qhiRoad-vec3(.0,-3.,.0),vec3(.1,2.8,1.2),.1)
    );
    float hiRoads_ = opUnion(hiRoad, hiBase);

    if(hiBase<WORLD_RES) m=MAT_CONCRETE;
    if(hiRoad<WORLD_RES) m=MAT_ASPHALT;

    float city_ = opUnion(opUnion(opUnion(block_,pavement_), lamps_),cars_);
    city_ = opUnion(city_, hiRoads_);

    pos -= vec3(.0,map(u_time,.0,T_INTRO*.8,15.,1.),-15.0);

    float p1 = sdBox(pos-vec3(-0.7,0.95,.0),vec3(0.05,0.95,0.05));
    float p2 = sdBox(pos-vec3(-0.5,1.85,.0),vec3(0.15,0.05,0.05));
    float p3 = sdBox(pos-vec3(-0.5,0.95,.0),vec3(0.15,0.05,0.05));
    float p4 = sdBox(pos-vec3(-0.3,1.40,.0),vec3(0.05,0.50,0.05));
    float p_ = opUnion(p1,opUnion(p4,opUnion(p2,p3)));

    float i1 = sdBox(pos-vec3(.0,0.95,.0),vec3(0.05,0.95,0.05));
    float i2 = sdBox(pos-vec3(-.1,1.75,.0),vec3(0.05,0.05,0.05));
    float i_ = opUnion(i1,i2);

    float x1 = sdBox(pos-vec3(0.3,1.55,.0),vec3(0.05,0.35,0.05));
    float x2 = sdBox(pos-vec3(0.7,1.55,.0),vec3(0.05,0.35,0.05));
    float x3 = sdBox(pos-vec3(0.3,0.45,.0),vec3(0.05,0.45,0.05));
    float x4 = sdBox(pos-vec3(0.7,0.45,.0),vec3(0.05,0.45,0.05));
    float x5 = sdBox(pos-vec3(0.4,1.15,.0),vec3(0.05,0.05,0.05));
    float x6 = sdBox(pos-vec3(0.6,1.15,.0),vec3(0.05,0.05,0.05));
    float x7 = sdBox(pos-vec3(0.4,0.95,.0),vec3(0.05,0.05,0.05));
    float x8 = sdBox(pos-vec3(0.6,0.95,.0),vec3(0.05,0.05,0.05));
    float x9 = sdBox(pos-vec3(0.5,1.05,.0),vec3(0.05,0.05,0.05));
    float x_ = opUnion(x1,opUnion(x2,opUnion(x3,opUnion(x4,opUnion(x5,opUnion(x6,opUnion(x7,opUnion(x8,x9))))))));

    float p1x_=opUnion(p_,opUnion(i_,x_));
    if (p1x_<WORLD_RES) m=MAT_DARKBLUE;

    float world=opUnion(ground,opUnion(city_,p1x_));

    return vec2(world,m);
}

/*
 * NORMALS
 *
 * */
vec3 calcNormal(in vec3 pos){
    vec2 e = vec2(0.0001,0.0);
    return normalize(
        vec3(sdfWorld(pos+e.xyy).x-sdfWorld(pos-e.xyy).x,
             sdfWorld(pos+e.yxy).x-sdfWorld(pos-e.yxy).x,
             sdfWorld(pos+e.yyx).x-sdfWorld(pos-e.yyx).x));
}

/*
 * RAYCASTING
 *
 * */
vec2 castRay(in vec3 ro, vec3 rd){
    vec2 res=vec2(-1.0,-1.0);
    float t = 0.5;
    for (int i=0; i<2600; i++){
        vec2 scene = sdfWorld(ro+rd*t);
        if (abs(scene.x)<WORLD_RES){
            res=vec2(t,scene.y);
            break;
        }
        t+=scene.x;
        if(t>WORLD_MAX) break;
    }
    return res;
}

/*
 * SHOFT SHADOW
 *
 * */
float getSoftShadow( in vec3 ro, in vec3 rd, float w)
{
    float res = 1.0;
    float t = 0.01;
    for( int i=0; i<256 && t<WORLD_MAX; i++ )
    {
        float h = sdfWorld(ro + t*rd).x;
        res = min( res, h/(w*t) );
        t += clamp(h, 0.005, 0.50);
        if( res<-1.0 || t>WORLD_MAX ) break;
    }
    res = max(res,-1.0);
    return 0.25*(1.0+res)*(1.0+res)*(2.0-res);
}

/*
 * AO
 *
 * */
float getAO(in vec3 ro, vec3 normal){
    float occ=0.0;
    float weight=1.0;
    for (int i=0; i<8; i++){
        float len = 0.01+0.02*float(i*i);
        float dist = sdfWorld(ro+normal*len).x;
        occ=(len-dist)*weight;
        weight*=.85;
    }
    return 1.0-clamp(.6*occ,0.0,1.0);
}

/*
 * MATERIALS GENERATION
 *
 * */
vec3 getMaterial(vec3 p, vec3 nor, float id){
    vec3 m=vec3(.0,.0,.0);

    if(id==MAT_GROUND){
        m=vec3(-2.,-2.,-1.5);
        m+=vec3(.0,1.,.0)*vec3(mod(floor(.5+p.x*.25)*floor(.5+p.z*.5),5.0));

    }else
    if(id==MAT_DARKBLUE){
        m=vec3(0.1,.2,5.0);
    }else
    if(id==MAT_CITY){
        float crnd = rnd(vec2(floor(abs(p.x)/10.0),
                floor(abs(p.z)/15.0)));
        float win=
        mod(floor(p.x*4.0)*floor(p.y*4.0),2.0)*nor.z+
        0.1*nor.y+
        mod(floor(p.y*4.0)*floor(p.z*4.0),2.0)*nor.x;
        m=vec3(map(u_fft,0.65,1.0,0.0,4.0)*win,win+crnd,win+crnd);
    }else
    if(id==MAT_CITY2){
        float crnd = rnd(vec2(floor(abs(p.x)/10.0),
                floor(abs(p.z)/15.0)));
        m=vec3(crnd*3.);
    }else
    if(id==MAT_METAL){
        m=vec3(1.,1.,1.)*nor*.2;
    }else
    if(id==MAT_ASPHALT){
        m=vec3(-2.,-2.,-1.5);
    }else
    if(id==MAT_CARS){
        m=vec3(-1.,-1.,1.)+nor*.1;
    }
    return m;
}

vec3 getColor(vec3 pos, vec3 nor,vec3 rd, float material_id){
    // colors reducer for better color correction
    vec3 mate = vec3(0.2);

    // environment: sun, shadows, fake bounce light
    vec3 sun_pos = normalize(vec3(
        map(u_time,T_SUNRISE*.25,T_SUNRISE,-6.0,1.0),
        map(u_time,T_SUNRISE*.25,T_SUNRISE,-3.0,3.0),
        3.0));
    float sun_shadow = getSoftShadow(pos+nor*0.001, sun_pos,.1);
    float ao = getAO(pos,nor);

    float sun_dif = clamp(dot(nor,sun_pos),0.0,1.0);
    float sky_dif = clamp(0.5 + 0.5*dot(nor,vec3(0.0,1.0,0.0)),0.0,1.0);
    float bou_dif = clamp(0.5 + 0.5*dot(nor,vec3(0.0,-1.0,0.0)),0.0,1.0);

    float sunrise = map(u_time,T_SUNRISE*.25,T_SUNRISE,.01,1.0);


    vec3 col = mate*getMaterial(pos,nor,material_id);
    col *= ao;
    col += mate*vec3(5.0,3.0,2.0)*sun_dif*sun_shadow;
    col += mate*vec3(0.5,0.8,0.9)*sky_dif;
    col *= ao;
    col *= sunrise;

    return col;
}


/*
 * THE RENDERER
 *
 * */
vec3 render(in vec2 p){

    // DIRECTOR OF PHOTOGRAPHY
    float cam_fov = 4.;
    float cam_z = -u_time;
    float cam_x = 0.0;
    float cam_y = 0.5;
    float aim_x = cam_x;
    float aim_y = 0.0;
    float aim_z = -1.0;

    if(u_time<T_INTRO){
        SCENE=SCENE_INTRO;
    }else
    if(u_time<T_STOP1){
        SCENE=SCENE_STREETLOOK;
    }else
    if(u_time<T_STOP2){
        SCENE=SCENE_HELI;
    }else
    if(u_time<T_END){
        SCENE=SCENE_END;
    }else{
        SCENE=SCENE_INTRO;
    }

    if(SCENE==SCENE_INTRO){
        cam_fov = CAM_LENS_50mm;
        cam_y = map(u_time,0.0,T_INTRO*.8,25.0,2.0);
        aim_y = map(u_time,0.0,T_INTRO*.8,-2.0,0.0);
    }else
    if(SCENE==SCENE_HELI){
        cam_fov = CAM_LENS_24mm;
        cam_x = 2.;
        cam_y = 15.+sin(u_time*.2)*.1;
        aim_y = -2.;
        aim_z = -.5-abs(sin(u_time*.2))*.5;
    }else
    if(SCENE==SCENE_STREETLOOK){
        cam_fov = CAM_LENS_35mm;
        cam_x = -3.5;
        cam_y = 0.2;
        cam_z = 20.;
        aim_x = 3.;
        aim_y = sin(u_time*.1)*2.;
        aim_z = sin(1.0+u_time*.1)*2.;
    }else
    if(SCENE==SCENE_END){
        cam_fov = CAM_LENS_35mm;
        cam_y = .3;
    }

    vec3 ro = vec3(cam_x,cam_y,cam_z);
    vec3 ta = vec3(cam_x+aim_x,cam_y+aim_y,cam_z+aim_z);

    // calculate ray direction
    vec3 ww = normalize (ta-ro);
    vec3 uu = normalize( cross(ww, vec3(0,1,0)));
    vec3 vv = normalize (cross(uu,ww));
    vec3 rd = normalize(p.x*uu+p.y*vv+cam_fov*ww);

    // sky simulation
    vec3 col = vec3(map(u_time,T_SUNRISE*.5,T_SUNRISE,1.0,.0),0.75,1.0) - 0.5*rd.y;
    col =  mix(col, vec3(0.7,0.8,0.8), exp(-10.0*rd.y));
    col *= map(u_time,T_SUNRISE*.25,T_SUNRISE,.0,1.0);

    // hit ray trace
    vec2 ray = castRay(ro,rd);
    float ray_hit=ray.x;
    float material_id=ray.y;

    if (ray_hit>0.0){
        vec3 pos = ro+rd*ray_hit;
        vec3 nor = calcNormal(pos);
        col = getColor(pos,nor,rd,material_id);

        float fogAmount = 1.0 - exp( -ray_hit*FOG_DIST );
        vec3  fogColor  = FOG_COLOR* map(u_time,T_SUNRISE*.25,T_SUNRISE,.01,1.0);
        col = mix( col, fogColor, fogAmount );

    }
    return col;
}

float barrelDistortion(vec2 pos) {
    float radius = length(pos);

    return 1.0 + CAM_LENS_DISTOR_K1 * pow(radius, 2.0) + CAM_LENS_DISTOR_K2 * pow(radius, 4.0);
}

vec2 chromaticAberration(vec2 coord, float radius, vec2 direction, float strength) {
    return coord + strength * radius * direction;
}


/*
 * MAIN
 *
 * */
void main() {
    vec3 col = render(barrelDistortion(TexCoord)*TexCoord);

    float filmNoise = rnd(TexCoord+vec2(u_time));
    col -= col*vec3(filmNoise)*.2;

    // color/exposure correction;
    col = pow (col, vec3(0.4545));
    FragColor = vec4(col, 1.0);
}

/*
 * END OF LISTING
 *
 * */
