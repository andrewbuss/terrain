#define znew (z=36969*(z&65535)+(z>>16))
#define wnew (w=18000*(w&65535)+(w>>16))
#define MWC ((znew<<16)+wnew )
#define SHR3 (jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5))
#define CONG (jcong=69069*jcong+1234567)
#define FIB ((b=a+b),(a=b-a))
#define KISS ((MWC^CONG)+SHR3)
#define LFIB4 (c++,t[c]=t[c]+t[UC(c+58)]+t[UC(c+119)]+t[UC(c+178)])
#define SWB (c++,bro=(x<y),t[c]=(x=t[UC(c+34)])-(y=t[UC(c+19)]+bro))
#define UNI (KISS*2.328306e-10)
#define VNI ((long) KISS)*4.656613e-10
#define UC (unsigned char) /*a cast operation*/
typedef unsigned long UL;

class arandom{
 public:
  UL z,w,jsr,jcong,a,b,t[256],x,y,bro;
  unsigned char c;
  arandom(){
    x=0;
    y=0;
    c=0;
    settable(12345,65435,34221,12345,9983651,95746118);
    int i;
    UL k;
    for(i=1;i<1000001;i++){k=LFIB4;} printf("%u\n", k-1064612766U);
    for(i=1;i<1000001;i++){k=SWB  ;} printf("%u\n", k- 627749721U);
    for(i=1;i<1000001;i++){k=KISS ;} printf("%u\n", k-1372460312U);
    for(i=1;i<1000001;i++){k=CONG ;} printf("%u\n", k-1529210297U);
    for(i=1;i<1000001;i++){k=SHR3 ;} printf("%u\n", k-2642725982U);
    for(i=1;i<1000001;i++){k=MWC  ;} printf("%u\n", k- 904977562U);
    for(i=1;i<1000001;i++){k=FIB  ;} printf("%u\n", k-3519793928U);
    
  }
  void settable(UL i1,UL i2,UL i3,UL i4,UL i5, UL i6){ 
    z=i1;w=i2,jsr=i3; jcong=i4; a=i5; b=i6;
    for(int i=0;i<256;i=i+1) t[i]=KISS;
  }
  float nl(){
    return KISS;
  }
};

    
