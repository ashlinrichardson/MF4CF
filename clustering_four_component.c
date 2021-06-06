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


int main(int argc, char ** argv){
  if(argc < 2) err("clustering_four_component.c [path used to call MF4CF.c e.g. T3/]\n\te.g.:\n\t./clustering_four_component.exe T3");

  #define Pd P[0]
  #define Ps P[1] 
  #define Pv P[2]
  #define Pc P[3]
  
  float P[N_IN];
  char * path = argv[1];
  FILE * in_f[N_IN];
  char fn[STR_MAX];
  float class = 0;
  int i;

  for0(i, N_IN){
    path_cat(fn, path, D_fn[i]);
    printf("+r %s\n", fn);
    in_f[i] = fopen(fn, "rb");
    if(!in_f) err("failed to open input file");
  }
  
  path_cat(fn, path, "MF4C_Clustered.bin");
  FILE * out = fopen(fn, "wb");

  while(1 == fread(&P[0], sizeof(float), 1, in_f[0])){
    
    for0(i, 3)
      fread(&P[i + 1], sizeof(float), 1, in_f[i + 1]);

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

    // printf("%e %e %e %e %d\n ", Pd, Ps, Pv, Pc, (int)class);

    fwrite(&class, sizeof(float), 1, out);

  }

  fclose(out);
  return 0;
}
