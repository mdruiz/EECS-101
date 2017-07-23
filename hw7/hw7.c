#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS	(int)480
#define COLS	(int)480

#define midX	(int)240
#define midY	(int)240

void clear( unsigned char image[][COLS] );
void header( int row, int col, unsigned char head[32] );
double dot_product(double v[], double u[]);


int main( int argc, char **argv ) {
	
	int				i, j, k, x, y;
	double			alpha, H[3], p, q, L1,Ls;
	double 		r[9] = {50,50,50,10,100,50,50,50,50};
    double			N[3];
	double 		viewDirection[3] = {0,0,1};
	double 		s[9][3] = {{0,0,1}, {1/sqrt(3), 1/sqrt(3), 1/sqrt(3)}, {1, 0, 0}, {0, 0, 1},{0, 0, 1},{0, 0, 1},{0, 0, 1},{0, 0, 1},{0, 0, 1}};
	double 		a[9] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.1, 1, 0.5, 0.5};
	double 		m[9] = {1, 1, 1, 1, 1, 1, 1, 0.1, 10000};
	double 		imageValue[ROWS][COLS], normalize;
	FILE				*fp;
	unsigned char	image[ROWS][COLS], head[32];
	char				filename[9][50], ch;
	
	header(ROWS, COLS, head);
	
	strcpy(filename[0], "image_A");
	strcpy(filename[1], "image_B");
	strcpy(filename[2], "image_C");
	strcpy(filename[3], "image_D");
	strcpy(filename[4], "image_E");
	strcpy(filename[5], "image_F");
	strcpy(filename[6], "image_G");
	strcpy(filename[7], "image_H");
	strcpy(filename[8], "image_I");
	
	for(k= 0; k< 9; k++) {
		
	//initializing image to be all black
		for(i = 0; i < ROWS; i++){
			for(j = 0; j < COLS; j++){
				image[i][j] = 0;
				imageValue[i][j] = image[i][j];
			}
		}

		//calculating angular bisector = viewDirection + source direction
        for(i = 0; i < 3; i++){
            H[i] = (viewDirection[i]+s[k][i]);
        }
		
        normalize = sqrt(H[0]*H[0] + H[1]*H[1] + H[2]*H[2]);
        
		//normalizing bisector
		for(i = 0; i < 3; i++){
            H[i] = H[i]/normalize;
        }
	
	
	for(i = 0; i < ROWS; i++){
		for(j = 0; j < COLS; j++){
			x = j - midX;
			y = midY - i;
			p = -x / sqrt(r[k]*r[k] -(x*x + y*y));
			q = -y / sqrt(r[k]*r[k] -(x*x + y*y));
			N[0] = -p /sqrt(p*p + q*q +1);
			N[1] = -q /sqrt(p*p + q*q +1);
			N[2] = 1 /sqrt(p*p + q*q +1);
			L1 = dot_product(N, s[k]);
			if(L1 >= 0){
				alpha = acos(dot_product(N, H));
				Ls = exp( -((alpha/m[k]) * (alpha/m[k])));
				imageValue[i][j] = a[k]*L1 + (1 - a[k])*Ls ;
				image[i][j] = imageValue[i][j]  * 255;
            }
		}
	}
	
	if(!( fp = fopen( strcat( filename[k], ".ras"), "wb") ))
	{
		fprintf( stderr, "error: could not open %s\n", filename );
		exit( 1 );
	}		
	fwrite(head, 4, 8, fp);
	for( i = 0; i< ROWS; i++) fwrite(image[i], 1, COLS, fp);
	fclose(fp);
		

	}
	
	return 0;

}


double dot_product(double v[3], double u[3])
{
	double result = 0.0;
	int i;
	for(i = 0; i < 3; i++)
		result += v[i] * u[i];
	return result;
}

void clear( unsigned char image[][COLS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */
	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch ++; 
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++; 
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;
	
	ch = (char*)&num;
	head[19] = *ch;
	ch ++; 
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
	head[16] = *ch;
	
	/* Big-endian for unix */
/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;

*/
}
