/* unsupervised classification scheme. After running MF4CF.c / MF4CF.exe:

to compile:
  gcc clustering_four_component.c -o clustering_four_component.exe -lm

to run:
  ./clustering_four_component.exe T3       */
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define STR_MAX 4096
#define for0(i,n) for(i = 0; i < n; i++) /* for loop shorthand */

char sep(){
  #ifdef _WIN32
    return '\\'; /* windows path separator */
  #else
    return '/'; /* posix path sep */
  #endif
}

void err(const char * m){
  printf("Error: %s\n", m);
  exit(1); /* print message and bail */
}

void path_cat(char * fn, char * path, const char * f){
  strcpy(fn, path);
  char s = sep();
  int ix = strlen(path);
  if(fn[ix - 1] == s) ix --;
  fn[ix] = s;
  strcpy(fn + ix + 1, f);
}

#define N_IN 4 // number of input files

const char* D_fn[] = {"pd_f.bin",
                      "ps_f.bin",
                      "pv_f.bin",
                      "pc_f.bin"};

float max_f(float a, float b, float c){
  return fmax(fmax(a,b), fmax(b,c));
}

float min_f(float a, float b, float c){
  return fmin(fmin(a,b), fmin(b,c));
}

int rgb_to_hsv(float * src_r, float * src_g, float * src_b, float * dst_h, float * dst_s, float * dst_v){
    float r = (*src_r) / 255.; // rgb \in [0, 255]
    float g = (*src_g) / 255.;
    float b = (*src_b) / 255.; // h:0-360.0, s:0.0-1.0, v:0.0-1.0
    
    float max = max_f(r, g, b);
    float min = min_f(r, g, b);
    float h, s, v;
    v = max;

    if (max == 0.0f || (max - min == 0.)) s = h = 0.;
    else {
        s = (max - min) / max;
        if (max == r) h = 60 * ((g - b) / (max - min)) + 0;
        else if (max == g) h = 60 * ((b - r) / (max - min)) + 120;
        else h = 60 * ((r - g) / (max - min)) + 240;
    }
    if (h < 0) h += 360.;
    *dst_h = h;   // dst_h : 0-360
    *dst_s = s * 255.; // dst_s : 0-255
    *dst_v = v * 255.; // dst_v : 0-255
    return 0;
}

int hsv_to_rgb(float *r, float *g, float *b, float h, float s, float v){
  // h \in [0, 360]. s, v \in [0, 1]
  if( (h>360.)||(h<0.)){
    printf("H: HSV out of range %f %f %f\n", h, s, v);
    return 1.;
  }
  if((s<0.)||(s>1.)){
    printf("S: HSV out of range %f %f %f\n", h, s, v);
    return 1.;
  }
  if((v<0.)||(v>1.)){
    printf("V: HSV out of range %f %f %f\n", h, s, v);
    return 1.;
  }
  if(h == 360.) h = 0.;
  
  int i;
  float f, p, q, t;
  if( s == 0 ) {
    // achromatic (grey)
    *r = *g = *b = v;
    return 0;
  }
  float H,S,V;
  H = h; V = v; S = s;
  h /= 60.; // sector 0 to 5
  i = (int)floor(h);
  f = h - i; // factorial part of h
  p = v * (1. - s );
  q = v * (1. - s * f );
  t = v * (1. - s * (1. - f) );
  switch(i) {
    case 0: *r = v; *g = t; *b = p; break;
    case 1: *r = q; *g = v; *b = p; break;
    case 2: *r = p; *g = v; *b = t; break;
    case 3: *r = p; *g = q; *b = v; break;
    case 4: *r = t; *g = p; *b = v; break;
    case 5: *r = v; *g = p; *b = q; break;
    default: printf("\nERROR HSV to RGB"); printf("i=%d hsv= %f %f %f\n", i, H, S, V);
  }
  return 0;
}

int main(int argc, char ** argv){
  if(argc < 2) err("clustering_four_component.c [path used to call MF4CF.c e.g. T3/]\n\te.g.:\n\t./clustering_four_component.exe T3");

  #define Pd P[0]
  #define Ps P[1]
  #define Pv P[2]
  #define Pc P[3]

  char * path = argv[1];
  FILE * in_f[N_IN];
  char fn[STR_MAX];
  float class = 0;
  float P[N_IN];
  int i;

  for0(i, N_IN){
    path_cat(fn, path, D_fn[i]);
    printf("+r %s\n", fn);
    in_f[i] = fopen(fn, "rb");
    if(!in_f) err("failed to open input file");
  }

  path_cat(fn, path, "MF4C_Clustered.bin"); FILE * out = fopen(fn, "wb");
  path_cat(fn, path, "r.bin"); FILE * out_r = fopen(fn, "wb");
  path_cat(fn, path, "g.bin"); FILE * out_g = fopen(fn, "wb");
  path_cat(fn, path, "b.bin"); FILE * out_b = fopen(fn, "wb");

  float r, g, b;
  while(1 == fread(&P[0], sizeof(float), 1, in_f[0])){

    for0(i, 3) fread(&P[i + 1], sizeof(float), 1, in_f[i + 1]);
    class = 0;
    if(Pd > Ps && Ps > Pv && Pv > Pc) class = 1;
    if(Pd > Ps && Ps > Pc && Pc > Pv) class = 2;
    if(Pd > Pv && Pv > Ps && Ps > Pc) class = 3;
    if(Pd > Pv && Pv > Pc && Pc > Ps) class = 4;
    if(Pd > Pc && Pc > Ps && Ps > Pv) class = 5;
    if(Pd > Pc && Pc > Pv && Pv > Ps) class = 6;
    if(Ps > Pd && Pd > Pv && Pv > Pc) class = 7;
    if(Ps > Pd && Pd > Pc && Pc > Pv) class = 8;
    if(Ps > Pv && Pv > Pd && Pd > Pc) class = 9;
    if(Ps > Pv && Pv > Pc && Pc > Pd) class = 10;
    if(Ps > Pc && Pc > Pd && Pd > Pv) class = 11;
    if(Ps > Pc && Pc > Pv && Pv > Pd) class = 12;
    if(Pv > Pd && Pd > Ps && Ps > Pc) class = 13;
    if(Pv > Pd && Pd > Pc && Pc > Ps) class = 14;
    if(Pv > Ps && Ps > Pd && Pd > Pc) class = 15;
    if(Pv > Ps && Ps > Pc && Pc > Pd) class = 16;
    if(Pv > Pc && Pc > Pd && Pd > Ps) class = 17;
    if(Pv > Pc && Pc > Ps && Ps > Pd) class = 18;
    if(Pc > Pd && Pd > Ps && Ps > Pv) class = 19;
    if(Pc > Pd && Pd > Pv && Pv > Ps) class = 20;
    if(Pc > Ps && Ps > Pd && Pd > Pv) class = 21;
    if(Pc > Ps && Ps > Pv && Pv > Pd) class = 22;
    if(Pc > Pv && Pv > Pd && Pd > Ps) class = 23;
    if(Pc > Pv && Pv > Ps && Ps > Pd) class = 24;

    fwrite(&class, sizeof(float), 1, out);

    r = g = b = 0;
    if((int)(class / 6) == 0) r = 255;
    if((int)(class / 6) == 1) b = 255;
    if((int)(class / 6) == 2) g = 255;
    if((int)(class / 6) == 3){
        r = 255;
        b = 255;
    }

    float h, s, v;
    rgb_to_hsv(&r,&g,&b,&h,&s,&v);
    s *= (6. -  (((int)class) % 6)) / 6.;
    hsv_to_rgb(&r, &g, &b, h, s/255., v/255.);

    fwrite(&r, sizeof(float), 1, out_r);
    fwrite(&g, sizeof(float), 1, out_g);
    fwrite(&b, sizeof(float), 1, out_b);

  }
  fclose(out_r);
  fclose(out_g);
  fclose(out_b);
  fclose(out);
  return 0;
}
