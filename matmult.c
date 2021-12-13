/**********************************************/
/*                                            */
/*  Produit de matrice par double diffusion   */
/*                                            */
/**********************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>
#include <stdarg.h>

/******* Fonctions d'affichage ***********/
#define VOIRD(expr) do {printf("P%d (%d,%d) <%.3d> : \t{ " #expr " = %d }\n", \
                               my_id,i_row,i_col,__LINE__,expr);fflush(NULL);} while(0)
#define PRINT_MESSAGE(...) FPRINT_MESSAGE(my_id,i_row,i_col, __LINE__,  __VA_ARGS__)

static void FPRINT_MESSAGE(int id, int row, int col, int line, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  printf("P%d (%d,%d) <%.3d> : \t",id,row,col,line);
  vprintf(fmt, ap);
  fflush(NULL);
  va_end(ap);
  return;
}


typedef struct { 
  int row;   /* le nombre de lignes   */
  int col;   /* le nombre de colonnes */
  double* data; /* les valeurs           */
} matrix_t;

int my_id, nb_proc;
int nb_col,nb_row;       /* Largeur et hauteur de la grille */
MPI_Comm MPI_HORIZONTAL; /* Communicateur pour diffusions horizontales */
MPI_Comm MPI_VERTICAL;   /* Communicateur pour diffusions verticales */
int i_col,i_row;         /* Position dans la grille */


/*******************************************/
/* initialisation aléatoire de la matrice  */
/*******************************************/
void mat_init_alea(matrix_t *mat,int width, int height)
{
  int i;

  mat->row = height;
  mat->col = width;

  mat->data=(double*)calloc(height*width,sizeof(double));
  for(i=0 ; i<height*width ; i++)
    mat->data[i]=1.0*rand()/(RAND_MAX+1.0);
}

/*******************************************/
/* initialisation à 0 de la matrice        */
/*******************************************/
void mat_init_empty(matrix_t *mat,int width, int height)
{
  mat->row = height;
  mat->col = width;
  mat->data=(double*)calloc((mat->row)*(mat->col),sizeof(double));
}

/*******************************************/
/* affichage de la matrice                 */
/*******************************************/
void mat_display(matrix_t A)
{
  int i,j,t=0;

  printf("      ");
  for(j=0;j<A.col;j++)
    printf("%.3d ",j);
  printf("\n");
  printf("    __");
  for(j=0;j<A.col;j++)
    printf("____");
  printf("_\n");

  for(i=0;i<A.row;i++)
    {
      printf("%.3d | ",i);
      for(j=0;j<A.col;j++)
	printf("%.3g ",A.data[t++]);
      printf("|\n");
    }
  printf("    --");
  for(j=0;j<A.col;j++)
    printf("----");
  printf("-\n");
}

/*******************************************/
/* C+=A*B                                  */
/*******************************************/
void mat_mult(matrix_t A, matrix_t B, matrix_t C)
{
  int i,j,k,M,N,K;
  double *_A,*_B,*_C;
 
  _A=A.data;
  _B=B.data;
  _C=C.data;

  M = C.row;
  N = C.col;
  K = A.col;

  if((M!=A.row) || (N!=B.col) || (K!=B.row)) {
    PRINT_MESSAGE("Attention, tailles incompatibles");
    VOIRD(A.row);VOIRD(A.col);VOIRD(B.row);
    VOIRD(C.col);VOIRD(C.row);VOIRD(C.col);
    exit(1);
  }

  for(i=0 ; i<M ; i++)
    for(j=0 ; j<N ; j++)
      for(k=0 ; k<K ; k++) 
	_C[i*N+j]+=_A[i*K+k]*_B[k*N+j];
}

/*******************************************/
/* Initialisation de la grille             */
/*******************************************/
void init_communicateurs(void)
{
  int new_rank_h;
  int new_rank_v;

  nb_col=nb_proc; nb_row=1;
  /* Améliorez la disposition */
  for (int i = sqrt(nb_proc); i >= 1; i--)
  {
    if (nb_proc%i == 0)
    {
      nb_row = i;
      nb_col = nb_proc / nb_row;  
      break;
    }
    
  }
  
  i_col = my_id % nb_col;
  i_row = my_id % nb_row;
  
  
  /* Partitionnez MPI_COMM_WORLD en communicateurs pour les lignes 
     et en communicateurs pour les colonnes */
  MPI_Comm_split(MPI_COMM_WORLD, i_row, i_col, &MPI_HORIZONTAL);
  MPI_Comm_split(MPI_COMM_WORLD, i_col, i_row, &MPI_VERTICAL);
  
  /* Vérifiez que votre rang dans le nouveau communicateur 
     correspond bien à celui que vous attendez. */
  MPI_Comm_rank(MPI_HORIZONTAL, &new_rank_h);
  MPI_Comm_rank(MPI_VERTICAL, &new_rank_v);

  printf("%d = %d",new_rank_h, i_col);
  printf("%d = %d",new_rank_v, i_row);
}

/*******************************************/
/* Produit de matrice par double diffusion */
/*******************************************/
void parallel_mat_mult(matrix_t A, matrix_t B, matrix_t C)
{
  matrix_t buf_col, buf_row;
  int k;  

  mat_init_empty(&buf_col,1,C.row);
  mat_init_empty(&buf_row,C.col,1);

  for(k=0; k<(nb_col*C.col) ; k++) {
    /* diffusion horizontale */
    if(k/A.col == i_col) {
      int i;
      for(i=0;i<C.row;i++) 
	buf_col.data[i]=A.data[i*A.col+(k%A.col)];
    }
    MPI_Bcast(buf_col.data, C.row, MPI_INT, k/A.col, MPI_HORIZONTAL);

    /* diffusion verticale */
    if(k/B.row == i_row) {
      int j;
      for(j=0;j<C.col;j++) 
	buf_row.data[j]=B.data[(k%B.row)*B.col+j];
    }
    MPI_Bcast(buf_row.data, C.col, MPI_INT, k/B.row, MPI_VERTICAL);

    /* produit de matrices en local */
    mat_mult(buf_col, buf_row,C);
  }
}


int main(int argc, char **argv) 
{
  int taille=0;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  
  if(argc==0) {
    fprintf(stderr, "Usage : %s <taille de matrice>", argv[0]);
    return 1;
  } else {
    taille = atoi(argv[1]);
  }
  
  init_communicateurs();
  
  {
    matrix_t A,B,C;
    
    mat_init_alea(&A,taille/nb_col,taille/nb_row);
    mat_init_alea(&B,taille/nb_col,taille/nb_row);
    mat_init_empty(&C,taille/nb_col,taille/nb_row);
    parallel_mat_mult(A,B,C);
    if(my_id==0) mat_display(C);
  }
  MPI_Finalize();
  
  return (0);
}
